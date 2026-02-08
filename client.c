#include <stdio.h>
#include <string.h>
#include "binder.h"

int main(int argc, char **argv)
{
    struct binder_state *bs;
    const char *msg = "Hello from Refactored Client!";

    bs = binder_open("/dev/binderfs/binder");
    if (!bs) return -1;

    LOGI("Sending message to Server...");

    binder_call(bs,
                0,              // Target Handle (0 = ServiceManager)
                0,              // Transaction Code
                (void*)msg,     // Data Payload
                strlen(msg) + 1 // Data Size
    );

    LOGI("Message sent.");

    binder_close(bs);
    return 0;
}