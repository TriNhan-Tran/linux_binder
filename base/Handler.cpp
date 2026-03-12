#include "base/Handler.h"

#include "driver/binder.h"

#include <exception>

Handler::~Handler() {
    stop();
}

void Handler::onInit() {
}

void Handler::onStart() {
}

void Handler::onRun() {
}

void Handler::onStop() {
}

void Handler::start() {
    if (m_running.load()) {
        LOG_WARN("[Handler] start skipped: already running");
        return;
    }

    if (m_worker.joinable()) {
        if (m_worker.get_id() == std::this_thread::get_id()) {
            LOG_ERROR("[Handler] start failed: called from worker thread");
            return;
        }
        m_worker.join();
    }

    LOG_INFO("Handler starting");

    try {
        onInit();
    } catch (const std::exception& ex) {
        LOG_ERROR("[Handler] onInit failed: %s", ex.what());
        try {
            onStop();
        } catch (const std::exception& stopEx) {
            LOG_WARN("[Handler] onStop raised during init failure: %s", stopEx.what());
        } catch (...) {
            LOG_WARN("[Handler] onStop raised during init failure: unknown");
        }
        return;
    } catch (...) {
        LOG_ERROR("[Handler] onInit failed: unknown exception");
        try {
            onStop();
        } catch (const std::exception& stopEx) {
            LOG_WARN("[Handler] onStop raised during init failure: %s", stopEx.what());
        } catch (...) {
            LOG_WARN("[Handler] onStop raised during init failure: unknown");
        }
        return;
    }

    LOG_INFO("Handler initialized");

    m_queue.reset();

    m_running.store(true);
    try {
        m_worker = std::thread([this]() {
            workerLoop();
        });
    } catch (const std::exception& ex) {
        m_running.store(false);
        LOG_ERROR("[Handler] start failed: %s", ex.what());
        m_queue.stop();
        try {
            onStop();
        } catch (const std::exception& stopEx) {
            LOG_WARN("[Handler] onStop raised during start failure: %s", stopEx.what());
        } catch (...) {
            LOG_WARN("[Handler] onStop raised during start failure: unknown");
        }
        return;
    } catch (...) {
        m_running.store(false);
        LOG_ERROR("[Handler] start failed: unknown exception");
        m_queue.stop();
        try {
            onStop();
        } catch (const std::exception& stopEx) {
            LOG_WARN("[Handler] onStop raised during start failure: %s", stopEx.what());
        } catch (...) {
            LOG_WARN("[Handler] onStop raised during start failure: unknown");
        }
        return;
    }

    try {
        onStart();
    } catch (const std::exception& ex) {
        LOG_ERROR("[Handler] onStart failed: %s", ex.what());
        stop();
        return;
    } catch (...) {
        LOG_ERROR("[Handler] onStart failed: unknown exception");
        stop();
        return;
    }

    LOG_INFO("[Handler] started");
}

void Handler::run() {
    if (!isRunning()) {
        LOG_WARN("[Handler] run skipped: not running");
        return;
    }

    try {
        onRun();
    } catch (const std::exception& ex) {
        LOG_ERROR("[Handler] onRun failed: %s", ex.what());
    } catch (...) {
        LOG_ERROR("[Handler] onRun failed: unknown exception");
    }
}

void Handler::stop() {
    const bool wasRunning = m_running.exchange(false);
    if (!wasRunning && !m_worker.joinable()) {
        return;
    }

    LOG_INFO("Handler stopping");

    m_queue.stop();

    if (m_worker.joinable()) {
        if (m_worker.get_id() == std::this_thread::get_id()) {
            LOG_WARN("[Handler] stop called from worker thread; join deferred");
        } else {
            m_worker.join();
        }
    }

    try {
        onStop();
    } catch (const std::exception& ex) {
        LOG_WARN("[Handler] onStop raised exception: %s", ex.what());
    } catch (...) {
        LOG_WARN("[Handler] onStop raised exception: unknown");
    }

    if (wasRunning) {
        LOG_INFO("[Handler] stopped");
    }
}

bool Handler::isRunning() const {
    return m_running.load();
}

void Handler::sendMessage(const Message& message) {
    LOG_INFO("[Handler] message posted what=%d", message.what);

    if (!m_queue.enqueue(message)) {
        LOG_WARN("[Handler] failed to enqueue message what=%d", message.what);
    }
}

void Handler::workerLoop() {
    while (m_running.load()) {
        Message message;
        if (!m_queue.dequeue(message)) {
            continue;
        }

        if (!m_running.load()) {
            break;
        }

        LOG_INFO("[Handler] message dequeued what=%d", message.what);

        try {
            handleMessage(message);
        } catch (const std::exception& ex) {
            LOG_ERROR("[Handler] handler exception: %s", ex.what());
        } catch (...) {
            LOG_ERROR("[Handler] handler exception: unknown");
        }
    }
}
