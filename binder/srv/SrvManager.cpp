#include "SrvManager.h"

#include "BinderClient.h"

namespace demo {

SrvManager::~SrvManager() {
    LOG_INFO("SrvManager::~SrvManager releasing %zu registered handles", m_registry.size());
    for (const auto& [name, handle] : m_registry) {
        if (handle != 0 && ipc::IPCThreadState::releaseHandle(handle) != 0) {
            LOG_ERROR("SrvManager: failed to release handle %u for '%s'", handle, name.c_str());
        }
    }
}

int SrvManager::onAddSrv(const std::string& name, uint32_t handle) {
    if (handle == 0) {
        LOG_ERROR("SrvManager: addSrv '%s' with invalid handle 0", name.c_str());
        return -1;
    }

    const auto it = m_registry.find(name);
    if (it != m_registry.end()) {
        const uint32_t oldHandle = it->second;
        if (oldHandle != 0 && ipc::IPCThreadState::releaseHandle(oldHandle) != 0) {
            LOG_ERROR("SrvManager: failed to release old handle %u for '%s'", oldHandle, name.c_str());
            return -1;
        }
        it->second = handle;
        LOG_INFO("SrvManager: replaced '%s' handle %u -> %u", name.c_str(), oldHandle, handle);
        return 0;
    }

    m_registry[name] = handle;
    LOG_INFO("SrvManager: registered '%s' -> handle %u", name.c_str(), handle);
    return 0;
}

uint32_t SrvManager::onGetSrv(const std::string& name) {
    const auto it = m_registry.find(name);
    if (it != m_registry.end()) {
        LOG_INFO("SrvManager: getSrv '%s' -> handle %u", name.c_str(), it->second);
        return it->second;
    }
    LOG_WARN("SrvManager: getSrv '%s' NOT FOUND", name.c_str());
    return 0;
}

} // namespace demo
