// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

namespace frc {
/**
 * Runtime type.
 */
enum RuntimeType {
  /// roboRIO 1.0.
  ROBORIO,
  /// roboRIO 2.0.
  ROBORIO2,
  /// Simulation runtime.
  SIMULATION
};
}  // namespace frc
