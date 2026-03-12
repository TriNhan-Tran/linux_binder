#pragma once

#include <cstdint>

extern "C" {
#include "driver/binder.h"
}


class Parcel;

/**
 * @brief Process-wide binder driver owner.
 *
 * This singleton owns exactly one binder driver descriptor per process.
 */
class ProcessState {
public:
    /** @brief Access singleton process binder state. */
    static ProcessState& self();

    /** @brief Return the native binder state pointer. */
    struct binder_state* bs() const;

    /** @brief Return true when binder driver is opened. */
    bool isOpen() const;

    /**
     * @brief Open binder driver if it is not opened yet.
     *
     * @param driver Driver path, default `/dev/binderfs/binder`.
     */
    void open(const char* driver = "/dev/binderfs/binder");

    /** @brief Close binder driver if opened. */
    void close();

private:
    ProcessState() = default;
    ~ProcessState();
    ProcessState(const ProcessState&) = delete;
    ProcessState& operator=(const ProcessState&) = delete;
    ProcessState(ProcessState&&) = delete;
    ProcessState& operator=(ProcessState&&) = delete;

    struct binder_state* m_bs = nullptr;
};

/**
 * @brief Base class for local binder server implementations.
 */
class BBinder {
public:
    /** @brief Virtual destructor for polymorphic base. */
    BBinder() = default;
    virtual ~BBinder() = default;

    BBinder(const BBinder&) = delete;
    BBinder& operator=(const BBinder&) = delete;
    BBinder(BBinder&&) = delete;
    BBinder& operator=(BBinder&&) = delete;

    /**
     * @brief Handle a binder transaction directed at this local object.
     *
     * @param code Transaction identifier.
     * @param data Request payload.
     * @param reply Reply payload destination for synchronous calls.
     * @return 0 on success, non-zero on failure.
     */
    virtual int onTransact(uint32_t code, const Parcel& data, Parcel* reply) = 0;
};

/**
 * @brief Generic remote binder handle wrapper.
 */
class BpBinder {
public:
    /** @brief Construct from remote binder handle. */
    explicit BpBinder(uint32_t handle);

    /** @brief Return remote binder handle. */
    uint32_t handle() const;

    /**
     * @brief Perform synchronous transaction.
     *
     * @param code Transaction identifier.
     * @param data Request payload.
     * @param reply Optional reply destination.
     * @return 0 on success, non-zero on failure.
     */
    int transact(uint32_t code, const Parcel& data, Parcel* reply);

    /**
     * @brief Send one-way transaction (no reply expected).
     *
     * @param code Transaction identifier.
     * @param data Request payload.
     * @return 0 on success, non-zero on failure.
     */
    int send(uint32_t code, const Parcel& data);

private:
    uint32_t m_handle;
};

/**
 * @brief Binder thread and transaction helper utilities.
 */
class IPCThreadState {
public:
    /** @brief Become binder context manager (handle 0 owner). */
    static int becomeContextManager();

    /** @brief Increase process reference ownership for a remote handle. */
    static int acquireHandle(uint32_t handle);

    /** @brief Release process reference ownership for a remote handle. */
    static int releaseHandle(uint32_t handle);

    /**
     * @brief Send reply parcel for current synchronous transaction.
     *
     * @param reply Reply parcel payload.
     * @return 0 on success, non-zero on failure.
     */
    static int sendReply(const Parcel& reply);

    /**
     * @brief Enter blocking binder thread loop.
     *
     * @param defaultServer Server used when transaction cookie is 0.
     */
    static void joinThreadPool(BBinder* defaultServer = nullptr);
};

