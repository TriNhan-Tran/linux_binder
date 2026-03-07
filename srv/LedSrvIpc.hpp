#pragma once

#include "BinderCore.hpp"

#include <cstdint>

namespace ipc {

/**
 * @brief Client proxy for LED server binder interface.
 */
class BpLedSrv {
public:
    /** @brief Construct from remote LED server handle. */
    explicit BpLedSrv(uint32_t handle);

    /** @brief Request LED ON action. */
    int requestLedOn();

    /**
     * @brief Register callback binder for async LED events.
     *
     * @param callback Local callback binder object.
     * @return 0 on success, non-zero on failure.
     */
    int registerCallback(BBinder* callback);

private:
    BpBinder m_remote;
};

/**
 * @brief Server-side LED server binder interface.
 */
class BnLedSrv : public BBinder {
public:
    /** @brief Execute LED ON request. */
    virtual int onRequestLedOn() = 0;

    /**
     * @brief Register remote callback handle for notifications.
     *
     * @param callbackHandle Kernel-translated callback handle.
     * @return 0 on success, non-zero on failure.
     */
    virtual int onRegisterCallback(uint32_t callbackHandle) = 0;

    /** @brief Dispatch binder transaction for LED server codes. */
    int onTransact(uint32_t code, const Parcel& data, Parcel* reply) override;
};

} // namespace ipc
