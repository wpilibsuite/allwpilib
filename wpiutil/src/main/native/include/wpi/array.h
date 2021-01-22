// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <array>
#include <cstddef>
#include <tuple>
#include <utility>

namespace wpi {

struct empty_array_t {};
constexpr empty_array_t empty_array;

/**
 * This class is a wrapper around std::array that does compile time size
 * checking.
 *
 * std::array's implicit constructor can lead result in uninitialized elements
 * if the number of arguments doesn't match the std::array size.
 */
template <typename T, size_t N>
class array : public std::array<T, N> {
 public:
  explicit array(empty_array_t) {}

  template <typename... Ts>
  array(T arg, Ts&&... args)  // NOLINT
      : std::array<T, N>{arg, std::forward<Ts>(args)...} {
    static_assert(1 + sizeof...(args) == N, "Dimension mismatch");
  }

  array(const array<T, N>&) = default;
  array& operator=(const array<T, N>&) = default;
  array(array<T, N>&&) = default;
  array& operator=(array<T, N>&&) = default;

  array(const std::array<T, N>& rhs) {  // NOLINT
    *static_cast<std::array<T, N>*>(this) = rhs;
  }

  array& operator=(const std::array<T, N>& rhs) {
    *static_cast<std::array<T, N>*>(this) = rhs;
    return *this;
  }

  array(std::array<T, N>&& rhs) {  // NOLINT
    *static_cast<std::array<T, N>*>(this) = rhs;
  }

  array& operator=(std::array<T, N>&& rhs) {
    *static_cast<std::array<T, N>*>(this) = rhs;
    return *this;
  }
};

template <typename T, typename... Ts>
array(T, Ts...) -> array<std::enable_if_t<(std::is_same_v<T, Ts> && ...), T>,
                         1 + sizeof...(Ts)>;

}  // namespace wpi

template <size_t I, typename T, size_t N>
constexpr T& get(wpi::array<T, N>& arr) noexcept {
  static_assert(I < N, "array index is within bounds");
  return std::get<I>(static_cast<std::array<T, N>>(arr));
}

template <size_t I, typename T, size_t N>
constexpr T&& get(wpi::array<T, N>&& arr) noexcept {
  static_assert(I < N, "array index is within bounds");
  return std::move(std::get<I>(arr));
}

template <size_t I, typename T, size_t N>
constexpr const T& get(const wpi::array<T, N>& arr) noexcept {
  static_assert(I < N, "array index is within bounds");
  return std::get<I>(static_cast<std::array<T, N>>(arr));
}

template <size_t I, typename T, size_t N>
constexpr const T&& get(const wpi::array<T, N>&& arr) noexcept {
  static_assert(I < N, "array index is within bounds");
  return std::move(std::get<I>(arr));
}

// Enables structured bindings
namespace std {  // NOLINT
// Partial specialization for wpi::array
template <typename T, size_t N>
struct tuple_size<wpi::array<T, N>> : public integral_constant<size_t, N> {};

// Partial specialization for wpi::array
template <size_t I, typename T, size_t N>
struct tuple_element<I, wpi::array<T, N>> {
  static_assert(I < N, "index is out of bounds");
  using type = T;
};
}  // namespace std
