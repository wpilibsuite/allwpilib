// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <stdint.h>

#include <concepts>
#include <cstdint>
#include <string>
#include <string_view>
#include <tuple>
#include <type_traits>
#include <utility>
#include <vector>

#include "wpi/tunable/CustomTunable.hpp"
#include "wpi/tunable/detail/TunableBase.hpp"
#include "wpi/tunable/detail/TunableMember.hpp"
#include "wpi/tunable/detail/TunableTypeTraits.hpp"
#include "wpi/tunable/detail/TunableTypeValue.hpp"
#include "wpi/util/protobuf/Protobuf.hpp"
#include "wpi/util/struct/Struct.hpp"

namespace wpi {
struct TunableConfig;
}  // namespace wpi

namespace wpi::detail {

template <typename... Args>
struct FirstArgIsTunableConfig : std::false_type {};

template <typename First, typename... Rest>
struct FirstArgIsTunableConfig<First, Rest...>
    : std::bool_constant<
          std::same_as<std::remove_cvref_t<First>, wpi::TunableConfig>> {};

template <typename... Args>
concept FirstArgNotTunableConfig = !FirstArgIsTunableConfig<Args...>::value;

template <TunableValueType T>
class TunableValueBase : public detail::TunableBase {
 public:
  constexpr TunableValueBase()
      : TunableBase{GetTunableTypeValue<T>()}, m_value{} {}
  explicit TunableValueBase(const TunableConfig& config)
      : TunableBase{GetTunableTypeValue<T>(), config}, m_value{} {}
  constexpr explicit TunableValueBase(T value)
      : TunableBase{GetTunableTypeValue<T>()}, m_value{std::move(value)} {}
  TunableValueBase(T value, const TunableConfig& config)
      : TunableBase{GetTunableTypeValue<T>(), config},
        m_value{std::move(value)} {}

  constexpr const T& Get() const { return m_value; }

  void Set(T value) {
    m_value = std::move(value);
    SetTunableChanged();
  }

  constexpr T& Mutate() { return m_value; }

 private:
  T m_value;
};

template <>
class TunableValueBase<std::string> : public detail::TunableBase {
 public:
  constexpr TunableValueBase()
      : TunableBase{TunableTypeValue::STRING}, m_value{} {}
  explicit TunableValueBase(const TunableConfig& config)
      : TunableBase{TunableTypeValue::STRING, config}, m_value{} {}
  constexpr explicit TunableValueBase(std::string value)
      : TunableBase{TunableTypeValue::STRING}, m_value{std::move(value)} {}
  TunableValueBase(const char* value, const TunableConfig& config)
      : TunableBase{TunableTypeValue::STRING, config}, m_value{value} {}
  TunableValueBase(std::string&& value, const TunableConfig& config)
      : TunableBase{TunableTypeValue::STRING, config},
        m_value{std::move(value)} {}
  TunableValueBase(std::string_view value, const TunableConfig& config)
      : TunableBase{TunableTypeValue::STRING, config}, m_value{value} {}

  constexpr const std::string& Get() const { return m_value; }

  void Set(const char* value) {
    m_value = value;
    SetTunableChanged();
  }
  void Set(std::string&& value) {
    m_value = std::move(value);
    SetTunableChanged();
  }
  void Set(std::string_view value) {
    m_value = value;
    SetTunableChanged();
  }

  constexpr std::string& Mutate() { return m_value; }

 private:
  std::string m_value;
};

template <typename U>
class TunableValueBase<std::vector<U>> : public detail::TunableBase {
 public:
  constexpr TunableValueBase()
      : TunableBase{GetTunableTypeValue<std::vector<U>>()}, m_value{} {}
  explicit TunableValueBase(const TunableConfig& config)
      : TunableBase{GetTunableTypeValue<std::vector<U>>(), config}, m_value{} {}
  constexpr explicit TunableValueBase(std::vector<U> value)
      : TunableBase{GetTunableTypeValue<std::vector<U>>()},
        m_value{std::move(value)} {}
  TunableValueBase(std::vector<U>&& value, const TunableConfig& config)
      : TunableBase{GetTunableTypeValue<std::vector<U>>(), config},
        m_value{std::move(value)} {}
  TunableValueBase(std::span<const U> value, const TunableConfig& config)
      : TunableBase{GetTunableTypeValue<std::vector<U>>(), config},
        m_value{value.begin(), value.end()} {}

  constexpr const std::vector<U>& Get() const { return m_value; }

  void Set(std::vector<U>&& value) {
    m_value = std::move(value);
    SetTunableChanged();
  }
  void Set(std::span<const U> value) {
    m_value.assign(value.begin(), value.end());
    SetTunableChanged();
  }

  constexpr std::vector<U>& Mutate() { return m_value; }

 private:
  std::vector<U> m_value;
};

class TunableStructBase : public TunableBase {
 public:
  constexpr TunableStructBase() : TunableBase{TunableTypeValue::STRUCT} {}
  explicit TunableStructBase(const TunableConfig& config)
      : TunableBase{TunableTypeValue::STRUCT, config} {}

  TunableStructBase(const TunableStructBase&) = default;
  TunableStructBase& operator=(const TunableStructBase&) = default;
  TunableStructBase(TunableStructBase&&) = default;
  TunableStructBase& operator=(TunableStructBase&&) = default;

  virtual std::string GetStructTypeName() const = 0;
  // note: size may be dynamic due to struct arrays
  virtual size_t GetStructSize() const = 0;
  virtual void ForEachStructSchema(
      wpi::util::function_ref<void(std::string_view typeName,
                                   std::string_view schema)>
          fn) const = 0;
  virtual void UnpackStruct(std::span<const uint8_t> data) = 0;
  virtual void PackStruct(std::span<uint8_t> buf) const = 0;

 protected:
  virtual ~TunableStructBase() = default;
};

class TunableProtobufBase : public TunableBase {
 public:
  constexpr TunableProtobufBase() : TunableBase{TunableTypeValue::PROTOBUF} {}
  explicit TunableProtobufBase(const TunableConfig& config)
      : TunableBase{TunableTypeValue::PROTOBUF, config} {}

  TunableProtobufBase(const TunableProtobufBase&) = default;
  TunableProtobufBase& operator=(const TunableProtobufBase&) = default;
  TunableProtobufBase(TunableProtobufBase&&) = default;
  TunableProtobufBase& operator=(TunableProtobufBase&&) = default;

  virtual std::string GetProtobufTypeString() const = 0;
  virtual bool UnpackProtobuf(std::span<const uint8_t> data) = 0;
  virtual bool PackProtobuf(std::vector<uint8_t>& buf) const = 0;
  virtual void ForEachProtobufDescriptor(
      util::function_ref<bool(std::string_view filename)> exists,
      util::function_ref<void(std::string_view filename,
                              std::string_view descriptor)>
          fn) const = 0;

 protected:
  virtual ~TunableProtobufBase() = default;
};

/**
 * A tunable struct-serialized value.
 *
 * @tparam T value class
 * @tparam I optional struct type info parameters
 */
template <typename T, typename... I>
  requires wpi::util::StructSerializable<T, I...>
class TunableStruct : public detail::TunableStructBase {
 public:
  template <typename... Args>
    requires FirstArgNotTunableConfig<Args...>
  constexpr explicit TunableStruct(I... info, Args&&... args)
      : m_value{std::forward<Args>(args)...}, m_info{std::move(info)...} {}

  template <typename... Args>
  explicit TunableStruct(const TunableConfig& config, I... info, Args&&... args)
      : detail::TunableStructBase{config},
        m_value{std::forward<Args>(args)...},
        m_info{std::move(info)...} {}

  constexpr operator const T&() const {  // NOLINT(google-explicit-constructor)
    return m_value;
  }

  TunableStruct& operator=(T value) {
    m_value = std::move(value);
    SetTunableChanged();
    return *this;
  }

  constexpr const T& Get() const { return m_value; }

  void Set(T value) {
    m_value = std::move(value);
    SetTunableChanged();
  }

  constexpr T& Mutate() { return m_value; }

 protected:
  std::string GetStructTypeName() const override {
    return std::apply(
        [](const I&... info) {
          return std::string{wpi::util::GetStructTypeName<T>(info...)};
        },
        m_info);
  }

  size_t GetStructSize() const override {
    return std::apply(
        [](const I&... info) { return wpi::util::GetStructSize<T>(info...); },
        m_info);
  }

  void ForEachStructSchema(
      wpi::util::function_ref<void(std::string_view typeName,
                                   std::string_view schema)>
          fn) const override {
    std::apply(
        [&](const I&... info) {
          wpi::util::ForEachStructSchema<T>(fn, info...);
        },
        m_info);
  }

  void UnpackStruct(std::span<const uint8_t> data) override {
    std::apply(
        [&](const I&... info) {
          wpi::util::UnpackStructInto(&m_value, data, info...);
        },
        m_info);
  }

  void PackStruct(std::span<uint8_t> buf) const override {
    std::apply(
        [&](const I&... info) { wpi::util::PackStruct(buf, m_value, info...); },
        m_info);
  }

 private:
  T m_value;
  [[no_unique_address]]
  std::tuple<I...> m_info;
};

template <typename T, typename... I>
  requires wpi::util::StructSerializable<T, I...>
class TunableStructVector : public detail::TunableStructBase {
 public:
  template <typename... Args>
    requires FirstArgNotTunableConfig<Args...>
  constexpr explicit TunableStructVector(I... info, Args&&... args)
      : m_value{std::forward<Args>(args)...}, m_info{std::move(info)...} {}

  template <typename... Args>
  explicit TunableStructVector(const TunableConfig& config, I... info,
                               Args&&... args)
      : detail::TunableStructBase{config},
        m_value{std::forward<Args>(args)...},
        m_info{std::move(info)...} {}

  constexpr
  operator const std::vector<T>&()  // NOLINT(google-explicit-constructor)
      const {
    return m_value;
  }

  TunableStructVector& operator=(std::vector<T> value) {
    m_value = std::move(value);
    SetTunableChanged();
    return *this;
  }

  TunableStructVector& operator=(std::span<const T> value) {
    m_value.assign(value.begin(), value.end());
    SetTunableChanged();
    return *this;
  }

  constexpr const std::vector<T>& Get() const { return m_value; }

  void Set(std::vector<T> value) {
    m_value = std::move(value);
    SetTunableChanged();
  }
  void Set(std::span<const T> value) {
    m_value.assign(value.begin(), value.end());
    SetTunableChanged();
  }

  constexpr std::vector<T>& Mutate() { return m_value; }

 protected:
  std::string GetStructTypeName() const override {
    return std::apply(
        [](const I&... info) {
          return std::string{
              wpi::util::MakeStructArrayTypeString<T, std::dynamic_extent>(
                  info...)};
        },
        m_info);
  }

  size_t GetStructSize() const override {
    return m_value.size() * std::apply(
                                [](const I&... info) {
                                  return wpi::util::GetStructSize<T>(info...);
                                },
                                m_info);
  }

  void ForEachStructSchema(
      wpi::util::function_ref<void(std::string_view typeName,
                                   std::string_view schema)>
          fn) const override {
    std::apply(
        [&](const I&... info) {
          wpi::util::ForEachStructSchema<T>(fn, info...);
        },
        m_info);
  }

  void UnpackStruct(std::span<const uint8_t> data) override {
    std::apply(
        [&](const I&... info) {
          size_t size = wpi::util::GetStructSize<T>(info...);
          m_value.clear();
          m_value.reserve(data.size() / size);
          for (size_t pos = 0, end = data.size(); pos < end; pos += size) {
            m_value.emplace_back(
                wpi::util::UnpackStruct<T>(data.subspan(pos, size), info...));
          }
        },
        m_info);
  }

  void PackStruct(std::span<uint8_t> buf) const override {
    std::apply(
        [&](const I&... info) {
          size_t size = wpi::util::GetStructSize<T>(info...);
          for (auto&& val : m_value) {
            if (buf.size() < size) {
              break;  // shouldn't happen, but avoid clobbering memory
            }
            wpi::util::PackStruct(buf, val, info...);
            buf = buf.subspan(size);
          }
        },
        m_info);
  }

 private:
  std::vector<T> m_value;
  [[no_unique_address]]
  std::tuple<I...> m_info;
};

/**
 * A tunable protobuf-serialized value.
 *
 * @tparam T value class
 */
template <wpi::util::ProtobufSerializable T>
class TunableProtobuf : public detail::TunableProtobufBase {
 public:
  template <typename... Args>
    requires FirstArgNotTunableConfig<Args...>
  constexpr explicit TunableProtobuf(Args&&... args)
      : m_value{std::forward<Args>(args)...} {}

  template <typename... Args>
  explicit TunableProtobuf(const TunableConfig& config, Args&&... args)
      : TunableProtobufBase{config}, m_value{std::forward<Args>(args)...} {}

  constexpr operator const T&() const {  // NOLINT(google-explicit-constructor)
    return m_value;
  }

  TunableProtobuf& operator=(T value) {
    m_value = std::move(value);
    SetTunableChanged();
    return *this;
  }

  constexpr const T& Get() const { return m_value; }

  void Set(T value) {
    m_value = std::move(value);
    SetTunableChanged();
  }

  constexpr T& Mutate() { return m_value; }

 protected:
  std::string GetProtobufTypeString() const override {
    return m_message.GetTypeString();
  }

  bool UnpackProtobuf(std::span<const uint8_t> data) override {
    return m_message.UnpackInto(&m_value, data);
  }

  bool PackProtobuf(std::vector<uint8_t>& buf) const override {
    return m_message.Pack(buf, m_value);
  }

  void ForEachProtobufDescriptor(
      wpi::util::function_ref<bool(std::string_view filename)> exists,
      wpi::util::function_ref<void(std::string_view filename,
                                   std::string_view descriptor)>
          fn) const override {
    m_message.ForEachProtobufDescriptor(
        exists,
        [&](std::string_view filename, std::span<const uint8_t> descriptor) {
          auto data = reinterpret_cast<const char*>(descriptor.data());
          fn(filename, {data, descriptor.size()});
        });
  }

 private:
  T m_value;
  [[no_unique_address]]
  mutable wpi::util::ProtobufMessage<T> m_message;
};

template <typename T, bool Member>
inline auto CastTunable(TunableBase* tunable, TunableTypeValue type) {
  if constexpr (std::same_as<T, TunableStructTag>) {
    if constexpr (Member) {
      return static_cast<TunableMemberStructBase*>(
          type == TunableTypeValue::MEMBER_STRUCT ? tunable : nullptr);
    } else {
      return static_cast<TunableStructBase*>(
          type == TunableTypeValue::STRUCT ? tunable : nullptr);
    }
  } else if constexpr (std::same_as<T, TunableProtobufTag>) {
    if constexpr (Member) {
      return static_cast<TunableMemberProtobufBase*>(
          type == TunableTypeValue::MEMBER_PROTOBUF ? tunable : nullptr);
    } else {
      return static_cast<TunableProtobufBase*>(
          type == TunableTypeValue::PROTOBUF ? tunable : nullptr);
    }
  } else {
    if constexpr (Member) {
      return static_cast<TunableMemberValueBase<T>*>(
          type == GetTunableTypeValue<T, Member>() ? tunable : nullptr);
    } else {
      return static_cast<TunableValueBase<T>*>(
          type == GetTunableTypeValue<T, Member>() ? tunable : nullptr);
    }
  }
}

}  // namespace wpi::detail
