// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <concepts>
#include <functional>
#include <type_traits>
#include <utility>

#if defined(__APPLE__) && !defined(__cpp_lib_concepts)

namespace std {

template <typename T, typename... Args>
concept constructible_from =
    is_nothrow_destructible_v<T> && is_constructible_v<T, Args...>;

template <typename From, typename To>
concept convertible_to = is_convertible_v<From, To> &&
                         requires { static_cast<To>(declval<From>()); };

template <typename T>
concept move_constructible = constructible_from<T, T> && convertible_to<T, T>;

template <typename T>
concept copy_constructible =
    move_constructible<T> && constructible_from<T, T&> &&
    convertible_to<T&, T> && constructible_from<T, const T&> &&
    convertible_to<const T&, T> && constructible_from<T, const T> &&
    convertible_to<const T, T>;

template <typename T>
concept default_initializable =
    constructible_from<T> && requires { T{}; } && requires { ::new T; };

template <typename Derived, typename Base>
concept derived_from =
    is_base_of_v<Base, Derived> &&
    is_convertible_v<const volatile Derived*, const volatile Base*>;

template <typename T>
concept floating_point = is_floating_point_v<T>;

template <typename T>
concept integral = is_integral_v<T>;

template <typename F, typename... Args>
concept invocable = requires(F&& f, Args&&... args) {
  invoke(forward<F>(f), forward<Args>(args)...);
};  // NOLINT

}  // namespace std

#endif  // defined(__APPLE__)
