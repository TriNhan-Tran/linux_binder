#pragma once

#include "binder/callback/native/BnLedSrvCallback.h"
#include "binder/srv/led/ILedSrv.h"

#include <memory>

class App;

/**
 * @brief Singleton adapter bridging the LED service to the App message pipeline.
 *
 * Registers a callback with the LED service and forwards events as App
 * messages via the Handler queue.
 */
class LedSrvAdapter : public BnLedSrvCallback {
public:
    LedSrvAdapter() = default;
    ~LedSrvAdapter() override = default;
    LedSrvAdapter(const LedSrvAdapter&) = delete;
    LedSrvAdapter& operator=(const LedSrvAdapter&) = delete;
    LedSrvAdapter(LedSrvAdapter&&) = delete;
    LedSrvAdapter& operator=(LedSrvAdapter&&) = delete;

    /** @brief Return the process-wide singleton instance. */
    static LedSrvAdapter& getInstance();

    /** @brief Bind this adapter to the owning App instance. */
    void attachApp(App* app);

    /**
     * @brief Create a BpLedSrv proxy bound to the given binder handle.
     * @param handle Remote binder handle for the LED service.
     */
    void initialize(uint32_t handle);

    /** @brief Register this adapter as a callback with the LED service. */
    int registerCallback();

    /** @brief Invoke a LED-on request through the service proxy. */
    int requestLedOn();

    /** @brief Release the service proxy and clear the app binding. */
    void shutdown();

    void onLedEvent(int32_t eventType, const std::string& message) override;

private:
    App* m_app = nullptr;
    std::unique_ptr<ILedSrv> m_ledSrv;
};
