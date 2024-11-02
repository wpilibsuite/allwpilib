// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include "wpi/protobuf/Protobuf.h"
#include "wpi/SmallVector.h"

#include "wpi/array.h"

#include <span>
#include <utility>
#include <vector>

#include "pb.h"

#include <fmt/format.h>

namespace wpi {

template <class T>
concept StringLike = std::is_convertible_v<T, std::string_view>;

template <class T>
concept ConstVectorLike = std::is_convertible_v<T, std::span<const uint8_t>>;

template <class T>
concept MutableVectorLike = std::is_convertible_v<T, std::span<uint8_t>>;

template <typename T>
concept PackBytes = StringLike<T> || ConstVectorLike<T>;

template <typename T>
concept UnpackBytes = (PackBytes<T> || MutableVectorLike<T>) && requires(T& t) {
  { t.resize(size_t()) };
  { t.size() } -> std::same_as<size_t>;
  { t.data() } -> std::convertible_to<void*>;
};

enum class DecodeLimits {
  Ignore,
  Add,
  Fail,
};

namespace detail {
template <typename T>
concept enumeration = std::is_enum_v<T>;

template <typename T>
concept Packable = enumeration<T> || std::integral<T> || std::floating_point<T>;
}  // namespace detail

template <typename T>
constexpr bool ValidateType(pb_type_t type) {
  switch (type) {
    case PB_LTYPE_BOOL:
      return std::integral<T>;
    case PB_LTYPE_VARINT:
      return std::signed_integral<T> || detail::enumeration<T>;
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

template <typename T, typename U, size_t N = 1>
class DirectUnpackCallback {
 public:
  explicit DirectUnpackCallback(U& storage) : m_storage{storage} {
    m_callback.funcs.decode = CallbackFunc;
    m_callback.arg = this;
  }
  DirectUnpackCallback(const DirectUnpackCallback&) = delete;
  DirectUnpackCallback(DirectUnpackCallback&&) = delete;
  DirectUnpackCallback& operator=(const DirectUnpackCallback&) = delete;
  DirectUnpackCallback& operator=(DirectUnpackCallback&&) = delete;

  void SetLimits(DecodeLimits limit) noexcept { m_limits = limit; }

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
    if constexpr (detail::Packable<T>) {
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
          if constexpr (std::signed_integral<T> || detail::enumeration<T>) {
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
      ProtoInputStream istream{stream, wpi::Protobuf<T>::Message()};
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

    if (!ValidateType<T>(fieldType)) {
      return false;
    }

    // Validate our types
    if constexpr (detail::Packable<T>) {
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

template <typename T, size_t N = 1>
class UnpackCallback
    : public DirectUnpackCallback<T, wpi::SmallVector<T, N>, N> {
 public:
  UnpackCallback()
      : DirectUnpackCallback<T, wpi::SmallVector<T, N>, N>{m_storedBuffer} {
    this->SetLimits(DecodeLimits::Ignore);
  }

  std::span<T> Items() noexcept { return m_storedBuffer; }

  std::span<const T> Items() const noexcept { return m_storedBuffer; }

  wpi::SmallVector<T, N>& Vec() noexcept { return m_storedBuffer; }

 private:
  wpi::SmallVector<T, N> m_storedBuffer;
};

template <typename T, size_t N = 1>
class StdVectorUnpackCallback
    : public DirectUnpackCallback<T, std::vector<T>, N> {
 public:
  StdVectorUnpackCallback()
      : DirectUnpackCallback<T, std::vector<T>, N>{m_storedBuffer} {
    this->SetLimits(DecodeLimits::Ignore);
  }

  std::span<T> Items() noexcept { return m_storedBuffer; }

  std::span<const T> Items() const noexcept { return m_storedBuffer; }

  std::vector<T>& Vec() noexcept { return m_storedBuffer; }

 private:
  std::vector<T> m_storedBuffer;
};

template <typename T, size_t N>
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

template <typename T, size_t N>
struct WpiArrayUnpackCallback
    : public DirectUnpackCallback<T, WpiArrayEmplaceWrapper<T, N>, N> {
  WpiArrayUnpackCallback()
      : DirectUnpackCallback<T, WpiArrayEmplaceWrapper<T, N>, N>{m_array} {
    this->SetLimits(DecodeLimits::Fail);
  }

  bool IsFull() const noexcept { return m_array.m_currentIndex == N; }

  size_t Size() const noexcept { return m_array.m_currentIndex; }

  wpi::array<T, N>& Array() noexcept { return m_array.m_array; }

 private:
  WpiArrayEmplaceWrapper<T, N> m_array;
};

template <typename T>
concept ProtoPackable = ProtobufSerializable<T> || PackBytes<T> ||
                        std::integral<T> || std::floating_point<T>;

template <ProtoPackable T>
class PackCallback {
 public:
  explicit PackCallback(std::span<const T> buffer) : m_buffer{buffer} {
    m_callback.funcs.encode = CallbackFunc;
    m_callback.arg = this;
  }
  explicit PackCallback(const T* element)
      : m_buffer{std::span<const T>{element, 1}} {
    m_callback.funcs.encode = CallbackFunc;
    m_callback.arg = this;
  }
  PackCallback(const PackCallback&) = delete;
  PackCallback(PackCallback&&) = delete;
  PackCallback& operator=(const PackCallback&) = delete;
  PackCallback& operator=(PackCallback&&) = delete;

  pb_callback_t Callback() const { return m_callback; }

  std::span<const T> Bufs() const { return m_buffer; }

 private:
  bool EncodeItem(ProtoOutputStream& stream, const pb_field_t* field,
                  const T& value) const {
    if constexpr (std::floating_point<T>) {
      pb_type_t fieldType = PB_LTYPE(field->type);
      switch (fieldType) {
        case PB_LTYPE_FIXED32: {
          float flt = static_cast<float>(value);
          return pb_encode_fixed32(stream.Stream(), &flt);
        }
        case PB_LTYPE_FIXED64: {
          double dbl = static_cast<double>(value);
          return pb_encode_fixed64(stream.Stream(), &dbl);
        }
        default:
          return false;
      }
    } else if constexpr (std::integral<T> || detail::enumeration<T>) {
      pb_type_t fieldType = PB_LTYPE(field->type);
      switch (fieldType) {
        case PB_LTYPE_BOOL:
        case PB_LTYPE_VARINT:
        case PB_LTYPE_UVARINT:
          return pb_encode_varint(stream.Stream(), value);
        case PB_LTYPE_SVARINT:
          return pb_encode_svarint(stream.Stream(), value);
        case PB_LTYPE_FIXED32: {
          uint32_t f = value;
          return pb_encode_fixed32(stream.Stream(), &f);
        }
        case PB_LTYPE_FIXED64: {
          uint64_t f = value;
          return pb_encode_fixed64(stream.Stream(), &f);
        }
        default:
          return false;
      }
    } else if constexpr (StringLike<T>) {
      std::string_view view{value};
      return pb_encode_string(stream.Stream(),
                              reinterpret_cast<const pb_byte_t*>(view.data()),
                              view.size());
    } else if constexpr (ConstVectorLike<T>) {
      std::span<const uint8_t> view{value};
      return pb_encode_string(stream.Stream(),
                              reinterpret_cast<const pb_byte_t*>(view.data()),
                              view.size());
    } else if constexpr (ProtobufSerializable<T>) {
      return wpi::Protobuf<T>::Pack(stream, value);
    }
  }

  bool EncodeLoop(pb_ostream_t* stream, const pb_field_t* field,
                  bool writeTag) const {
    const pb_msgdesc_t* desc = nullptr;
    if constexpr (ProtobufSerializable<T>) {
      desc = wpi::Protobuf<T>::Message();
    }
    ProtoOutputStream ostream{stream, desc};
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

    if (!ValidateType<T>(fieldType)) {
      return false;
    }

    if constexpr (detail::Packable<T>) {
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
