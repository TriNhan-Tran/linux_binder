#pragma once

#include "base/MessageQueue.h"

#include <atomic>
#include <thread>

/**
 * @brief Reusable base for message-driven applications.
 *
 * Exposes non-virtual lifecycle API and delegates app-specific setup/teardown
 * via protected hooks.
 */
class Handler {
public:
    Handler() = default;
    virtual ~Handler();

    Handler(const Handler&) = delete;
    Handler& operator=(const Handler&) = delete;
    Handler(Handler&&) = delete;
    Handler& operator=(Handler&&) = delete;

    /** @brief Initialise, launch the worker thread, and call onStart(). */
    void start();
    /** @brief Call onRun() on the calling thread (blocking). */
    void run();
    /** @brief Drain the queue, join the worker thread, and call onStop(). */
    void stop();

    /** @brief Return true while the worker thread is active. */
    bool isRunning() const;
    /** @brief Enqueue a message for handleMessage() on the worker thread. */
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
