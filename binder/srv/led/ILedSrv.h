#pragma once

#include "binder/callback/interfaces/ILedSrvCallback.h"


/**
 * @brief Public client contract for the LED service.
 */
class ILedSrv {
public:
    virtual ~ILedSrv() = default;

    virtual int requestLedOn() = 0;
    virtual int registerCallback(ILedSrvCallback* callback) = 0;
};

