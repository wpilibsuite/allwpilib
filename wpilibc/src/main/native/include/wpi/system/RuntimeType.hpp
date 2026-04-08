// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

namespace wpi {
/**
 * Runtime type.
 */
enum class RuntimeType {
  /// roboRIO 1.0.
  ROBORIO,
  /// roboRIO 2.0.
  ROBORIO_2,
  /// Simulation runtime.
  SIMULATION,
  SYSTEMCORE
};
}  // namespace wpi
