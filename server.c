#include <stdio.h>
#include <string.h>
#include "binder.h"

void my_transaction_handler(struct binder_state *bs, struct binder_transaction_data *txn)
{
    // LOGI("Received transaction code: %d", txn->code);

    if (txn->data_size > 0) {
        char buffer[256] = {0};
        size_t len = txn->data_size < 255 ? txn->data_size : 255;

        memcpy(buffer, (void*)txn->data.ptr.buffer, len);

        LOGI("Message from Client: '%s'", buffer);
    }
}

int main(int argc, char **argv)
{
    struct binder_state *bs;

    bs = binder_open("/dev/binderfs/binder");
    if (!bs) return -1;

    if (binder_become_context_manager(bs)) {
        LOGE("Failed to become context manager");
        return -1;
    }

    LOGI("Service started. Waiting for transactions...");

    binder_loop(bs, my_transaction_handler);

    binder_close(bs);
    return 0;
}