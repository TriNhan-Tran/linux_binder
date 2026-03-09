#pragma once

#include "BnLedSrvCallback.h"
#include "ILedSrv.h"

#include <memory>

namespace demo {

class App;

/**
 * @brief Adapter that handles only LED service callbacks.
 */
class LedSrvAdapter : public ipc::BnLedSrvCallback {
public:
    static LedSrvAdapter& getInstance();

    LedSrvAdapter(const LedSrvAdapter&) = delete;
    LedSrvAdapter& operator=(const LedSrvAdapter&) = delete;

    void attachApp(App* app);
    void initialize(uint32_t handle);
    int registerCallback();
    int requestLedOn();
    void shutdown();

    void onLedEvent(int32_t eventType, const std::string& message) override;

private:
    LedSrvAdapter() = default;

    App* m_app = nullptr;
    std::unique_ptr<ipc::ILedSrv> m_ledSrv;
};

} // namespace demo
