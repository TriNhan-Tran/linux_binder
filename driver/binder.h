#ifndef BINDER_DRIVER_H
#define BINDER_DRIVER_H

#include <stddef.h>
#include <stdio.h>
#include <stdint.h>
#include <stdarg.h>
#include <unistd.h>
#include <sys/syscall.h>

#include <linux/android/binder.h>

#ifdef __cplusplus
extern "C" {
#endif

static inline long binder_log_tid(void)
{
#ifdef SYS_gettid
    return (long)syscall(SYS_gettid);
#else
    return (long)getpid();
#endif
}

static inline void binder_log_emit(const char* level, const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    fprintf(stdout, "[%s][tid=%ld] ", level, binder_log_tid());
    vfprintf(stdout, fmt, args);
    fprintf(stdout, "\n");
    fflush(stdout);
    va_end(args);
}

#define LOG_DEBUG(fmt, ...) binder_log_emit("DEBUG", fmt, ##__VA_ARGS__)
#define LOG_INFO(fmt, ...) binder_log_emit("INFO", fmt, ##__VA_ARGS__)
#define LOG_WARN(fmt, ...) binder_log_emit("WARN", fmt, ##__VA_ARGS__)
#define LOG_ERROR(fmt, ...) binder_log_emit("ERROR", fmt, ##__VA_ARGS__)

/* Backward compatibility aliases for existing call sites. */
#define LOGI(fmt, ...) LOG_INFO(fmt, ##__VA_ARGS__)
#define LOGE(fmt, ...) LOG_ERROR(fmt, ##__VA_ARGS__)

struct binder_state {
    int fd;
    void *mapped;
    size_t mapsize;
};

/**
 * @brief Open binder driver and map process binder buffer.
 *
 * @param driver Binder device path (for example `/dev/binderfs/binder`).
 * @return Pointer to binder state on success, NULL on failure.
 */
struct binder_state *binder_open(const char* driver);

/**
 * @brief Close binder driver and release process state.
 *
 * @param bs Binder process state.
 */
void binder_close(struct binder_state *bs);

/**
 * @brief Become context manager for the binder domain.
 *
 * @param bs Binder process state.
 * @return 0 on success, -1 on failure.
 */
int binder_become_context_manager(struct binder_state *bs);

/**
 * @brief Return transaction buffer to kernel after processing.
 *
 * @param bs Binder process state.
 * @param buffer_ptr Kernel-provided transaction buffer pointer.
 */
void binder_free_buffer(struct binder_state *bs, binder_uintptr_t buffer_ptr);

typedef void (*binder_handler_ctx_func)(struct binder_state *bs,
                                        struct binder_transaction_data *txn,
                                        void *context);

/**
 * @brief Enter blocking binder loop and dispatch transactions to callback.
 *
 * @param bs Binder process state.
 * @param func Transaction callback.
 * @param context User context passed to callback.
 */
void binder_loop_ctx(struct binder_state *bs, binder_handler_ctx_func func, void *context);

/*
 * Extended API with flat_binder_object offset support.
 *
 * offsets      = array of binder_size_t byte-offsets into data where
 *                flat_binder_object structs reside.
 * offsets_size = total byte size of the offsets array.
 * The kernel uses these offsets to translate binder/handle objects.
 */

/**
 * @brief Perform synchronous binder transaction and read reply.
 */
int binder_transact(struct binder_state *bs,
                     uint32_t target_handle,
                     uint32_t code,
                     const void *data, size_t data_size,
                     const binder_size_t *offsets, size_t offsets_size,
                     void *reply_buf, size_t reply_buf_size,
                     size_t *reply_size);

/**
 * @brief Send one-way binder transaction.
 */
int binder_call(struct binder_state *bs,
                 uint32_t target_handle,
                 uint32_t code,
                 void *data, size_t data_size,
                 const binder_size_t *offsets, size_t offsets_size);

/**
 * @brief Send binder reply payload.
 */
int binder_send_reply(struct binder_state *bs,
                       const void *data, size_t data_size,
                       const binder_size_t *offsets, size_t offsets_size);

/**
 * @brief Acquire reference ownership for remote handle.
 */
int binder_acquire_handle(struct binder_state *bs, uint32_t handle);

/**
 * @brief Release previously acquired handle ownership.
 */
int binder_release_handle(struct binder_state *bs, uint32_t handle);

#ifdef __cplusplus
}
#endif

#endif // BINDER_DRIVER_H
