#include "LedSrvAdapter.h"

#include "App.h"

#include "BpLedSrv.h"

namespace demo {

LedSrvAdapter& LedSrvAdapter::getInstance() {
    static LedSrvAdapter instance;
    LOG_DEBUG("LedSrvAdapter::getInstance singleton=%p", static_cast<void*>(&instance));
    return instance;
}

void LedSrvAdapter::attachApp(App* app) {
    LOG_INFO("LedSrvAdapter::attachApp app=%p", static_cast<void*>(app));
    m_app = app;
}

void LedSrvAdapter::initialize(uint32_t handle) {
    if (handle == 0) {
        LOG_ERROR("LedSrvAdapter::initialize failed: handle=0");
        m_ledSrv.reset();
        return;
    }

    if (m_ledSrv) {
        LOG_WARN("LedSrvAdapter::initialize replacing existing LED proxy");
    }

    LOG_INFO("LedSrvAdapter::initialize creating BpLedSrv proxy for handle=%u", handle);
    m_ledSrv = std::make_unique<ipc::BpLedSrv>(handle);
    LOG_INFO("LedSrvAdapter::initialize completed");
}

int LedSrvAdapter::registerCallback() {
    if (!m_ledSrv) {
        LOG_ERROR("LedSrvAdapter::registerCallback failed: proxy not initialized");
        return -1;
    }

    LOG_INFO("LedSrvAdapter::registerCallback sending IPC request");
    const int status = m_ledSrv->registerCallback(this);
    if (status != 0) {
        LOG_ERROR("LedSrvAdapter::registerCallback failed status=%d", status);
    } else {
        LOG_INFO("LedSrvAdapter::registerCallback succeeded");
    }
    return status;
}

int LedSrvAdapter::requestLedOn() {
    if (!m_ledSrv) {
        LOG_ERROR("LedSrvAdapter::requestLedOn failed: proxy not initialized");
        return -1;
    }

    LOG_INFO("LedSrvAdapter::requestLedOn sending IPC request");
    const int status = m_ledSrv->requestLedOn();
    if (status != 0) {
        LOG_ERROR("LedSrvAdapter::requestLedOn failed status=%d", status);
    } else {
        LOG_INFO("LedSrvAdapter::requestLedOn completed status=%d", status);
    }
    return status;
}

void LedSrvAdapter::shutdown() {
    LOG_INFO("LedSrvAdapter::shutdown releasing proxy and app binding");
    m_ledSrv.reset();
    m_app = nullptr;
}

void LedSrvAdapter::onLedEvent(int32_t eventType, const std::string& message) {
    LOG_INFO("LedSrvAdapter::onLedEvent received eventType=%d message='%s'",
             eventType,
             message.c_str());
    if (m_app != nullptr) {
        Message appMessage;
        appMessage.what = App::MSG_LED_EVENT;
        appMessage.obj = App::LedEvent{eventType, message};
        m_app->sendMessage(appMessage);
    } else {
        LOG_WARN("LedSrvAdapter::onLedEvent dropped: app is null");
    }
}

} // namespace demo
