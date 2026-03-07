#pragma once

#include "BinderCore.hpp"

#include <cstdint>
#include <string>

namespace ipc {

/**
 * @brief Server-side callback interface implemented by app process.
 */
class BnSrvCallback : public BBinder {
public:
    /**
     * @brief Receive asynchronous server event.
     *
     * @param eventType Event code from `NotifyEvent`.
     * @param message Event payload message.
     */
    virtual void onEvent(int32_t eventType, const std::string& message) = 0;

    /** @brief Dispatch callback transaction from remote server. */
    int onTransact(uint32_t code, const Parcel& data, Parcel* reply) override;
};

/**
 * @brief Client-side callback proxy used by servers.
 */
class BpSrvCallback {
public:
    /** @brief Construct from callback binder handle. */
    explicit BpSrvCallback(uint32_t handle);

    /**
     * @brief Send one-way callback event to app process.
     *
     * @param eventType Event code from `NotifyEvent`.
     * @param message Event payload message.
     */
    void onEvent(int32_t eventType, const std::string& message);

private:
    BpBinder m_remote;
};

} // namespace ipc
