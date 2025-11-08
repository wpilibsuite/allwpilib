// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <cassert>
#include <span>

namespace wpi {

/// Drop the first \p N elements of the array.
template <typename T, size_t N>
constexpr std::span<T> drop_front(std::span<T, N> in,
                                  typename std::span<T>::size_type n = 1) {
  assert(in.size() >= n && "Dropping more elements than exist");
  return in.subspan(n, in.size() - n);
}

/// Drop the last \p N elements of the array.
template <typename T, size_t N>
constexpr std::span<T> drop_back(std::span<T, N> in,
                                 typename std::span<T>::size_type n = 1) {
  assert(in.size() >= n && "Dropping more elements than exist");
  return in.subspan(0, in.size() - n);
}

/**
 * Returns a span equal to @p in but with only the first @p n
 * elements remaining.  If @p n is greater than the length of the
 * span, the entire span is returned.
 */
template <typename T, size_t N>
constexpr std::span<T> take_front(std::span<T, N> in,
                                  typename std::span<T>::size_type n = 1) {
  auto length = in.size();
  if (n >= length) {
    return in;
  }
  return drop_back(in, length - n);
}

/**
 * Returns a span equal to @p in but with only the last @p n
 * elements remaining.  If @p n is greater than the length of the
 * span, the entire span is returned.
 */
template <typename T, size_t N>
constexpr std::span<T> take_back(std::span<T, N> in,
                                 typename std::span<T>::size_type n = 1) {
  auto length = in.size();
  if (n >= length) {
    return in;
  }
  return drop_front(in, length - n);
}

}  // namespace wpi
