#include "Parcel.h"

#include <cstring>

namespace ipc {

namespace {
constexpr uint32_t DEFAULT_BINDER_FLAGS = 0x7f;
} // namespace

Parcel::Parcel(const void* data, size_t size) {
    if (data == nullptr || size == 0) {
        return;
    }

    const auto* bytes = static_cast<const uint8_t*>(data);
    m_data.assign(bytes, bytes + size);
}

void Parcel::writeInt32(int32_t value) {
    append(&value, sizeof(value));
}

void Parcel::writeUint32(uint32_t value) {
    append(&value, sizeof(value));
}

void Parcel::writeString(const std::string& value) {
    const uint32_t length = static_cast<uint32_t>(value.size());
    writeUint32(length);
    append(value.data(), length);
}

void Parcel::writeStrongBinder(const BBinder* localBinder) {
    if (localBinder == nullptr) {
        LOGE("Parcel::writeStrongBinder: localBinder is null");
        return;
    }

    alignWriteForBinderObject();
    const binder_size_t offset = static_cast<binder_size_t>(m_data.size());

    struct flat_binder_object object;
    std::memset(&object, 0, sizeof(object));
    object.hdr.type = BINDER_TYPE_BINDER;
    object.flags = DEFAULT_BINDER_FLAGS;
    object.binder = reinterpret_cast<binder_uintptr_t>(const_cast<BBinder*>(localBinder));
    object.cookie = reinterpret_cast<binder_uintptr_t>(const_cast<BBinder*>(localBinder));

    append(&object, sizeof(object));
    m_offsets.push_back(offset);
}

void Parcel::writeBinderHandle(uint32_t handle) {
    alignWriteForBinderObject();
    const binder_size_t offset = static_cast<binder_size_t>(m_data.size());

    struct flat_binder_object object;
    std::memset(&object, 0, sizeof(object));
    object.hdr.type = BINDER_TYPE_HANDLE;
    object.flags = DEFAULT_BINDER_FLAGS;
    object.handle = handle;
    object.cookie = 0;

    append(&object, sizeof(object));
    m_offsets.push_back(offset);
}

int32_t Parcel::readInt32() {
    int32_t value = 0;
    readBytes(&value, sizeof(value));
    return value;
}

uint32_t Parcel::readUint32() {
    uint32_t value = 0;
    readBytes(&value, sizeof(value));
    return value;
}

std::string Parcel::readString() {
    const uint32_t length = readUint32();
    if (m_readPos + length > m_data.size()) {
        return {};
    }

    std::string value(reinterpret_cast<const char*>(m_data.data() + m_readPos), length);
    m_readPos += length;
    return value;
}

uint32_t Parcel::readBinderHandle() {
    if (!alignReadForBinderObject()) {
        LOGE("Parcel::readBinderHandle: invalid aligned read position");
        return 0;
    }

    struct flat_binder_object object;
    if (m_readPos + sizeof(object) > m_data.size()) {
        LOGE("Parcel::readBinderHandle: not enough data");
        return 0;
    }

    std::memcpy(&object, m_data.data() + m_readPos, sizeof(object));
    m_readPos += sizeof(object);

    if (object.hdr.type == BINDER_TYPE_HANDLE) {
        return object.handle;
    }

    LOGE("Parcel::readBinderHandle: unexpected type 0x%x (expected HANDLE)",
         object.hdr.type);
    return 0;
}

const uint8_t* Parcel::data() const {
    return m_data.data();
}

size_t Parcel::dataSize() const {
    return m_data.size();
}

const binder_size_t* Parcel::offsets() const {
    return m_offsets.empty() ? nullptr : m_offsets.data();
}

size_t Parcel::offsetsSize() const {
    return m_offsets.size() * sizeof(binder_size_t);
}

bool Parcel::hasBinderObjects() const {
    return !m_offsets.empty();
}

size_t Parcel::readPos() const {
    return m_readPos;
}

bool Parcel::hasData() const {
    return m_readPos < m_data.size();
}

size_t Parcel::alignUp(size_t value, size_t alignment) {
    return (value + alignment - 1) & ~(alignment - 1);
}

void Parcel::alignWriteForBinderObject() {
    const size_t aligned = alignUp(m_data.size(), BINDER_OBJECT_ALIGNMENT);
    if (aligned > m_data.size()) {
        m_data.resize(aligned, 0);
    }
}

bool Parcel::alignReadForBinderObject() {
    const size_t aligned = alignUp(m_readPos, BINDER_OBJECT_ALIGNMENT);
    if (aligned > m_data.size()) {
        return false;
    }

    m_readPos = aligned;
    return true;
}

void Parcel::append(const void* buffer, size_t size) {
    const auto* bytes = static_cast<const uint8_t*>(buffer);
    m_data.insert(m_data.end(), bytes, bytes + size);
}

void Parcel::readBytes(void* buffer, size_t size) {
    if (m_readPos + size > m_data.size()) {
        std::memset(buffer, 0, size);
        return;
    }

    std::memcpy(buffer, m_data.data() + m_readPos, size);
    m_readPos += size;
}

} // namespace ipc
