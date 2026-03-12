#include "binder/manager/SrvMgr.h"

#include "binder/core/BinderClient.h"


SrvMgr::~SrvMgr() {
    LOG_INFO("SrvMgr::~SrvMgr releasing %zu registered handles", m_registry.size());
    for (const auto& [name, handle] : m_registry) {
        if (handle != 0 && IPCThreadState::releaseHandle(handle) != 0) {
            LOG_ERROR("SrvMgr: failed to release handle %u for '%s'", handle, name.c_str());
        }
    }
}

int SrvMgr::onAddSrv(const std::string& name, uint32_t handle) {
    if (handle == 0) {
        LOG_ERROR("SrvMgr: addSrv '%s' with invalid handle 0", name.c_str());
        return -1;
    }

    const auto it = m_registry.find(name);
    if (it != m_registry.end()) {
        const uint32_t oldHandle = it->second;
        if (oldHandle != 0 && IPCThreadState::releaseHandle(oldHandle) != 0) {
            LOG_ERROR("SrvMgr: failed to release old handle %u for '%s'", oldHandle, name.c_str());
            return -1;
        }
        it->second = handle;
        LOG_INFO("SrvMgr: replaced '%s' handle %u -> %u", name.c_str(), oldHandle, handle);
        return 0;
    }

    m_registry[name] = handle;
    LOG_INFO("SrvMgr: registered '%s' -> handle %u", name.c_str(), handle);
    return 0;
}

uint32_t SrvMgr::onGetSrv(const std::string& name) {
    const auto it = m_registry.find(name);
    if (it != m_registry.end()) {
        LOG_INFO("SrvMgr: getSrv '%s' -> handle %u", name.c_str(), it->second);
        return it->second;
    }
    LOG_WARN("SrvMgr: getSrv '%s' NOT FOUND", name.c_str());
    return 0;
}

