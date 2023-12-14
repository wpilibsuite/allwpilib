// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <concepts>

#include "frc2/command/CommandPtr.h"

namespace frc2 {
namespace cmd {

template<typename T>
concept OwnedCommand = requires (T command) {
  CommandPtr(command);
};

} // namespace cmd
} // namespace frc2
