#include "LedSrvAdapter.h"

#include "App.h"

#include "BpLedSrv.h"

namespace demo {

LedSrvAdapter& LedSrvAdapter::getInstance() {
    static LedSrvAdapter instance;
    return instance;
}

void LedSrvAdapter::attachApp(App* app) {
    m_app = app;
}

void LedSrvAdapter::initialize(uint32_t handle) {
    if (handle == 0) {
        m_ledSrv.reset();
        return;
    }

    m_ledSrv = std::make_unique<ipc::BpLedSrv>(handle);
}

int LedSrvAdapter::registerCallback() {
    if (!m_ledSrv) {
        return -1;
    }

    return m_ledSrv->registerCallback(this);
}

int LedSrvAdapter::requestLedOn() {
    if (!m_ledSrv) {
        return -1;
    }

    return m_ledSrv->requestLedOn();
}

void LedSrvAdapter::shutdown() {
    m_ledSrv.reset();
    m_app = nullptr;
}

void LedSrvAdapter::onLedEvent(int32_t eventType, const std::string& message) {
    if (m_app != nullptr) {
        m_app->handleLedEvent(eventType, message);
    }
}

} // namespace demo
