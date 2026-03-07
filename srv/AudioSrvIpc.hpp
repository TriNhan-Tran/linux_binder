#pragma once

#include "BinderCore.hpp"

#include <cstdint>

namespace ipc {

/**
 * @brief Client proxy for audio server binder interface.
 */
class BpAudioSrv {
public:
    /** @brief Construct from remote audio server handle. */
    explicit BpAudioSrv(uint32_t handle);

    /** @brief Start audio playback. */
    int playAudio();

    /** @brief Stop audio playback. */
    int stopAudio();

    /**
     * @brief Register callback binder for async audio events.
     *
     * @param callback Local callback binder object.
     * @return 0 on success, non-zero on failure.
     */
    int registerCallback(BBinder* callback);

private:
    BpBinder m_remote;
};

/**
 * @brief Server-side audio server binder interface.
 */
class BnAudioSrv : public BBinder {
public:
    /** @brief Execute audio play request. */
    virtual int onPlayAudio() = 0;

    /** @brief Execute audio stop request. */
    virtual int onStopAudio() = 0;

    /**
     * @brief Register remote callback handle for notifications.
     *
     * @param callbackHandle Kernel-translated callback handle.
     * @return 0 on success, non-zero on failure.
     */
    virtual int onRegisterCallback(uint32_t callbackHandle) = 0;

    /** @brief Dispatch binder transaction for audio server codes. */
    int onTransact(uint32_t code, const Parcel& data, Parcel* reply) override;
};

} // namespace ipc
