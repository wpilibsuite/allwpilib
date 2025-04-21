// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/StackTrace.h"

#include <string>

#if defined(_MSC_VER)

namespace wpi {

std::string GetStackTraceDefault(int offset) {
  // FIXME: Use C++23 std::stacktrace
  return "";
}

}  // namespace wpi

#endif  // defined(_MSC_VER)
