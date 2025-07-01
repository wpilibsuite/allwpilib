// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <concepts>
#include <type_traits>

namespace wpi {

template <typename T, typename... I>
struct CustomTunable {};

namespace detail {

class TunableBase;

template <typename T>
concept IsTunableBaseReference =
    std::is_reference_v<T> &&
    std::derived_from<std::remove_cvref_t<T>, TunableBase>;

template <typename T, typename... I>
concept CustomTunableType =
    requires(CustomTunable<std::remove_cvref_t<T>, I...> value) {
      typename CustomTunable<std::remove_cvref_t<T>, I...>;
      { value.GetInnerTunable() } -> IsTunableBaseReference;
      { value.Get() } -> std::convertible_to<std::remove_cvref_t<T>>;
      { value.Set(std::declval<std::remove_cvref_t<T>>()) };
    };

template <typename T, typename ValueType>
concept CustomTunableADLType = requires(T tunable) {
  { tunable.GetInnerTunable() } -> IsTunableBaseReference;
  { tunable.Get() } -> std::convertible_to<ValueType>;
  { tunable.Set(std::declval<ValueType>()) };
};

template <typename T>
concept CustomTunableADL = requires(T value) {
  { GetCustomTunable(value) } -> CustomTunableADLType<std::remove_cvref_t<T>>;
};

template <typename T, typename... I>
concept IsCustomTunable = CustomTunableType<T, I...> || CustomTunableADL<T>;

}  // namespace detail
}  // namespace wpi
