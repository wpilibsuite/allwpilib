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
  kRoboRIO,
  /// roboRIO 2.0.
  kRoboRIO2,
  /// Simulation runtime.
  kSimulation
};
}  // namespace frc
