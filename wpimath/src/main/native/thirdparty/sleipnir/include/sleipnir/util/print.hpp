// Copyright (c) Sleipnir contributors

#pragma once

#ifndef SLEIPNIR_DISABLE_DIAGNOSTICS
#include <cstdio>
#include <system_error>
#include <utility>

#include <fmt/base.h>
#endif

namespace slp {

#ifndef SLEIPNIR_DISABLE_DIAGNOSTICS

/// Wrapper around fmt::print() that squelches write failure exceptions.
template <typename... T>
void print(fmt::format_string<T...> fmt, T&&... args) {
  try {
    fmt::print(fmt, std::forward<T>(args)...);
  } catch (const std::system_error&) {
  }
}

/// Wrapper around fmt::print() that squelches write failure exceptions.
template <typename... T>
void print(std::FILE* f, fmt::format_string<T...> fmt, T&&... args) {
  try {
    fmt::print(f, fmt, std::forward<T>(args)...);
  } catch (const std::system_error&) {
  }
}

/// Wrapper around fmt::println() that squelches write failure exceptions.
template <typename... T>
void println(fmt::format_string<T...> fmt, T&&... args) {
  try {
    fmt::println(fmt, std::forward<T>(args)...);
  } catch (const std::system_error&) {
  }
}

/// Wrapper around fmt::println() that squelches write failure exceptions.
template <typename... T>
void println(std::FILE* f, fmt::format_string<T...> fmt, T&&... args) {
  try {
    fmt::println(f, fmt, std::forward<T>(args)...);
  } catch (const std::system_error&) {
  }
}

#else

template <typename... Args>
void print([[maybe_unused]] Args&&... args) {}

template <typename... Args>
void println([[maybe_unused]] Args&&... args) {}

#endif

}  // namespace slp
