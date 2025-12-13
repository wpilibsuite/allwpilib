// Copyright (c) Sleipnir contributors

#pragma once

#ifdef JORMUNGANDR
#include <source_location>
#include <stdexcept>

#include <fmt/format.h>
/**
 * Throw an exception in Python.
 */
#define slp_assert(condition)                                        \
  do {                                                               \
    if (!(condition)) {                                              \
      auto location = std::source_location::current();               \
      throw std::invalid_argument(fmt::format(                       \
          "{}:{}: {}: Assertion `{}' failed.", location.file_name(), \
          location.line(), location.function_name(), #condition));   \
    }                                                                \
  } while (0);
#else
#include <cassert>
/**
 * Abort in C++.
 */
#define slp_assert(condition) assert(condition)
#endif
