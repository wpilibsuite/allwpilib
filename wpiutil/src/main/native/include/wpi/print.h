// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <cstdio>
#include <system_error>
#include <utility>

#if __has_include(<fmt/base.h>)
#include <fmt/base.h>
#else
#include <fmt/core.h>
#endif

namespace wpi {

/**
 * Wrapper around fmt::print() that squelches write failure exceptions.
 */
template <typename... T>
inline void print(fmt::format_string<T...> fmt, T&&... args) {
  try {
    fmt::print(fmt, std::forward<T>(args)...);
  } catch (const std::system_error&) {
  }
}

/**
 * Wrapper around fmt::print() that squelches write failure exceptions.
 */
template <typename... T>
inline void print(std::FILE* f, fmt::format_string<T...> fmt, T&&... args) {
  try {
    fmt::print(f, fmt, std::forward<T>(args)...);
  } catch (const std::system_error&) {
  }
}

/**
 * Wrapper around fmt::println() that squelches write failure exceptions.
 */
template <typename... T>
inline void println(fmt::format_string<T...> fmt, T&&... args) {
  try {
    fmt::println(fmt, std::forward<T>(args)...);
  } catch (const std::system_error&) {
  }
}

/**
 * Wrapper around fmt::println() that squelches write failure exceptions.
 */
template <typename... T>
inline void println(std::FILE* f, fmt::format_string<T...> fmt, T&&... args) {
  try {
    fmt::println(f, fmt, std::forward<T>(args)...);
  } catch (const std::system_error&) {
  }
}

}  // namespace wpi
