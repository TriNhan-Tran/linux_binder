#pragma once

#include <any>
#include <condition_variable>
#include <cstdint>
#include <deque>
#include <mutex>
#include <utility>

namespace demo {

/**
 * @brief Generic message type for the Handler message pipeline.
 */
struct Message {
    int32_t what = 0;
    std::any obj = nullptr;
};

template <typename T>
class MessageQueue {
public:
    MessageQueue() = default;

    MessageQueue(const MessageQueue&) = delete;
    MessageQueue& operator=(const MessageQueue&) = delete;

    bool enqueue(const T& message) {
        std::lock_guard<std::mutex> lock(m_mutex);
        if (m_stopped) {
            return false;
        }

        m_messages.push_back(message);
        m_cv.notify_one();
        return true;
    }

    bool dequeue(T& message) {
        std::unique_lock<std::mutex> lock(m_mutex);
        m_cv.wait(lock, [this]() {
            return m_stopped || !m_messages.empty();
        });

        if (m_messages.empty()) {
            return false;
        }

        message = std::move(m_messages.front());
        m_messages.pop_front();
        return true;
    }

    void stop() {
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            m_stopped = true;
        }
        m_cv.notify_all();
    }

    void reset() {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_stopped = false;
        m_messages.clear();
    }

private:
    std::mutex m_mutex;
    std::condition_variable m_cv;
    std::deque<T> m_messages;
    bool m_stopped = false;
};

} // namespace demo
