// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#ifndef WPIUTIL_WPI_STACKTRACE_H_
#define WPIUTIL_WPI_STACKTRACE_H_

#include <string>

namespace wpi {

/**
 * Get a stack trace, ignoring the first "offset" symbols.
 *
 * @param offset The number of symbols at the top of the stack to ignore
 */
std::string GetStackTrace(int offset);

}  // namespace wpi

#endif  // WPIUTIL_WPI_STACKTRACE_H_
