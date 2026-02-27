#ifndef __BINDER_H_
#define __BINDER_H_

#include <stddef.h>
#include <stdint.h>
#include <linux/android/binder.h>

#define LOGI(fmt, ...) printf("[INFO] " fmt "\n", ##__VA_ARGS__)
#define LOGE(fmt, ...) printf("[ERROR] " fmt "\n", ##__VA_ARGS__)

struct binder_state {
    int fd;
    void *mapped;
    size_t mapsize;
};

typedef void (*binder_handler_func)(struct binder_state *bs,
                                    struct binder_transaction_data *txn);


struct binder_state *binder_open(const char* driver);

void binder_close(struct binder_state *bs);

int binder_become_context_manager(struct binder_state *bs);

void binder_loop(struct binder_state *bs, binder_handler_func func);

int binder_call(struct binder_state *bs,
                uint32_t target_handle,
                uint32_t code,
                void *data,
                size_t data_size);

int binder_transact(struct binder_state *bs,
                    uint32_t target_handle,
                    uint32_t code,
                    void *data,
                    size_t data_size,
                    void *reply_data,
                    size_t reply_capacity,
                    size_t *reply_size);

int binder_send_reply(struct binder_state *bs,
                      uint32_t code,
                      void *data,
                      size_t data_size);

void binder_free_buffer(struct binder_state *bs, binder_uintptr_t buffer_ptr);

#endif // __BINDER_H_