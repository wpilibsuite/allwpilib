// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <cstdint>
#include <initializer_list>
#include <span>
#include <type_traits>
#include <vector>

#include <catch2/matchers/catch_matchers_range_equals.hpp>

namespace wpi::util {

template <typename T>
inline auto SpanEq(const T& good) {
  return Catch::Matchers::RangeEquals(
      std::vector<std::remove_cv_t<typename T::value_type>>{good.begin(),
                                                            good.end()});
}

template <typename T>
inline auto SpanEq(std::initializer_list<const T> good) {
  return Catch::Matchers::RangeEquals(
      std::vector<std::remove_cv_t<T>>{good.begin(), good.end()});
}

}  // namespace wpi::util

inline std::span<const uint8_t> operator""_us(const char* str, size_t len) {
  return {reinterpret_cast<const uint8_t*>(str), len};
}
