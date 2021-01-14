// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <array>
#include <cstddef>
#include <utility>

namespace wpi {

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
  template <typename... Ts>
  array(Ts&&... args) : std::array<T, N>{std::forward<Ts>(args)...} {  // NOLINT
    static_assert(sizeof...(args) == N, "Dimension mismatch");
  }

  array(const array<T, N>&) = default;
  array& operator=(const array<T, N>&) = default;
  array(array<T, N>&&) = default;
  array& operator=(array<T, N>&&) = default;
};

}  // namespace wpi
