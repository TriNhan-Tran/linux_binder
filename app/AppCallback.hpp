#pragma once

#include "SrvCallbackIpc.hpp"

#include <cstdint>
#include <string>

namespace demo {

/**
 * @brief Application callback binder implementation.
 */
class AppCallback : public ipc::BnSrvCallback {
public:
    void onEvent(int32_t eventType, const std::string& message) override;
};

} // namespace demo
