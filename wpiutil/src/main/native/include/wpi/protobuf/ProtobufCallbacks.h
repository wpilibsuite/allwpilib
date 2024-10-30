// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include "wpi/protobuf/Protobuf.h"
#include "wpi/SmallVector.h"

#include <span>

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
concept UnpackBytes = requires(T& t) {
  { t.resize(size_t()) };
  { t.size() } -> std::same_as<size_t>;
  { t.data() } -> std::convertible_to<void*>;
} && (PackBytes<T> || MutableVectorLike<T> || ConstVectorLike<T>);

static_assert(UnpackBytes<std::string>);
static_assert(UnpackBytes<std::vector<uint8_t>>);
static_assert(UnpackBytes<wpi::SmallVector<uint8_t, 128>>);

  enum class Limits {
    Ignore,
    Add,
    Fail,
  };

template <typename T, typename U, size_t N = 1>
class DirectUnpackCallback {
 public:

  DirectUnpackCallback(U& storage) : m_storage{storage} {
    m_callback.funcs.decode = CallbackFunc;
    m_callback.arg = this;
  }
  DirectUnpackCallback(const DirectUnpackCallback&) = delete;
  DirectUnpackCallback(DirectUnpackCallback&&) = delete;
  DirectUnpackCallback& operator=(const DirectUnpackCallback&) = delete;
  DirectUnpackCallback& operator=(DirectUnpackCallback&&) = delete;

  // TODO, do we want a limit that is higher then the small size?

  void SetLimits(Limits limit) noexcept { m_limits = limit; }

  pb_callback_t Callback() const { return m_callback; }

 private:
  bool SizeCheck(bool* retVal) const {
    if (m_storage.size() >= N) {
      switch (m_limits) {
        case Limits::Ignore:
          *retVal = true;
          return false;

        case Limits::Add:
          break;

        default:
          *retVal = false;
          return false;
      }
    }
    return true;
  }

  bool FixedDecodeFunc(pb_istream_t* stream, const pb_field_t* field) {
    bool sizeRetVal = 0;
    if (!SizeCheck(&sizeRetVal)) {
      return sizeRetVal;
    }

    pb_type_t fieldType = PB_LTYPE(field->type);

    if constexpr (std::floating_point<T>) {
      switch (fieldType) {
        case PB_LTYPE_FIXED32: {
          float flt = 0;
          if (!pb_decode_fixed32(stream, &flt)) {
            return false;
          }
          m_storage.emplace_back(static_cast<T>(flt));
          return true;
        }
        case PB_LTYPE_FIXED64: {
          double dbl = 0;
          if (!pb_decode_fixed64(stream, &dbl)) {
            return false;
          }
          m_storage.emplace_back(static_cast<T>(dbl));
          return true;
        }
        default:
          return false;
      }
    } else if constexpr (std::integral<T>) {
      switch (fieldType) {
        case PB_LTYPE_BOOL: {
          bool bl = 0;
          if (!pb_decode_bool(stream, &bl)) {
            return false;
          }
          m_storage.emplace_back(static_cast<T>(bl));
          return true;
        }
        case PB_LTYPE_VARINT: {
          int64_t vint = 0;
          if (!pb_decode_varint(stream, reinterpret_cast<uint64_t*>(&vint))) {
            return false;
          }
          m_storage.emplace_back(static_cast<T>(vint));
          return true;
        }
        case PB_LTYPE_UVARINT: {
          uint64_t uvint = 0;
          if (!pb_decode_varint(stream, &uvint)) {
            return false;
          }
          m_storage.emplace_back(static_cast<T>(uvint));
          return true;
        }
        case PB_LTYPE_SVARINT: {
          int64_t svint = 0;
          if (!pb_decode_svarint(stream, &svint)) {
            return false;
          }
          m_storage.emplace_back(static_cast<T>(svint));
          return true;
        }
        case PB_LTYPE_FIXED32: {
          // See if we're signed or unsigned
          if constexpr (std::signed_integral<T>) {
            int32_t flt = 0;
            if (!pb_decode_fixed32(stream, &flt)) {
              return false;
            }
            m_storage.emplace_back(static_cast<T>(flt));
          } else {
            uint32_t flt = 0;
            if (!pb_decode_fixed32(stream, &flt)) {
              return false;
            }
            m_storage.emplace_back(static_cast<T>(flt));
          }

          return true;
        }
        case PB_LTYPE_FIXED64: {
          // See if we're signed or unsigned
          if constexpr (std::signed_integral<T>) {
            int64_t flt = 0;
            if (!pb_decode_fixed64(stream, &flt)) {
              return false;
            }
            m_storage.emplace_back(static_cast<T>(flt));
          } else {
            uint64_t flt = 0;
            if (!pb_decode_fixed64(stream, &flt)) {
              return false;
            }
            m_storage.emplace_back(static_cast<T>(flt));
          }
          return true;
        }
        default:
          return false;
      }
    }
  }

  bool CallbackFunc(pb_istream_t* stream, const pb_field_t* field) {
    if constexpr (std::integral<T> || std::floating_point<T>) {
      while (stream->bytes_left > 0) {
        bool status = FixedDecodeFunc(stream, field);
        if (!status) {
          return false;
        }
      }
      return true;
    } else {
      bool sizeRetVal = 0;
      if (!SizeCheck(&sizeRetVal)) {
        return sizeRetVal;
      }
      if constexpr (UnpackBytes<T>) {
        T& space = m_storage.emplace_back(T{});
        space.resize(stream->bytes_left);
        return pb_read(stream, reinterpret_cast<pb_byte_t*>(space.data()),
                       space.size());
      } else {
        ProtoInputStream istream{stream, wpi::Protobuf<T>::Message()};
        std::optional<T> decoded = wpi::Protobuf<T>::Unpack(istream);
        if (decoded.has_value()) {
          m_storage.emplace_back(std::move(decoded.value()));
          return true;
        }
        return false;
      }
    }
  }
  static bool CallbackFunc(pb_istream_t* stream, const pb_field_t* field,
                           void** arg) {
    return reinterpret_cast<DirectUnpackCallback*>(*arg)->CallbackFunc(stream, field);
  }

  U& m_storage;
  pb_callback_t m_callback;
  Limits m_limits{Limits::Add};
};

template <typename T, size_t N = 1>
class UnpackCallback
    : public DirectUnpackCallback<T, wpi::SmallVector<T, N>, N> {
 public:
  UnpackCallback() : DirectUnpackCallback<T, wpi::SmallVector<T, N>, N> {m_storedBuffer} {
    this->SetLimits(Limits::Ignore);
  }

  std::span<T> Items() noexcept { return m_storedBuffer; }

  std::span<const T> Items() const noexcept { return m_storedBuffer; }

 private:
  wpi::SmallVector<T, N> m_storedBuffer;
};

template <typename T>
class PackCallback {
 public:
  PackCallback(std::span<const T> buffer) : m_buffer{buffer} {
    m_callback.funcs.encode = CallbackFunc;
    m_callback.arg = this;
  }
  PackCallback(const T* element) : m_buffer{std::span<const T>{element, 1}} {
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
  bool FixedFieldFunc(pb_ostream_t* stream, const pb_field_t* field,
                      T value) const {
    pb_type_t fieldType = PB_LTYPE(field->type);

    // First, check float or double.
    if constexpr (std::floating_point<T>) {
      switch (fieldType) {
        case PB_LTYPE_FIXED32: {
          float flt = static_cast<float>(value);
          return pb_encode_fixed32(stream, &flt);
        }
        case PB_LTYPE_FIXED64: {
          double dbl = static_cast<double>(value);
          return pb_encode_fixed64(stream, &dbl);
        }
        default:
          return false;
      }
    } else if constexpr (std::integral<T>) {
      switch (fieldType) {
        case PB_LTYPE_BOOL:
        case PB_LTYPE_VARINT:
        case PB_LTYPE_UVARINT:
          return pb_encode_varint(stream, value);
        case PB_LTYPE_SVARINT:
          return pb_encode_svarint(stream, value);
        case PB_LTYPE_FIXED32: {
          uint32_t f = value;
          return pb_encode_fixed32(stream, &f);
        }
        case PB_LTYPE_FIXED64: {
          uint64_t f = value;
          return pb_encode_fixed64(stream, &f);
        }
        default:
          return false;
      }
    } else {
      // We don't know how to encode this, just return false
      return false;
    }
  }

  bool FixedCallbackFuncInternal(pb_ostream_t* stream,
                                 const pb_field_t* field) const {
    if constexpr (std::integral<T> || std::floating_point<T>) {
      for (auto&& i : m_buffer) {
        bool status = FixedFieldFunc(stream, field, i);
        if (!status) {
          return false;
        }
      }
      return true;
    } else {
      return false;
    }
  }

  bool FixedCallbackFunc(pb_ostream_t* stream, const pb_field_t* field) const {
    if constexpr (std::integral<T> || std::floating_point<T>) {
      // We're always going to used packed encoding.
      // So first we need to get the packed size.

      pb_ostream_t substream = PB_OSTREAM_SIZING;
      if (!FixedCallbackFuncInternal(&substream, field)) {
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

      return FixedCallbackFuncInternal(stream, field);
    } else {
      return false;
    }
  }

  bool CallbackFunc(pb_ostream_t* stream, const pb_field_t* field) const {
    // First off, if we're empty, do nothing, but say we were successful
    if (m_buffer.empty()) {
      return true;
    }

    if (PB_LTYPE(field->type) <= PB_LTYPE_LAST_PACKABLE) {
      if constexpr (std::integral<T> || std::floating_point<T>) {
        return FixedCallbackFunc(stream, field);
      } else {
        return false;
      }
    }

    const pb_msgdesc_t* desc = nullptr;
    if constexpr (!(PackBytes<T> || std::integral<T> || std::floating_point<T>)) {
      desc = wpi::Protobuf<T>::Message();
    }
    ProtoOutputStream ostream{stream, desc};
    for (auto&& i : m_buffer) {
      if (!pb_encode_tag_for_field(stream, field)) {
        return false;
      }

      bool success;
      if constexpr (StringLike<T>) {
        std::string_view view{i};
        success = pb_encode_string(
            stream, reinterpret_cast<const pb_byte_t*>(view.data()),
            view.size());
      } else if constexpr (ConstVectorLike<T>) {
        std::span<const uint8_t> view{i};
        success = pb_encode_string(
            stream, reinterpret_cast<const pb_byte_t*>(view.data()),
            view.size());
      } else if constexpr (std::integral<T> || std::floating_point<T>) {
        return false;
      } else {
        success = wpi::Protobuf<T>::Pack(ostream, i);
      }
      if (!success) {
        return false;
      }
    }
    return true;
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
