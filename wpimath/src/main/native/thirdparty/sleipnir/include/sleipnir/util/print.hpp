// Copyright (c) Sleipnir contributors

#pragma once

#ifndef SLEIPNIR_DISABLE_DIAGNOSTICS
#include <cstdio>
#include <print>
#include <system_error>
#include <utility>
#endif

namespace slp {

#ifndef SLEIPNIR_DISABLE_DIAGNOSTICS

/// Wrapper around std::print() that squelches write failure exceptions.
template <typename... T>
void print(std::format_string<T...> fmt, T&&... args) {
  try {
    std::print(fmt, std::forward<T>(args)...);
  } catch (const std::system_error&) {
  }
}

/// Wrapper around std::print() that squelches write failure exceptions.
template <typename... T>
void print(std::FILE* f, std::format_string<T...> fmt, T&&... args) {
  try {
    std::print(f, fmt, std::forward<T>(args)...);
  } catch (const std::system_error&) {
  }
}

/// Wrapper around std::println() that squelches write failure exceptions.
template <typename... T>
void println(std::format_string<T...> fmt, T&&... args) {
  try {
    std::println(fmt, std::forward<T>(args)...);
  } catch (const std::system_error&) {
  }
}

/// Wrapper around std::println() that squelches write failure exceptions.
template <typename... T>
void println(std::FILE* f, std::format_string<T...> fmt, T&&... args) {
  try {
    std::println(f, fmt, std::forward<T>(args)...);
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
