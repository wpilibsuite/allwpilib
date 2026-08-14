// Copyright (c) Sleipnir contributors

#pragma once

#ifdef SLEIPNIR_PYTHON
#include <format>
#include <source_location>
#include <stdexcept>

/// Throws an exception in Python.
#define slp_assert(condition)                                        \
  do {                                                               \
    if (!(condition)) {                                              \
      auto location = std::source_location::current();               \
      throw std::invalid_argument(std::format(                       \
          "{}:{}: {}: Assertion `{}' failed.", location.file_name(), \
          location.line(), location.function_name(), #condition));   \
    }                                                                \
  } while (0)
#else
#include <cassert>

/// Aborts in C++.
#define slp_assert(condition) assert(condition)
#endif
