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

int binder_write(struct binder_state *bs, void *data, size_t len)
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

static int binder_parse(struct binder_state *bs, struct binder_io *bio,
                        uintptr_t ptr, size_t size, binder_handler_func func)
{
    uint8_t *p = (uint8_t *)ptr;
    uint8_t *end = p + size;

    while (p < end) {
        uint32_t cmd = *(uint32_t *)p;
        p += sizeof(uint32_t);

        switch (cmd) {
        case BR_NOOP:
            break;
        case BR_TRANSACTION_COMPLETE:
            // LOGI("BR_TRANSACTION_COMPLETE");
            break;
        case BR_TRANSACTION:
        case BR_REPLY: {
            struct binder_transaction_data *txn = (struct binder_transaction_data *)p;
            if (func) {
                func(bs, txn);
            }

            if (txn->data.ptr.buffer) {
                 binder_free_buffer(bs, txn->data.ptr.buffer);
            }
            p += sizeof(*txn);
            break;
        }
        case BR_DEAD_BINDER:
            LOGE("BR_DEAD_BINDER");
            break;
        case BR_FAILED_REPLY:
            LOGE("BR_FAILED_REPLY");
            break;
        case BR_DEAD_REPLY:
            LOGE("BR_DEAD_REPLY");
            break;
        default:
            // LOGI("Parse: Unknown cmd %d", cmd);
            return -1;
        }
    }
    return 0;
}

void binder_loop(struct binder_state *bs, binder_handler_func func)
{
    int res;
    struct binder_write_read bwr;
    uint8_t readbuf[32*1024]; // 32KB buffer

    uint32_t cmd = BC_ENTER_LOOPER;
    binder_write(bs, &cmd, sizeof(cmd));

    LOGI("Binder Loop: Entered");

    for (;;) {
        bwr.read_size = sizeof(readbuf);
        bwr.read_consumed = 0;
        bwr.read_buffer = (binder_uintptr_t)readbuf;

        bwr.write_size = 0;
        bwr.write_consumed = 0;
        bwr.write_buffer = 0;

        res = ioctl(bs->fd, BINDER_WRITE_READ, &bwr);

        if (res < 0) {
            LOGE("binder_loop: ioctl failed (%s)", strerror(errno));
            break;
        }

        res = binder_parse(bs, 0, (uintptr_t)readbuf, bwr.read_consumed, func);
        if (res != 0) {
            LOGE("binder_loop: parse error");
            break;
        }
    }
}

int binder_call(struct binder_state *bs,
                uint32_t target_handle,
                uint32_t code,
                void *data,
                size_t data_size)
{
    struct binder_transaction_data tr;
    memset(&tr, 0, sizeof(tr));

    tr.target.handle = target_handle;
    tr.code = code;
    tr.flags = TF_ONE_WAY; // Demo mode simple

    tr.data_size = data_size;
    tr.data.ptr.buffer = (uintptr_t)data;
    tr.data.ptr.offsets = 0;

    struct {
        uint32_t cmd;
        struct binder_transaction_data txn;
    } __attribute__((packed)) write_data;

    write_data.cmd = BC_TRANSACTION;
    write_data.txn = tr;

    return binder_write(bs, &write_data, sizeof(write_data));
}