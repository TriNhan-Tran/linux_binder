#pragma once

#include <cstdint>
#include <string>

namespace ipc {

/**
 * @brief Public callback contract used by services to notify app clients.
 */
class ISrvCallback {
public:
    virtual ~ISrvCallback() = default;
    virtual void onEvent(int32_t eventType, const std::string& message) = 0;
};

} // namespace ipc
