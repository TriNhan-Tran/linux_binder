#pragma once

#include <cstdint>
#include <string>

namespace ipc {

/**
 * @brief LED service callback contract.
 */
class ILedSrvCallback {
public:
    virtual ~ILedSrvCallback() = default;
    virtual void onLedEvent(int32_t eventType, const std::string& message) = 0;
};

} // namespace ipc
