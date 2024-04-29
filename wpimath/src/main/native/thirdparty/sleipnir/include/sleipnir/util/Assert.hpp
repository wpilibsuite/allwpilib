// Copyright (c) Sleipnir contributors

#pragma once

#ifdef JORMUNGANDR
#include <stdexcept>

#include <fmt/format.h>
/**
 * Throw an exception in Python.
 */
#define Assert(condition)                                                      \
  do {                                                                         \
    if (!(condition)) {                                                        \
      throw std::invalid_argument(                                             \
          fmt::format("{}:{}: {}: Assertion `{}' failed.", __FILE__, __LINE__, \
                      __func__, #condition));                                  \
    }                                                                          \
  } while (0);
#else
#include <cassert>
/**
 * Abort in C++.
 */
#define Assert(condition) assert(condition)
#endif
