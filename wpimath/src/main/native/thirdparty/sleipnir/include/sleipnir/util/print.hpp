// Copyright (c) Sleipnir contributors

#pragma once

#ifndef SLEIPNIR_DISABLE_DIAGNOSTICS
#include <cstdio>
#include <system_error>
#include <utility>

#if __has_include(<fmt/base.h>)
#include <fmt/base.h>
#else
#include <fmt/core.h>
#endif

#endif

namespace slp {

#ifndef SLEIPNIR_DISABLE_DIAGNOSTICS

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

#else

template <typename... Args>
inline void print([[maybe_unused]] Args&&... args) {}

template <typename... Args>
inline void println([[maybe_unused]] Args&&... args) {}

#endif

}  // namespace slp
