#pragma once

#include "ILedSrvCallback.h"

namespace ipc {

/**
 * @brief Public client contract for the LED service.
 */
class ILedSrv {
public:
    virtual ~ILedSrv() = default;

    virtual int requestLedOn() = 0;
    virtual int registerCallback(ILedSrvCallback* callback) = 0;
};

} // namespace ipc
