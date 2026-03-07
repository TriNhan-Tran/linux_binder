#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include "binder.h"

struct binder_state *binder_open(const char* driver)
{
    struct binder_state *bs;
    bs = (struct binder_state *)malloc(sizeof(*bs));
    if (!bs) return NULL;

    bs->fd = open(driver, O_RDWR | O_CLOEXEC);
    if (bs->fd < 0) {
        LOGE("Cannot open %s (%s)", driver, strerror(errno));
        goto fail_open;
    }

    bs->mapsize = 1 * 1024 * 1024; // 1MB default size
    bs->mapped = mmap(NULL, bs->mapsize, PROT_READ, MAP_PRIVATE, bs->fd, 0);
    if (bs->mapped == MAP_FAILED) {
        LOGE("Cannot mmap binder (%s)", strerror(errno));
        goto fail_map;
    }

    return bs;

fail_map:
    close(bs->fd);
fail_open:
    free(bs);
    return NULL;
}

void binder_close(struct binder_state *bs)
{
    if (!bs) return;
    munmap(bs->mapped, bs->mapsize);
    close(bs->fd);
    free(bs);
}

int binder_become_context_manager(struct binder_state *bs)
{
    return ioctl(bs->fd, BINDER_SET_CONTEXT_MGR, 0);
}

static int binder_write(struct binder_state *bs, void *data, size_t len)
{
    struct binder_write_read bwr;
    memset(&bwr, 0, sizeof(bwr));
    bwr.write_size = len;
    bwr.write_buffer = (binder_uintptr_t)data;

    int res = ioctl(bs->fd, BINDER_WRITE_READ, &bwr);
    if (res < 0) {
        LOGE("binder_write: ioctl failed (%s)", strerror(errno));
    }
    return res;
}

void binder_free_buffer(struct binder_state *bs, binder_uintptr_t buffer_ptr)
{
    struct {
        uint32_t cmd;
        binder_uintptr_t buffer;
    } __attribute__((packed)) cmd_free;

    cmd_free.cmd = BC_FREE_BUFFER;
    cmd_free.buffer = buffer_ptr;

    binder_write(bs, &cmd_free, sizeof(cmd_free));
}

static int binder_send_ptr_cookie_cmd(struct binder_state *bs,
                                      uint32_t cmd,
                                      const struct binder_ptr_cookie *pc)
{
    struct {
        uint32_t cmd;
        struct binder_ptr_cookie pc;
    } __attribute__((packed)) wd;

    wd.cmd = cmd;
    wd.pc = *pc;
    return binder_write(bs, &wd, sizeof(wd));
}

static int binder_handle_ref_command(struct binder_state *bs,
                                     uint32_t cmd,
                                     const struct binder_ptr_cookie *pc)
{
    if (cmd == BR_INCREFS) {
        return binder_send_ptr_cookie_cmd(bs, BC_INCREFS_DONE, pc);
    }
    if (cmd == BR_ACQUIRE) {
        return binder_send_ptr_cookie_cmd(bs, BC_ACQUIRE_DONE, pc);
    }
    return 0;
}

/*
 * Consume BR commands that are handled identically by both loop parser and
 * synchronous transact parser.
 *
 * Returns:
 *   1  -> command handled and cursor consumed
 *   0  -> caller must handle command
 *  -1  -> error while handling command
 */
static int binder_consume_common_br_command(struct binder_state *bs,
                                            uint32_t cmd,
                                            uint8_t **cursor)
{
    switch (cmd) {
    case BR_NOOP:
    case BR_TRANSACTION_COMPLETE:
    case BR_SPAWN_LOOPER:
    case BR_FINISHED:
    case BR_ONEWAY_SPAM_SUSPECT:
    case BR_TRANSACTION_PENDING_FROZEN:
        return 1;

    case BR_ERROR:
    case BR_ACQUIRE_RESULT:
        *cursor += sizeof(int32_t);
        return 1;

    case BR_DEAD_BINDER:
    case BR_CLEAR_DEATH_NOTIFICATION_DONE:
        if (cmd == BR_DEAD_BINDER) {
            LOGE("BR_DEAD_BINDER");
        }
        *cursor += sizeof(binder_uintptr_t);
        return 1;

    case BR_INCREFS:
    case BR_ACQUIRE:
    case BR_RELEASE:
    case BR_DECREFS: {
        struct binder_ptr_cookie *pc = (struct binder_ptr_cookie *)(*cursor);
        if (binder_handle_ref_command(bs, cmd, pc) < 0) {
            return -1;
        }
        *cursor += sizeof(*pc);
        return 1;
    }

    default:
        return 0;
    }
}

static void binder_acquire_handles_in_transaction(struct binder_state *bs,
                                                  const struct binder_transaction_data *txn)
{
    if (!bs || !txn) {
        return;
    }
    if (txn->offsets_size == 0 || txn->data.ptr.offsets == 0 || txn->data.ptr.buffer == 0) {
        return;
    }

    const binder_size_t *offsets =
        (const binder_size_t *)(uintptr_t)txn->data.ptr.offsets;
    const uint8_t *buffer =
        (const uint8_t *)(uintptr_t)txn->data.ptr.buffer;
    const size_t offsets_count = txn->offsets_size / sizeof(binder_size_t);

    for (size_t i = 0; i < offsets_count; ++i) {
        binder_size_t off = offsets[i];
        if (off + sizeof(struct flat_binder_object) > txn->data_size) {
            LOGE("binder_acquire_handles_in_transaction: invalid object offset %llu",
                 (unsigned long long)off);
            continue;
        }

        const struct flat_binder_object *obj =
            (const struct flat_binder_object *)(buffer + off);
        if (obj->hdr.type == BINDER_TYPE_HANDLE && obj->handle != 0) {
            if (binder_acquire_handle(bs, obj->handle) < 0) {
                LOGE("binder_acquire_handles_in_transaction: acquire failed for handle %u",
                     obj->handle);
            }
        }
    }
}

static int binder_parse_ctx(struct binder_state *bs,
                            uintptr_t ptr, size_t size,
                            binder_handler_ctx_func func, void *context)
{
    uint8_t *p = (uint8_t *)ptr;
    uint8_t *end = p + size;

    while (p < end) {
        uint32_t cmd = *(uint32_t *)p;
        p += sizeof(uint32_t);

        int common_result = binder_consume_common_br_command(bs, cmd, &p);
        if (common_result < 0) {
            LOGE("binder_parse_ctx: failed to ack ref cmd=%u", cmd);
            return -1;
        }
        if (common_result > 0) {
            continue;
        }

        switch (cmd) {
        case BR_TRANSACTION: {
            struct binder_transaction_data *txn = (struct binder_transaction_data *)p;
            if (func) {
                func(bs, txn, context);
            }
            if (txn->data.ptr.buffer) {
                binder_free_buffer(bs, txn->data.ptr.buffer);
            }
            p += sizeof(*txn);
            break;
        }
        case BR_TRANSACTION_SEC_CTX: {
            struct binder_transaction_data_secctx *txn_sec =
                (struct binder_transaction_data_secctx *)p;
            struct binder_transaction_data *txn = &txn_sec->transaction_data;
            if (func) {
                func(bs, txn, context);
            }
            if (txn->data.ptr.buffer) {
                binder_free_buffer(bs, txn->data.ptr.buffer);
            }
            p += sizeof(*txn_sec);
            break;
        }
        case BR_REPLY: {
            struct binder_transaction_data *txn = (struct binder_transaction_data *)p;
            if (txn->data.ptr.buffer) {
                binder_free_buffer(bs, txn->data.ptr.buffer);
            }
            p += sizeof(*txn);
            break;
        }
        case BR_FROZEN_REPLY:
            LOGE("BR_FROZEN_REPLY");
            break;
        case BR_FAILED_REPLY:
            LOGE("BR_FAILED_REPLY");
            break;
        case BR_DEAD_REPLY:
            LOGE("BR_DEAD_REPLY");
            break;
        default:
            return -1;
        }
    }
    return 0;
}

void binder_loop_ctx(struct binder_state *bs, binder_handler_ctx_func func, void *context)
{
    int res;
    struct binder_write_read bwr;
    uint8_t read_buffer[32*1024];

    uint32_t cmd = BC_ENTER_LOOPER;
    binder_write(bs, &cmd, sizeof(cmd));

    LOGI("Binder Loop (ctx): Entered");

    for (;;) {
        bwr.read_size = sizeof(read_buffer);
        bwr.read_consumed = 0;
        bwr.read_buffer = (binder_uintptr_t)read_buffer;

        bwr.write_size = 0;
        bwr.write_consumed = 0;
        bwr.write_buffer = 0;

        res = ioctl(bs->fd, BINDER_WRITE_READ, &bwr);

        if (res < 0) {
            LOGE("binder_loop_ctx: ioctl failed (%s)", strerror(errno));
            break;
        }

        res = binder_parse_ctx(bs, (uintptr_t)read_buffer, bwr.read_consumed, func, context);
        if (res != 0) {
            LOGE("binder_loop_ctx: parse error");
            break;
        }
    }
}

/* ------------------------------------------------------------------ */
/* Extended API with flat_binder_object offset support                */
/* ------------------------------------------------------------------ */

int binder_transact2(struct binder_state *bs,
                     uint32_t target_handle,
                     uint32_t code,
                     const void *data, size_t data_size,
                     const binder_size_t *offsets, size_t offsets_size,
                     void *reply_buf, size_t reply_buf_size,
                     size_t *reply_size)
{
    struct binder_transaction_data tr;
    memset(&tr, 0, sizeof(tr));

    tr.target.handle = target_handle;
    tr.code = code;
    tr.flags = 0;

    tr.data_size = data_size;
    tr.data.ptr.buffer = (binder_uintptr_t)data;
    tr.data.ptr.offsets = (binder_uintptr_t)offsets;
    tr.offsets_size = offsets_size;

    struct {
        uint32_t cmd;
        struct binder_transaction_data txn;
    } __attribute__((packed)) wd;

    wd.cmd = BC_TRANSACTION;
    wd.txn = tr;

    struct binder_write_read bwr;
    uint8_t read_buffer[32*1024];

    memset(&bwr, 0, sizeof(bwr));
    bwr.write_size = sizeof(wd);
    bwr.write_buffer = (binder_uintptr_t)&wd;
    bwr.read_size = sizeof(read_buffer);
    bwr.read_buffer = (binder_uintptr_t)read_buffer;

    int res = ioctl(bs->fd, BINDER_WRITE_READ, &bwr);
    if (res < 0) {
        LOGE("binder_transact2: ioctl failed (%s)", strerror(errno));
        return -1;
    }

    *reply_size = 0;

    uint8_t *p = read_buffer;
    uint8_t *end = p + bwr.read_consumed;
    int got_reply = 0;

    while (!got_reply) {
        while (p < end) {
            uint32_t cmd = *(uint32_t *)p;
            p += sizeof(uint32_t);

            int common_result = binder_consume_common_br_command(bs, cmd, &p);
            if (common_result < 0) {
                LOGE("binder_transact2: failed to ack ref cmd=%u", cmd);
                return -1;
            }
            if (common_result > 0) {
                continue;
            }

            switch (cmd) {
            case BR_TRANSACTION: {
                struct binder_transaction_data *txn = (struct binder_transaction_data *)p;
                if (txn->data.ptr.buffer) {
                    binder_free_buffer(bs, txn->data.ptr.buffer);
                }
                p += sizeof(*txn);
                break;
            }
            case BR_TRANSACTION_SEC_CTX: {
                struct binder_transaction_data_secctx *txn_sec =
                    (struct binder_transaction_data_secctx *)p;
                struct binder_transaction_data *txn = &txn_sec->transaction_data;
                if (txn->data.ptr.buffer) {
                    binder_free_buffer(bs, txn->data.ptr.buffer);
                }
                p += sizeof(*txn_sec);
                break;
            }
            case BR_REPLY: {
                struct binder_transaction_data *rxn = (struct binder_transaction_data *)p;
                if (rxn->data_size > 0 && reply_buf) {
                    size_t copy_sz = rxn->data_size < reply_buf_size
                                     ? rxn->data_size : reply_buf_size;
                    memcpy(reply_buf, (void *)(uintptr_t)rxn->data.ptr.buffer, copy_sz);
                    *reply_size = copy_sz;
                }
                binder_acquire_handles_in_transaction(bs, rxn);
                if (rxn->data.ptr.buffer) {
                    binder_free_buffer(bs, rxn->data.ptr.buffer);
                }
                p += sizeof(*rxn);
                got_reply = 1;
                break;
            }
            case BR_DEAD_REPLY:
            case BR_FAILED_REPLY:
                LOGE("binder_transact2: reply error (cmd=%u)", cmd);
                return -1;
            default:
                LOGE("binder_transact2: unknown BR cmd=%u", cmd);
                p = end;
                break;
            }
        }

        if (!got_reply) {
            memset(&bwr, 0, sizeof(bwr));
            bwr.read_size = sizeof(read_buffer);
            bwr.read_buffer = (binder_uintptr_t)read_buffer;
            res = ioctl(bs->fd, BINDER_WRITE_READ, &bwr);
            if (res < 0) {
                LOGE("binder_transact2: ioctl read failed (%s)", strerror(errno));
                return -1;
            }
            p = read_buffer;
            end = p + bwr.read_consumed;
        }
    }

    return 0;
}

int binder_call2(struct binder_state *bs,
                 uint32_t target_handle,
                 uint32_t code,
                 void *data, size_t data_size,
                 const binder_size_t *offsets, size_t offsets_size)
{
    struct binder_transaction_data tr;
    memset(&tr, 0, sizeof(tr));

    tr.target.handle = target_handle;
    tr.code = code;
    tr.flags = TF_ONE_WAY;

    tr.data_size = data_size;
    tr.data.ptr.buffer = (binder_uintptr_t)data;
    tr.data.ptr.offsets = (binder_uintptr_t)offsets;
    tr.offsets_size = offsets_size;

    struct {
        uint32_t cmd;
        struct binder_transaction_data txn;
    } __attribute__((packed)) write_data;

    write_data.cmd = BC_TRANSACTION;
    write_data.txn = tr;

    return binder_write(bs, &write_data, sizeof(write_data));
}

int binder_send_reply2(struct binder_state *bs,
                       const void *data, size_t data_size,
                       const binder_size_t *offsets, size_t offsets_size)
{
    struct binder_transaction_data tr;
    memset(&tr, 0, sizeof(tr));

    tr.target.ptr = 0;
    tr.code = 0;
    tr.flags = 0;
    tr.data_size = data_size;
    tr.data.ptr.buffer = (binder_uintptr_t)data;
    tr.data.ptr.offsets = (binder_uintptr_t)offsets;
    tr.offsets_size = offsets_size;

    struct {
        uint32_t cmd;
        struct binder_transaction_data txn;
    } __attribute__((packed)) wd;

    wd.cmd = BC_REPLY;
    wd.txn = tr;

    return binder_write(bs, &wd, sizeof(wd));
}

int binder_acquire_handle(struct binder_state *bs, uint32_t handle)
{
    if (!bs || handle == 0) {
        return -1;
    }

    struct {
        uint32_t cmd_increfs;
        uint32_t handle_increfs;
        uint32_t cmd_acquire;
        uint32_t handle_acquire;
    } __attribute__((packed)) wd;

    wd.cmd_increfs = BC_INCREFS;
    wd.handle_increfs = handle;
    wd.cmd_acquire = BC_ACQUIRE;
    wd.handle_acquire = handle;

    return binder_write(bs, &wd, sizeof(wd));
}

int binder_release_handle(struct binder_state *bs, uint32_t handle)
{
    if (!bs || handle == 0) {
        return -1;
    }

    struct {
        uint32_t cmd_release;
        uint32_t handle_release;
        uint32_t cmd_decrefs;
        uint32_t handle_decrefs;
    } __attribute__((packed)) wd;

    wd.cmd_release = BC_RELEASE;
    wd.handle_release = handle;
    wd.cmd_decrefs = BC_DECREFS;
    wd.handle_decrefs = handle;

    return binder_write(bs, &wd, sizeof(wd));
}
