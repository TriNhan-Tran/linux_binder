#pragma once

#include "MessageQueue.h"

#include <atomic>
#include <thread>

namespace demo {

/**
 * @brief Reusable base for message-driven applications.
 *
 * Exposes non-virtual lifecycle API and delegates app-specific setup/teardown
 * via protected hooks.
 */
class Handler {
public:
    virtual ~Handler();

    void start();
    void run();
    void stop();

    bool isRunning() const;
    void sendMessage(const Message& message);

protected:
    virtual void onInit();
    virtual void onStart();
    virtual void onRun();
    virtual void onStop();

    virtual void handleMessage(const Message& message) = 0;

private:
    void workerLoop();

    MessageQueue<Message> m_queue;
    std::thread m_worker;
    std::atomic<bool> m_running{false};
};

} // namespace demo
