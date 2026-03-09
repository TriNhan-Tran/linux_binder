#pragma once

#include "BinderClient.h"

#include <cstdint>
#include <string>

namespace ipc {

/**
 * @brief Public client contract for talking to the service registry.
 */
class ISrvManager {
public:
    virtual ~ISrvManager() = default;

    virtual int addSrv(const std::string& name, BBinder* server) = 0;
    virtual uint32_t getSrv(const std::string& name) = 0;
};

} // namespace ipc
