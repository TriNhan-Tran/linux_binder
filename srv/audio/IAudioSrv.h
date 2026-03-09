#pragma once

#include "IAudioSrvCallback.h"

namespace ipc {

/**
 * @brief Public client contract for the audio service.
 */
class IAudioSrv {
public:
    virtual ~IAudioSrv() = default;

    virtual int playAudio() = 0;
    virtual int stopAudio() = 0;
    virtual int registerCallback(IAudioSrvCallback* callback) = 0;
};

} // namespace ipc
