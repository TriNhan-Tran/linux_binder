#include "BinderCore.hpp"

#include <array>

#include "Parcel.hpp"

namespace ipc {

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
        return;
    }

    m_bs = binder_open(driver);
    if (m_bs == nullptr) {
        LOGE("ProcessState: cannot open '%s'", driver);
    }
}

void ProcessState::close() {
    if (m_bs == nullptr) {
        return;
    }

    binder_close(m_bs);
    m_bs = nullptr;
}

BpBinder::BpBinder(uint32_t handle)
    : m_handle(handle) {
}

uint32_t BpBinder::handle() const {
    return m_handle;
}

int BpBinder::transact(uint32_t code, const Parcel& data, Parcel* reply) {
    auto* binderState = ProcessState::self().bs();
    if (binderState == nullptr) {
        return -1;
    }

    std::array<uint8_t, 4096> replyBuffer{};
    size_t replySize = 0;
    const int status = binder_transact2(
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
        return status;
    }

    if (reply != nullptr && replySize > 0) {
        *reply = Parcel(replyBuffer.data(), replySize);
    }

    return 0;
}

int BpBinder::send(uint32_t code, const Parcel& data) {
    auto* binderState = ProcessState::self().bs();
    if (binderState == nullptr) {
        return -1;
    }

    // binder_call2 expects mutable data pointer but does not modify payload.
    return binder_call2(
        binderState,
        m_handle,
        code,
        const_cast<uint8_t*>(data.data()),
        data.dataSize(),
        data.offsets(),
        data.offsetsSize());
}

int IPCThreadState::becomeContextManager() {
    auto* binderState = ProcessState::self().bs();
    if (binderState == nullptr) {
        return -1;
    }

    return binder_become_context_manager(binderState);
}

int IPCThreadState::acquireHandle(uint32_t handle) {
    auto* binderState = ProcessState::self().bs();
    if (binderState == nullptr || handle == 0) {
        return -1;
    }

    return binder_acquire_handle(binderState, handle);
}

int IPCThreadState::releaseHandle(uint32_t handle) {
    auto* binderState = ProcessState::self().bs();
    if (binderState == nullptr || handle == 0) {
        return -1;
    }

    return binder_release_handle(binderState, handle);
}

int IPCThreadState::sendReply(const Parcel& reply) {
    auto* binderState = ProcessState::self().bs();
    if (binderState == nullptr) {
        return -1;
    }

    return binder_send_reply2(
        binderState,
        reply.data(),
        reply.dataSize(),
        reply.offsets(),
        reply.offsetsSize());
}

void IPCThreadState::joinThreadPool(BBinder* defaultServer) {
    auto* binderState = ProcessState::self().bs();
    if (binderState == nullptr) {
        LOGE("IPCThreadState: no binder state");
        return;
    }

    struct LoopContext {
        BBinder* defaultServer;
    };

    static thread_local LoopContext loopContext;
    loopContext.defaultServer = defaultServer;

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
                LOGE("IPCThreadState: no handler for cookie=0x%llx",
                     static_cast<unsigned long long>(transaction->cookie));

                if ((transaction->flags & TF_ONE_WAY) == 0) {
                    Parcel errorReply;
                    errorReply.writeInt32(-1);
                    binder_send_reply2(
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
                    binder_send_reply2(
                        loopState,
                        reply.data(),
                        reply.dataSize(),
                        reply.offsets(),
                        reply.offsetsSize());
                } else {
                    Parcel errorReply;
                    errorReply.writeInt32(-1);
                    binder_send_reply2(
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

} // namespace ipc
