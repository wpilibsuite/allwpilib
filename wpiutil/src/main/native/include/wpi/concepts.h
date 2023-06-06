// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <concepts>

#if defined(__APPLE__) && !defined(__cpp_lib_concepts)

#include <functional>
#include <type_traits>
#include <utility>

namespace std {

template <typename T, typename U>
concept same_as = is_same_v<T, U> && is_same_v<U, T>;

template <typename T, typename... Args>
concept constructible_from =
    is_nothrow_destructible_v<T> && is_constructible_v<T, Args...>;

template <typename From, typename To>
concept convertible_to = is_convertible_v<From, To> &&
                         requires { static_cast<To>(declval<From>()); };

template <typename T, typename U>
concept common_reference_with =
    same_as<common_reference_t<T, U>, common_reference_t<U, T>> &&
    convertible_to<T, common_reference_t<T, U>> &&
    convertible_to<U, common_reference_t<T, U>>;

template <typename LHS, typename RHS>
concept assignable_from =
    is_lvalue_reference_v<LHS> &&
    common_reference_with<const remove_reference_t<LHS>&,
                          const remove_reference_t<RHS>&> &&
    requires(LHS lhs, RHS&& rhs) {
      { lhs = std::forward<RHS>(rhs) } -> same_as<LHS>
    };

template <typename T>
concept swappable = requires(T& a, T& b) { ranges::swap(a, b); };

template <typename T>
concept move_constructible = constructible_from<T, T> && convertible_to<T, T>;

template <typename T>
concept copy_constructible =
    move_constructible<T> && constructible_from<T, T&> &&
    convertible_to<T&, T> && constructible_from<T, const T&> &&
    convertible_to<const T&, T> && constructible_from<T, const T> &&
    convertible_to<const T, T>;

template <typename T>
concept movable = is_object_v<T> && move_constructible<T> &&
                  assignable_from<T&, T> && swappable<T>;

template <typename T>
concept copyable =
    copy_constructible<T> && movable<T> && assignable_from<T&, T&> &&
    assignable_from<T&, const T&> && asignable_from<T&, const T>;

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

#endif  // defined(__APPLE__) && !defined(__cpp_lib_concepts)
