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

using UnpackStringType = std::string;
using UnpackBytesType = wpi::SmallVector<uint8_t, 128>;

using PackStringType = std::string_view;
using PackBytesType = std::span<const uint8_t>;

template <typename T, size_t N = 1>
class UnpackCallback {
 public:
  enum class Limits {
    Ignore,
    Add,
    Fail,
  };

  UnpackCallback() {
    m_callback.funcs.decode = CallbackFunc;
    m_callback.arg = this;
  }
  UnpackCallback(const UnpackCallback&) = delete;
  UnpackCallback(UnpackCallback&&) = delete;
  UnpackCallback& operator=(const UnpackCallback&) = delete;
  UnpackCallback& operator=(UnpackCallback&&) = delete;

  // TODO, do we want a limit that is higher then the small size?

  void SetLimits(Limits limit) noexcept { m_limits = limit; }

  pb_callback_t Callback() const { return m_callback; }

  std::span<T> Items() noexcept { return m_storage; }

  std::span<const T> Items() const noexcept { return m_storage; }

 private:
  bool CallbackFunc(pb_istream_t* stream, const pb_field_t* field) {
    if (m_storage.size() >= N) {
      switch (m_limits) {
        case Limits::Ignore:
          return true;

        case Limits::Add:
          break;

        default:
          return false;
      }
    }

    if constexpr (std::same_as<T, UnpackStringType>) {
      T& space = m_storage.emplace_back(UnpackStringType{});
      space.resize(stream->bytes_left);
      return pb_read(stream, reinterpret_cast<pb_byte_t*>(space.data()),
                     space.size());
    } else if constexpr (std::same_as<T, UnpackBytesType>) {
      T& space = m_storage.emplace_back(UnpackBytesType{});
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
  static bool CallbackFunc(pb_istream_t* stream, const pb_field_t* field,
                           void** arg) {
    return reinterpret_cast<UnpackCallback*>(*arg)->CallbackFunc(stream, field);
  }

  wpi::SmallVector<T, N> m_storage;
  pb_callback_t m_callback;
  Limits m_limits{Limits::Ignore};
};

template <size_t N = 1>
struct StringUnpackCallback : UnpackCallback<UnpackStringType, N> {};

template <size_t N = 1>
struct BytesUnpackCallback : UnpackCallback<UnpackBytesType, N> {};

template <typename T>
class PackCallback {
 public:
  PackCallback(std::span<const T> buffer) : m_buffer{buffer} {
    m_callback.funcs.encode = CallbackFunc;
    m_callback.arg = this;
  }
  PackCallback(const T& element) : m_buffer{std::span<const T>{&element, 1}} {
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
  bool CallbackFunc(pb_ostream_t* stream, const pb_field_t* field) {
    const pb_msgdesc_t* desc = nullptr;
    if constexpr (!std::same_as<T, std::string_view> &&
                  !std::same_as<T, std::span<const uint8_t>>) {
      desc = wpi::Protobuf<T>::Message();
    }
    ProtoOutputStream ostream{stream, desc};
    for (auto&& i : m_buffer) {
      if (!pb_encode_tag_for_field(stream, field)) {
        return false;
      }
      bool success;
      if constexpr (std::same_as<T, std::string_view>) {
        success = pb_encode_string(
            stream, reinterpret_cast<const pb_byte_t*>(i.data()), i.size());
      } else if constexpr (std::same_as<T, std::span<const uint8_t>>) {
        success = pb_encode_string(
            stream, reinterpret_cast<const pb_byte_t*>(i.data()), i.size());
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
    return reinterpret_cast<PackCallback*>(*arg)->CallbackFunc(stream, field);
  }

  std::span<const T> m_buffer;
  pb_callback_t m_callback;
};

struct StringPackCallback : PackCallback<PackStringType> {
  StringPackCallback(std::span<const std::string_view> buffers)
      : PackCallback{buffers} {}
};

struct BytesPackCallback : PackCallback<PackBytesType> {};

}  // namespace wpi
