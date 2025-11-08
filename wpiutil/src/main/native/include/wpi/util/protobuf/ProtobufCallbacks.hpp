// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <span>
#include <utility>
#include <vector>

#include <fmt/format.h>

#include "pb.h"
#include "wpi/SmallVector.h"
#include "wpi/array.h"
#include "wpi/protobuf/Protobuf.h"

namespace wpi {

/**
 * The behavior to use when more elements are in the message then expected when
 * decoding.
 */
enum class DecodeLimits {
  // Ignore any extra elements
  Ignore,
  // Add any extra elements to the backing vector
  Add,
  // Cause decoding to fail if extra elements exist
  Fail,
};

template <class T>
concept StringLike = std::is_convertible_v<T, std::string_view>;

template <class T>
concept ConstVectorLike = std::is_convertible_v<T, std::span<const uint8_t>>;

template <class T>
concept MutableVectorLike = std::is_convertible_v<T, std::span<uint8_t>>;

template <typename T>
concept PackBytes = StringLike<T> || ConstVectorLike<T>;

template <typename T>
concept UnpackBytes = requires(T& t) {
  { t.resize(size_t()) };  // NOLINT
  { t.size() } -> std::same_as<size_t>;
  { t.data() } -> std::convertible_to<void*>;
} && (PackBytes<T> || MutableVectorLike<T>);

template <typename T>
concept ProtoEnumeration = std::is_enum_v<T>;

template <typename T>
concept ProtoPackable =
    ProtoEnumeration<T> || std::integral<T> || std::floating_point<T>;

template <typename T>
concept ProtoCallbackPackable =
    ProtobufSerializable<T> || PackBytes<T> || ProtoPackable<T>;

template <typename T>
concept ProtoCallbackUnpackable =
    ProtobufSerializable<T> || UnpackBytes<T> || ProtoPackable<T>;

namespace detail {

template <typename T>
concept Validatable = ProtoCallbackPackable<T> || ProtoCallbackUnpackable<T>;

template <Validatable T>
constexpr bool ValidateType(pb_type_t type) {
  switch (type) {
    case PB_LTYPE_BOOL:
      return std::integral<T>;
    case PB_LTYPE_VARINT:
      return std::signed_integral<T> || ProtoEnumeration<T>;
    case PB_LTYPE_UVARINT:
      return std::unsigned_integral<T>;
    case PB_LTYPE_SVARINT:
      return std::signed_integral<T>;
    case PB_LTYPE_FIXED32:
      return std::integral<T> || std::floating_point<T>;
    case PB_LTYPE_FIXED64:
      return std::integral<T> || std::floating_point<T>;
    case PB_LTYPE_BYTES:
    case PB_LTYPE_STRING:
      return PackBytes<T> || UnpackBytes<T>;
    case PB_LTYPE_SUBMESSAGE:
      return ProtobufSerializable<T>;
    default:
      return false;
  }
}

}  // namespace detail

/**
 * A callback method that will directly unpack elements into
 * the specified vector like data structure. The size passed
 * is the expected number of elements.
 *
 * By default, any elements in the packed buffer past N will
 * still be added to the vector.
 *
 * @tparam T object type
 * @tparam U vector type to pack into
 * @tparam N number of elements
 */
template <ProtoCallbackUnpackable T, typename U, size_t N = 1>
class DirectUnpackCallback {
 public:
  /**
   * Constructs a callback from a vector like type.
   *
   * @param storage the vector to store into
   */
  explicit DirectUnpackCallback(U& storage) : m_storage{storage} {
    m_callback.funcs.decode = CallbackFunc;
    m_callback.arg = this;
  }
  DirectUnpackCallback(const DirectUnpackCallback&) = delete;
  DirectUnpackCallback(DirectUnpackCallback&&) = delete;
  DirectUnpackCallback& operator=(const DirectUnpackCallback&) = delete;
  DirectUnpackCallback& operator=(DirectUnpackCallback&&) = delete;

  /**
   * Set the limits on what happens if more elements exist in the buffer then
   * expected.
   *
   * @param limit the limit to set
   */
  void SetLimits(DecodeLimits limit) noexcept { m_limits = limit; }

  /**
   * Gets the nanopb callback pointing to this object.
   *
   * @return nanopb callback
   */
  pb_callback_t Callback() const { return m_callback; }

 private:
  bool SizeCheck(bool* retVal) const {
    if (m_storage.size() >= N) {
      switch (m_limits) {
        case DecodeLimits::Ignore:
          *retVal = true;
          return false;

        case DecodeLimits::Add:
          break;

        default:
          *retVal = false;
          return false;
      }
    }
    return true;
  }

  bool Decode(pb_istream_t* stream, pb_type_t fieldType) {
    if constexpr (ProtoPackable<T>) {
      switch (fieldType) {
        case PB_LTYPE_BOOL:
          if constexpr (std::integral<T>) {
            bool val = false;
            if (!pb_decode_bool(stream, &val)) {
              return false;
            }
            m_storage.emplace_back(static_cast<T>(val));
            return true;
          } else {
            return false;
          }
        case PB_LTYPE_VARINT:
          if constexpr (std::signed_integral<T> || ProtoEnumeration<T>) {
            int64_t val = 0;
            if (!pb_decode_varint(stream, reinterpret_cast<uint64_t*>(&val))) {
              return false;
            }
            m_storage.emplace_back(static_cast<T>(val));
            return true;
          } else {
            return false;
          }
        case PB_LTYPE_UVARINT:
          if constexpr (std::unsigned_integral<T>) {
            uint64_t val = 0;
            if (!pb_decode_varint(stream, &val)) {
              return false;
            }
            m_storage.emplace_back(static_cast<T>(val));
            return true;
          } else {
            return false;
          }
        case PB_LTYPE_SVARINT:
          if constexpr (std::signed_integral<T>) {
            int64_t val = 0;
            if (!pb_decode_svarint(stream, &val)) {
              return false;
            }
            m_storage.emplace_back(static_cast<T>(val));
            return true;
          } else {
            return false;
          }
        case PB_LTYPE_FIXED32:
          if constexpr (std::signed_integral<T>) {
            int32_t val = 0;
            if (!pb_decode_fixed32(stream, &val)) {
              return false;
            }
            m_storage.emplace_back(static_cast<T>(val));
            return true;
          } else if constexpr (std::unsigned_integral<T>) {
            uint32_t val = 0;
            if (!pb_decode_fixed32(stream, &val)) {
              return false;
            }
            m_storage.emplace_back(static_cast<T>(val));
            return true;
          }
          if constexpr (std::floating_point<T>) {
            float val = 0;
            if (!pb_decode_fixed32(stream, &val)) {
              return false;
            }
            m_storage.emplace_back(static_cast<T>(val));
            return true;
          } else {
            return false;
          }
        case PB_LTYPE_FIXED64:
          if constexpr (std::signed_integral<T>) {
            int64_t val = 0;
            if (!pb_decode_fixed64(stream, &val)) {
              return false;
            }
            m_storage.emplace_back(static_cast<T>(val));
            return true;
          } else if constexpr (std::unsigned_integral<T>) {
            uint64_t val = 0;
            if (!pb_decode_fixed64(stream, &val)) {
              return false;
            }
            m_storage.emplace_back(static_cast<T>(val));
            return true;
          }
          if constexpr (std::floating_point<T>) {
            double val = 0;
            if (!pb_decode_fixed64(stream, &val)) {
              return false;
            }
            m_storage.emplace_back(static_cast<T>(val));
            return true;
          } else {
            return false;
          }
        default:
          return false;
      }
    } else if constexpr (UnpackBytes<T>) {
      T& space = m_storage.emplace_back(T{});
      space.resize(stream->bytes_left);
      return pb_read(stream, reinterpret_cast<pb_byte_t*>(space.data()),
                     space.size());
    } else if constexpr (ProtobufSerializable<T>) {
      ProtoInputStream<T> istream{stream};
      auto decoded = wpi::Protobuf<T>::Unpack(istream);
      if (decoded.has_value()) {
        m_storage.emplace_back(std::move(decoded.value()));
        return true;
      }
      return false;
    }
  }

  bool CallbackFunc(pb_istream_t* stream, const pb_field_t* field) {
    pb_type_t fieldType = PB_LTYPE(field->type);

    if (!detail::ValidateType<T>(fieldType)) {
      return false;
    }

    // Validate our types
    if constexpr (ProtoPackable<T>) {
      // Handle decode loop
      while (stream->bytes_left > 0) {
        bool sizeRetVal = 0;
        if (!SizeCheck(&sizeRetVal)) {
          return sizeRetVal;
        }

        if (!Decode(stream, fieldType)) {
          return false;
        }
      }
      return true;
    } else {
      // At this point, do the size check
      bool sizeRetVal = 0;
      if (!SizeCheck(&sizeRetVal)) {
        return sizeRetVal;
      }

      // At this point, we're good to decode
      return Decode(stream, fieldType);
    }
  }

  static bool CallbackFunc(pb_istream_t* stream, const pb_field_t* field,
                           void** arg) {
    return reinterpret_cast<DirectUnpackCallback*>(*arg)->CallbackFunc(stream,
                                                                       field);
  }

  U& m_storage;
  pb_callback_t m_callback;
  DecodeLimits m_limits{DecodeLimits::Add};
};

/**
 * A DirectUnpackCallback backed by a SmallVector<T, N>.
 *
 * By default, any elements in the packed buffer past N will
 * be ignored, but decoding will still succeed
 *
 * @tparam T object type
 * @tparam N small vector small size/number of expected elements
 */
template <ProtoCallbackUnpackable T, size_t N = 1>
class UnpackCallback
    : public DirectUnpackCallback<T, wpi::SmallVector<T, N>, N> {
 public:
  /**
   * Constructs an UnpackCallback.
   */
  UnpackCallback()
      : DirectUnpackCallback<T, wpi::SmallVector<T, N>, N>{m_storedBuffer} {
    this->SetLimits(DecodeLimits::Ignore);
  }

  /**
   * Gets a span pointing to the storage buffer.
   *
   * @return storage buffer span
   */
  std::span<T> Items() noexcept { return m_storedBuffer; }

  /**
   * Gets a const span pointing to the storage buffer.
   *
   * @return storage buffer span
   */
  std::span<const T> Items() const noexcept { return m_storedBuffer; }

  /**
   * Gets a reference to the backing small vector.
   *
   * @return small vector reference
   */
  wpi::SmallVector<T, N>& Vec() noexcept { return m_storedBuffer; }

 private:
  wpi::SmallVector<T, N> m_storedBuffer;
};

/**
 * A DirectUnpackCallback backed by a std::vector.
 *
 * By default, any elements in the packed buffer past N will
 * be ignored, but decoding will still succeed
 *
 * @tparam T object type
 * @tparam N number of expected elements
 */
template <ProtoCallbackUnpackable T, size_t N = 1>
class StdVectorUnpackCallback
    : public DirectUnpackCallback<T, std::vector<T>, N> {
 public:
  /**
   * Constructs a StdVectorUnpackCallback.
   */
  StdVectorUnpackCallback()
      : DirectUnpackCallback<T, std::vector<T>, N>{m_storedBuffer} {
    this->SetLimits(DecodeLimits::Ignore);
  }

  /**
   * Gets a span pointing to the storage buffer.
   *
   * @return storage buffer span
   */
  std::span<T> Items() noexcept { return m_storedBuffer; }

  /**
   * Gets a const span pointing to the storage buffer.
   *
   * @return storage buffer span
   */
  std::span<const T> Items() const noexcept { return m_storedBuffer; }

  /**
   * Gets a reference to the backing vector.
   *
   * @return vector reference
   */
  std::vector<T>& Vec() noexcept { return m_storedBuffer; }

 private:
  std::vector<T> m_storedBuffer;
};

/**
 * A wrapper around a wpi::array that lets us
 * treat it as a limited sized vector.
 */
template <ProtoCallbackUnpackable T, size_t N>
struct WpiArrayEmplaceWrapper {
  wpi::array<T, N> m_array{wpi::empty_array_t{}};
  size_t m_currentIndex = 0;

  size_t size() const { return m_currentIndex; }

  template <typename... ArgTypes>
  T& emplace_back(ArgTypes&&... Args) {
    m_array[m_currentIndex] = T(std::forward<ArgTypes>(Args)...);
    m_currentIndex++;
    return m_array[m_currentIndex - 1];
  }
};

/**
 * A DirectUnpackCallback backed by a wpi::array<T, N>.
 *
 * Any elements in the packed buffer past N will
 * be cause decoding to fail.
 *
 * @tparam T object type
 * @tparam N small vector small size/number of expected elements
 */
template <ProtoCallbackUnpackable T, size_t N>
struct WpiArrayUnpackCallback
    : public DirectUnpackCallback<T, WpiArrayEmplaceWrapper<T, N>, N> {
  /**
   * Constructs a WpiArrayUnpackCallback.
   */
  WpiArrayUnpackCallback()
      : DirectUnpackCallback<T, WpiArrayEmplaceWrapper<T, N>, N>{m_array} {
    this->SetLimits(DecodeLimits::Fail);
  }

  /**
   * Returns if the buffer is completely filled up.
   *
   * @return true if buffer is full
   */
  bool IsFull() const noexcept { return m_array.m_currentIndex == N; }

  /**
   * Returns the number of elements in the buffer.
   *
   * @return number of elements
   */
  size_t Size() const noexcept { return m_array.m_currentIndex; }

  /**
   * Returns a reference to the backing array.
   *
   * @return array reference
   */
  wpi::array<T, N>& Array() noexcept { return m_array.m_array; }

 private:
  WpiArrayEmplaceWrapper<T, N> m_array;
};

/**
 * A callback method that will pack elements when called.
 *
 * @tparam T object type
 */
template <ProtoCallbackPackable T>
class PackCallback {
 public:
  /**
   * Constructs a pack callback from a span of elements. The elements in the
   * buffer _MUST_ stay alive throughout the entire encode call.
   */
  explicit PackCallback(std::span<const T> buffer) : m_buffer{buffer} {
    m_callback.funcs.encode = CallbackFunc;
    m_callback.arg = this;
  }

  /**
   * Constructs a pack callback from a pointer to a single element.
   * This element _MUST_ stay alive throughout the entire encode call.
   * Do not pass a temporary here (This is why its a pointer and not a
   * reference)
   */
  explicit PackCallback(const T* element)
      : m_buffer{std::span<const T>{element, 1}} {
    m_callback.funcs.encode = CallbackFunc;
    m_callback.arg = this;
  }
  PackCallback(const PackCallback&) = delete;
  PackCallback(PackCallback&&) = delete;
  PackCallback& operator=(const PackCallback&) = delete;
  PackCallback& operator=(PackCallback&&) = delete;

  /**
   * Gets the nanopb callback pointing to this object.
   *
   * @return nanopb callback
   */
  pb_callback_t Callback() const { return m_callback; }

  /**
   * Gets a span pointing to the items
   *
   * @return span
   */
  std::span<const T> Bufs() const { return m_buffer; }

 private:
  static auto EncodeStreamTypeFinder() {
    if constexpr (ProtobufSerializable<T>) {
      return ProtoOutputStream<T>(nullptr);
    } else {
      return pb_ostream_t{};
    }
  }
  using EncodeStreamType = decltype(EncodeStreamTypeFinder());

  bool EncodeItem(EncodeStreamType& stream, const pb_field_t* field,
                  const T& value) const {
    if constexpr (std::floating_point<T>) {
      pb_type_t fieldType = PB_LTYPE(field->type);
      switch (fieldType) {
        case PB_LTYPE_FIXED32: {
          float flt = static_cast<float>(value);
          return pb_encode_fixed32(&stream, &flt);
        }
        case PB_LTYPE_FIXED64: {
          double dbl = static_cast<double>(value);
          return pb_encode_fixed64(&stream, &dbl);
        }
        default:
          return false;
      }
    } else if constexpr (std::integral<T> || ProtoEnumeration<T>) {
      pb_type_t fieldType = PB_LTYPE(field->type);
      switch (fieldType) {
        case PB_LTYPE_BOOL:
        case PB_LTYPE_VARINT:
        case PB_LTYPE_UVARINT:
          return pb_encode_varint(&stream, value);
        case PB_LTYPE_SVARINT:
          return pb_encode_svarint(&stream, value);
        case PB_LTYPE_FIXED32: {
          uint32_t f = value;
          return pb_encode_fixed32(&stream, &f);
        }
        case PB_LTYPE_FIXED64: {
          uint64_t f = value;
          return pb_encode_fixed64(&stream, &f);
        }
        default:
          return false;
      }
    } else if constexpr (StringLike<T>) {
      std::string_view view{value};
      return pb_encode_string(&stream,
                              reinterpret_cast<const pb_byte_t*>(view.data()),
                              view.size());
    } else if constexpr (ConstVectorLike<T>) {
      std::span<const uint8_t> view{value};
      return pb_encode_string(&stream,
                              reinterpret_cast<const pb_byte_t*>(view.data()),
                              view.size());
    } else if constexpr (ProtobufSerializable<T>) {
      return wpi::Protobuf<T>::Pack(stream, value);
    }
  }

  bool EncodeLoop(pb_ostream_t* stream, const pb_field_t* field,
                  bool writeTag) const {
    if constexpr (ProtobufSerializable<T>) {
      ProtoOutputStream<T> ostream{stream};
      for (auto&& i : m_buffer) {
        if (writeTag) {
          if (!pb_encode_tag_for_field(stream, field)) {
            return false;
          }
        }
        if (!EncodeItem(ostream, field, i)) {
          return false;
        }
      }
    } else {
      for (auto&& i : m_buffer) {
        if (writeTag) {
          if (!pb_encode_tag_for_field(stream, field)) {
            return false;
          }
        }
        if (!EncodeItem(*stream, field, i)) {
          return false;
        }
      }
    }

    return true;
  }

  bool PackedEncode(pb_ostream_t* stream, const pb_field_t* field) const {
    // We're always going to used packed encoding.
    // So first we need to get the packed size.

    pb_ostream_t substream = PB_OSTREAM_SIZING;
    if (!EncodeLoop(&substream, field, false)) {
      return false;
    }

    // Encode as a string tag
    if (!pb_encode_tag(stream, PB_WT_STRING, field->tag)) {
      return false;
    }

    // Write length as varint
    size_t size = substream.bytes_written;
    if (!pb_encode_varint(stream, static_cast<uint64_t>(size))) {
      return false;
    }

    return EncodeLoop(stream, field, false);
  }

  bool CallbackFunc(pb_ostream_t* stream, const pb_field_t* field) const {
    // First off, if we're empty, do nothing, but say we were successful
    if (m_buffer.empty()) {
      return true;
    }

    pb_type_t fieldType = PB_LTYPE(field->type);

    if (!detail::ValidateType<T>(fieldType)) {
      return false;
    }

    if constexpr (ProtoPackable<T>) {
      return PackedEncode(stream, field);
    } else {
      return EncodeLoop(stream, field, true);
    }
  }

  static bool CallbackFunc(pb_ostream_t* stream, const pb_field_t* field,
                           void* const* arg) {
    return reinterpret_cast<const PackCallback*>(*arg)->CallbackFunc(stream,
                                                                     field);
  }

  std::span<const T> m_buffer;
  pb_callback_t m_callback;
};

}  // namespace wpi
