// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <stdint.h>

#include <concepts>
#include <cstdint>
#include <memory>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>

#include "wpi/tunable/CustomTunable.hpp"
#include "wpi/tunable/detail/TunableBase.hpp"
#include "wpi/tunable/detail/TunableDetail.hpp"
#include "wpi/tunable/detail/TunableMember.hpp"
#include "wpi/tunable/detail/TunableTypeTraits.hpp"
#include "wpi/tunable/detail/TunableTypeValue.hpp"
#include "wpi/util/protobuf/Protobuf.hpp"
#include "wpi/util/struct/Struct.hpp"

namespace wpi {

struct TunableConfig;

/**
 * A tunable value.
 *
 * @tparam T value class
 * @tparam I optional info parameters (for custom or struct-serialized types)
 */
template <typename T, typename... I>
class Tunable;

template <typename T, typename... I>
  requires detail::TunableValueType<T>
class Tunable<T, I...> : public detail::TunableValueBase<T> {
 public:
  template <typename... Args>
  constexpr explicit Tunable(Args&&... args)
      : detail::TunableValueBase<T>{std::forward<Args>(args)...} {}

  template <typename U>
    requires(!std::same_as<std::remove_cvref_t<U>, Tunable<T, I...>>)
  constexpr Tunable<T, I...>& operator=(U&& value) {
    detail::TunableValueBase<T>::Set(value);
    return *this;
  }

  constexpr operator decltype(  // NOLINT(google-explicit-constructor)
      std::declval<detail::TunableValueBase<T>>().Get())() const {
    return detail::TunableValueBase<T>::Get();
  }
};

template <typename T, typename... I>
  requires(!detail::TunableValueType<T> && detail::CustomTunableType<T, I...>)
class Tunable<T, I...> : public CustomTunable<T, I...> {
 public:
  template <typename... Args>
  constexpr explicit Tunable(I&&... info, Args&&... args)
      : CustomTunable<T, I...>{std::forward<I>(info)...,
                               std::forward<Args>(args)...} {}

  template <typename... Args>
  constexpr explicit Tunable(const TunableConfig& config, I&&... info,
                             Args&&... args)
      : CustomTunable<T, I...>{config, std::forward<I>(info)...,
                               std::forward<Args>(args)...} {}

  template <typename U>
    requires(!std::same_as<std::remove_cvref_t<U>, Tunable<T, I...>>)
  constexpr Tunable<T, I...>& operator=(U&& value) {
    CustomTunable<T>::Set(value);
    return *this;
  }

  constexpr
  operator std::remove_cvref_t<decltype(  // NOLINT(google-explicit-constructor)
      std::declval<CustomTunable<T>>().Get())>() const {
    return CustomTunable<T>::Get();
  }
};

template <typename T, typename... I>
  requires(!detail::TunableValueType<T> && !detail::CustomTunableType<T> &&
           detail::CustomTunableADL<T>)
class Tunable<T, I...> : public decltype(GetCustomTunable(std::declval<T>())) {
  using Base = decltype(GetCustomTunable(std::declval<T>()));

 public:
  template <typename... Args>
  constexpr explicit Tunable(Args && ... args)
      : Base{std::forward<Args>(args)...} {}

  template <typename U>
    requires(!std::same_as<std::remove_cvref_t<U>, Tunable<T, I...>>)
  constexpr Tunable<T, I...>& operator=(U&& value) {
    Base::Set(value);
    return *this;
  }

  constexpr
  operator std::remove_cvref_t<decltype(  // NOLINT(google-explicit-constructor)
      std::declval<Base>().Get())>() const {
    return Base::Get();
  }
};

template <typename T, typename... I>
  requires(!detail::TunableValueType<T> && !detail::CustomTunableType<T> &&
           !detail::CustomTunableADL<T> &&
           wpi::util::StructSerializable<T, I...>)
class Tunable<T, I...> : public detail::TunableStruct<T, I...> {
 public:
  template <typename... Args>
    requires detail::FirstArgNotTunableConfig<Args...>
  constexpr explicit Tunable(I&&... info, Args&&... args)
      : detail::TunableStruct<T, I...>{std::forward<I>(info)...,
                                       std::forward<Args>(args)...} {}

  template <typename... Args>
  constexpr explicit Tunable(const TunableConfig& config, I&&... info,
                             Args&&... args)
      : detail::TunableStruct<T, I...>{config, std::forward<I>(info)...,
                                       std::forward<Args>(args)...} {}

  using detail::TunableStruct<T, I...>::operator=;
  using detail::TunableStruct<T, I...>::operator const T&;
};

template <typename T, typename... I>
  requires(!detail::TunableValueType<std::vector<T>> &&
           !detail::CustomTunableType<std::vector<T>> &&
           !detail::CustomTunableADL<std::vector<T>> &&
           wpi::util::StructSerializable<T, I...>)
class Tunable<std::vector<T>, I...>
    : public detail::TunableStructVector<T, I...> {
 public:
  template <typename... Args>
    requires detail::FirstArgNotTunableConfig<Args...>
  constexpr explicit Tunable(I&&... info, Args&&... args)
      : detail::TunableStructVector<T, I...>{std::forward<I>(info)...,
                                             std::forward<Args>(args)...} {}

  template <typename... Args>
  constexpr explicit Tunable(const TunableConfig& config, I&&... info,
                             Args&&... args)
      : detail::TunableStructVector<T, I...>{config, std::forward<I>(info)...,
                                             std::forward<Args>(args)...} {}

  using detail::TunableStructVector<T, I...>::operator=;
  using detail::TunableStructVector<T, I...>::operator const std::vector<T>&;
};

template <typename T, typename... I>
  requires(!detail::TunableValueType<T> && !detail::CustomTunableType<T> &&
           !detail::CustomTunableADL<T> &&
           !wpi::util::StructSerializable<T, I...> &&
           wpi::util::ProtobufSerializable<T>)
class Tunable<T, I...> : public detail::TunableProtobuf<T> {
 public:
  template <typename... Args>
    requires detail::FirstArgNotTunableConfig<Args...>
  constexpr explicit Tunable(Args&&... args)
      : detail::TunableProtobuf<T>{std::forward<Args>(args)...} {}

  template <typename... Args>
  constexpr explicit Tunable(const TunableConfig& config, Args&&... args)
      : detail::TunableProtobuf<T>{config, std::forward<Args>(args)...} {}

  using detail::TunableProtobuf<T>::operator=;
  using detail::TunableProtobuf<T>::operator const T&;
};

using TunableBool = Tunable<bool>;
using TunableInt32 = Tunable<int32_t>;
using TunableInt64 = Tunable<int64_t>;
using TunableFloat = Tunable<float>;
using TunableDouble = Tunable<double>;
using TunableString = Tunable<std::string>;
using TunableRaw = Tunable<std::vector<uint8_t>>;
using TunableBoolVector = Tunable<std::vector<bool>>;
using TunableInt32Vector = Tunable<std::vector<int32_t>>;
using TunableInt64Vector = Tunable<std::vector<int64_t>>;
using TunableFloatVector = Tunable<std::vector<float>>;
using TunableDoubleVector = Tunable<std::vector<double>>;
using TunableStringVector = Tunable<std::vector<std::string>>;

template <typename T, typename Class, typename... Args>
  requires detail::TunableValueType<T>
std::unique_ptr<detail::TunableMemberBase> MakeTunableMember(T Class::*member,
                                                             Args&&... args) {
  return std::make_unique<detail::TunableMemberValue<T>>(
      member, std::forward<Args>(args)...);
}

template <typename T, typename Class, typename... I>
  requires(!detail::TunableValueType<T> &&
           wpi::util::StructSerializable<T, I...>)
std::unique_ptr<detail::TunableMemberBase> MakeTunableMember(T Class::*member,
                                                             I&&... info) {
  return std::make_unique<detail::TunableMemberStruct<T, I...>>(
      member, std::forward<I>(info)...);
}

template <typename T, typename Class, typename... I>
  requires(!detail::TunableValueType<T> &&
           wpi::util::StructSerializable<T, I...>)
std::unique_ptr<detail::TunableMemberBase> MakeTunableMember(
    T Class::*member, const TunableConfig& config, I&&... info) {
  return std::make_unique<detail::TunableMemberStruct<T, I...>>(
      member, config, std::forward<I>(info)...);
}

template <typename T, typename Class, typename... I>
  requires(!detail::TunableValueType<std::vector<T>> &&
           wpi::util::StructSerializable<T, I...>)
std::unique_ptr<detail::TunableMemberBase> MakeTunableMember(
    std::vector<T> Class::*member, I&&... info) {
  return std::make_unique<detail::TunableMemberStructVector<T, I...>>(
      member, std::forward<I>(info)...);
}

template <typename T, typename Class, typename... I>
  requires(!detail::TunableValueType<std::vector<T>> &&
           wpi::util::StructSerializable<T, I...>)
std::unique_ptr<detail::TunableMemberBase> MakeTunableMember(
    std::vector<T> Class::*member, const TunableConfig& config, I&&... info) {
  return std::make_unique<detail::TunableMemberStructVector<T, I...>>(
      member, config, std::forward<I>(info)...);
}

template <typename T, typename Class, typename... Args>
  requires(!detail::TunableValueType<T> && !wpi::util::StructSerializable<T> &&
           wpi::util::ProtobufSerializable<T>)
std::unique_ptr<detail::TunableMemberBase> MakeTunableMember(T Class::*member,
                                                             Args&&... args) {
  return std::make_unique<detail::TunableMemberProtobuf<T>>(
      member, std::forward<Args>(args)...);
}

}  // namespace wpi
