#include "SrvManagerHost.hpp"

#include "BinderCore.hpp"

namespace demo {

SrvManagerHost::~SrvManagerHost() {
    for (const auto& [name, handle] : m_registry) {
        if (handle != 0 && ipc::IPCThreadState::releaseHandle(handle) != 0) {
            LOGE("SrvManager: failed to release handle %u for '%s'", handle, name.c_str());
        }
    }
}

int SrvManagerHost::onAddSrv(const std::string& name, uint32_t handle) {
    if (handle == 0) {
        LOGE("SrvManager: addSrv '%s' with invalid handle 0", name.c_str());
        return -1;
    }

    const auto it = m_registry.find(name);
    if (it != m_registry.end()) {
        const uint32_t oldHandle = it->second;
        if (oldHandle != 0 && ipc::IPCThreadState::releaseHandle(oldHandle) != 0) {
            LOGE("SrvManager: failed to release old handle %u for '%s'", oldHandle, name.c_str());
            return -1;
        }
        it->second = handle;
        LOGI("SrvManager: replaced '%s' handle %u -> %u", name.c_str(), oldHandle, handle);
        return 0;
    }

    m_registry[name] = handle;
    LOGI("SrvManager: registered '%s' -> handle %u", name.c_str(), handle);
    return 0;
}

uint32_t SrvManagerHost::onGetSrv(const std::string& name) {
    const auto it = m_registry.find(name);
    if (it != m_registry.end()) {
        LOGI("SrvManager: getSrv '%s' -> handle %u", name.c_str(), it->second);
        return it->second;
    }
    LOGE("SrvManager: getSrv '%s' NOT FOUND", name.c_str());
    return 0;
}

} // namespace demo
