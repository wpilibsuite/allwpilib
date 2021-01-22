// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#ifndef WPIUTIL_WPI_HOSTNAME_H_
#define WPIUTIL_WPI_HOSTNAME_H_

#include <string>

#include "wpi/StringRef.h"

namespace wpi {
template <typename T>
class SmallVectorImpl;

std::string GetHostname();
StringRef GetHostname(SmallVectorImpl<char>& name);
}  // namespace wpi

#endif  // WPIUTIL_WPI_HOSTNAME_H_
