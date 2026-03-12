#include "binder/core/BinderClient.h"

#include "binder/core/TransactionCode.h"
#include "binder/core/Parcel.h"

#include <array>


ProcessState& ProcessState::self() {
    static ProcessState instance;
    return instance;
}

ProcessState::~ProcessState() {
    close();
}

struct binder_state* ProcessState::bs() const {
    return m_bs;
}

bool ProcessState::isOpen() const {
    return m_bs != nullptr;
}

void ProcessState::open(const char* driver) {
    if (m_bs != nullptr) {
        LOG_WARN("ProcessState::open skipped: binder already open");
        return;
    }

    const char* driverPath = driver != nullptr ? driver : BINDER_DRIVER_PATH;
    LOG_INFO("ProcessState::open opening binder driver '%s'", driverPath);

    m_bs = binder_open(driverPath);
    if (m_bs == nullptr) {
        LOG_ERROR("ProcessState::open failed for '%s'", driverPath);
        return;
    }

    LOG_INFO("ProcessState::open succeeded for '%s'", driverPath);
}

void ProcessState::close() {
    if (m_bs == nullptr) {
        LOG_WARN("ProcessState::close skipped: binder already closed");
        return;
    }

    LOG_INFO("ProcessState::close closing binder driver");
    binder_close(m_bs);
    m_bs = nullptr;
    LOG_INFO("ProcessState::close completed");
}

BpBinder::BpBinder(uint32_t handle)
    : m_handle(handle) {
    LOG_DEBUG("BpBinder::BpBinder created for handle=%u", m_handle);
}

uint32_t BpBinder::handle() const {
    return m_handle;
}

int BpBinder::transact(uint32_t code, const Parcel& data, Parcel* reply) {
    auto* binderState = ProcessState::self().bs();
    if (binderState == nullptr) {
        LOG_ERROR("BpBinder::transact code=%u handle=%u failed: no binder state", code, m_handle);
        return -1;
    }

    LOG_DEBUG("BpBinder::transact sending code=%u handle=%u dataSize=%zu",
              code,
              m_handle,
              data.dataSize());

    std::array<uint8_t, 4096> replyBuffer{};
    size_t replySize = 0;
    const int status = binder_transact(
        binderState,
        m_handle,
        code,
        data.data(),
        data.dataSize(),
        data.offsets(),
        data.offsetsSize(),
        replyBuffer.data(),
        replyBuffer.size(),
        &replySize);

    if (status != 0) {
        LOG_ERROR("BpBinder::transact failed code=%u handle=%u status=%d", code, m_handle, status);
        return status;
    }

    if (reply != nullptr && replySize > 0) {
        *reply = Parcel(replyBuffer.data(), replySize);
    }

    LOG_DEBUG("BpBinder::transact completed code=%u handle=%u replySize=%zu",
              code,
              m_handle,
              replySize);

    return 0;
}

int BpBinder::send(uint32_t code, const Parcel& data) {
    auto* binderState = ProcessState::self().bs();
    if (binderState == nullptr) {
        LOG_ERROR("BpBinder::send code=%u handle=%u failed: no binder state", code, m_handle);
        return -1;
    }

    LOG_DEBUG("BpBinder::send sending one-way code=%u handle=%u dataSize=%zu",
              code,
              m_handle,
              data.dataSize());

    // binder_call expects mutable data pointer but does not modify payload.
    const int status = binder_call(
        binderState,
        m_handle,
        code,
        const_cast<uint8_t*>(data.data()),
        data.dataSize(),
        data.offsets(),
        data.offsetsSize());

    if (status != 0) {
        LOG_ERROR("BpBinder::send failed code=%u handle=%u status=%d", code, m_handle, status);
    } else {
        LOG_DEBUG("BpBinder::send completed code=%u handle=%u", code, m_handle);
    }

    return status;
}

int IPCThreadState::becomeContextManager() {
    auto* binderState = ProcessState::self().bs();
    if (binderState == nullptr) {
        LOG_ERROR("IPCThreadState::becomeContextManager failed: no binder state");
        return -1;
    }

    LOG_INFO("IPCThreadState::becomeContextManager requesting context manager role");
    const int status = binder_become_context_manager(binderState);
    if (status != 0) {
        LOG_ERROR("IPCThreadState::becomeContextManager failed status=%d", status);
    } else {
        LOG_INFO("IPCThreadState::becomeContextManager succeeded");
    }
    return status;
}

int IPCThreadState::acquireHandle(uint32_t handle) {
    auto* binderState = ProcessState::self().bs();
    if (binderState == nullptr || handle == 0) {
        LOG_ERROR("IPCThreadState::acquireHandle failed: invalid state handle=%u", handle);
        return -1;
    }

    const int status = binder_acquire_handle(binderState, handle);
    if (status != 0) {
        LOG_ERROR("IPCThreadState::acquireHandle failed for handle=%u status=%d", handle, status);
    } else {
        LOG_DEBUG("IPCThreadState::acquireHandle succeeded for handle=%u", handle);
    }
    return status;
}

int IPCThreadState::releaseHandle(uint32_t handle) {
    auto* binderState = ProcessState::self().bs();
    if (binderState == nullptr || handle == 0) {
        LOG_ERROR("IPCThreadState::releaseHandle failed: invalid state handle=%u", handle);
        return -1;
    }

    const int status = binder_release_handle(binderState, handle);
    if (status != 0) {
        LOG_ERROR("IPCThreadState::releaseHandle failed for handle=%u status=%d", handle, status);
    } else {
        LOG_DEBUG("IPCThreadState::releaseHandle succeeded for handle=%u", handle);
    }
    return status;
}

int IPCThreadState::sendReply(const Parcel& reply) {
    auto* binderState = ProcessState::self().bs();
    if (binderState == nullptr) {
        LOG_ERROR("IPCThreadState::sendReply failed: no binder state");
        return -1;
    }

    const int status = binder_send_reply(
        binderState,
        reply.data(),
        reply.dataSize(),
        reply.offsets(),
        reply.offsetsSize());

    if (status != 0) {
        LOG_ERROR("IPCThreadState::sendReply failed status=%d", status);
    } else {
        LOG_DEBUG("IPCThreadState::sendReply completed replySize=%zu", reply.dataSize());
    }

    return status;
}

void IPCThreadState::joinThreadPool(BBinder* defaultServer) {
    auto* binderState = ProcessState::self().bs();
    if (binderState == nullptr) {
        LOG_ERROR("IPCThreadState: no binder state");
        return;
    }

    struct LoopContext {
        BBinder* defaultServer;
    };

    static thread_local LoopContext loopContext;
    loopContext.defaultServer = defaultServer;

    LOG_INFO("IPCThreadState::joinThreadPool entering binder loop defaultServer=%p",
             static_cast<void*>(defaultServer));

    binder_loop_ctx(
        binderState,
        [](struct binder_state* loopState,
           struct binder_transaction_data* transaction,
           void* context) {
            auto* loop = static_cast<LoopContext*>(context);

            BBinder* target = nullptr;
            if (transaction->cookie != 0) {
                target = reinterpret_cast<BBinder*>(static_cast<uintptr_t>(transaction->cookie));
            } else {
                target = loop->defaultServer;
            }

            if (target == nullptr) {
                LOG_ERROR("IPCThreadState: no handler for cookie=0x%llx",
                     static_cast<unsigned long long>(transaction->cookie));

                if ((transaction->flags & TF_ONE_WAY) == 0) {
                    Parcel errorReply;
                    errorReply.writeInt32(-1);
                    binder_send_reply(
                        loopState,
                        errorReply.data(),
                        errorReply.dataSize(),
                        errorReply.offsets(),
                        errorReply.offsetsSize());
                }
                return;
            }

            const Parcel request(
                reinterpret_cast<const void*>(transaction->data.ptr.buffer),
                transaction->data_size);
            Parcel reply;
            const int status = target->onTransact(transaction->code, request, &reply);

            if ((transaction->flags & TF_ONE_WAY) == 0) {
                if (status == 0) {
                    binder_send_reply(
                        loopState,
                        reply.data(),
                        reply.dataSize(),
                        reply.offsets(),
                        reply.offsetsSize());
                } else {
                    Parcel errorReply;
                    errorReply.writeInt32(-1);
                    binder_send_reply(
                        loopState,
                        errorReply.data(),
                        errorReply.dataSize(),
                        errorReply.offsets(),
                        errorReply.offsetsSize());
                }
            }
        },
        &loopContext);
}

