// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/util/StackTrace.hpp"

#include <string>

#if defined(_MSC_VER)

namespace wpi::util {

std::string GetStackTraceDefault(int offset) {
  // FIXME: Use C++23 std::stacktrace
  return "";
}

}  // namespace wpi::util

#endif  // defined(_MSC_VER)
