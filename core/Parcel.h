#pragma once

#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>

extern "C" {
#include "binder.h"
}

namespace ipc {

class BBinder;

/**
 * @brief Binder Parcel serialization container.
 *
 * Parcel stores primitive values and binder object references in the wire
 * format expected by the Linux binder driver. Binder object payloads are
 * aligned to 4-byte boundaries before offset registration.
 */
class Parcel {
public:
    /** @brief Create an empty writable parcel. */
    Parcel() = default;

    /**
     * @brief Create a read-only view from binder transaction bytes.
     *
     * @param data Pointer to transaction buffer.
     * @param size Buffer size in bytes.
     */
    Parcel(const void* data, size_t size);

    /** @brief Append an `int32_t` value. */
    void writeInt32(int32_t value);

    /** @brief Append a `uint32_t` value. */
    void writeUint32(uint32_t value);

    /**
     * @brief Append a length-prefixed UTF-8 byte string.
     *
     * @param value UTF-8 content.
     */
    void writeString(const std::string& value);

    /**
     * @brief Append a local binder object reference.
     *
     * The binder driver translates this object to a remote handle in the
     * target process.
     *
     * @param localBinder Pointer to the local binder implementation.
     */
    void writeStrongBinder(const BBinder* localBinder);

    /**
     * @brief Append a binder handle for forwarding through another process.
     *
     * @param handle Binder handle value.
     */
    void writeBinderHandle(uint32_t handle);

    /** @brief Read an `int32_t` value. Returns 0 if out-of-range. */
    int32_t readInt32();

    /** @brief Read a `uint32_t` value. Returns 0 if out-of-range. */
    uint32_t readUint32();

    /** @brief Read a length-prefixed UTF-8 string. Returns empty on failure. */
    std::string readString();

    /**
     * @brief Read a binder handle object.
     *
     * The read cursor is aligned to binder object boundaries before parsing.
     *
     * @return Non-zero handle on success, 0 on parse failure.
     */
    uint32_t readBinderHandle();

    /** @brief Return raw serialized buffer pointer. */
    const uint8_t* data() const;

    /** @brief Return serialized buffer size. */
    size_t dataSize() const;

    /** @brief Return binder object offsets for transaction metadata. */
    const binder_size_t* offsets() const;

    /** @brief Return size in bytes of the offsets buffer. */
    size_t offsetsSize() const;

    /** @brief Return true when this parcel contains binder objects. */
    bool hasBinderObjects() const;

    /** @brief Return current read cursor position. */
    size_t readPos() const;

    /** @brief Return true when unread bytes are available. */
    bool hasData() const;

private:
    static constexpr size_t BINDER_OBJECT_ALIGNMENT = sizeof(uint32_t);

    static size_t alignUp(size_t value, size_t alignment);
    void alignWriteForBinderObject();
    bool alignReadForBinderObject();
    void append(const void* buffer, size_t size);
    void readBytes(void* buffer, size_t size);

    std::vector<uint8_t> m_data;
    std::vector<binder_size_t> m_offsets;
    size_t m_readPos = 0;
};

} // namespace ipc
