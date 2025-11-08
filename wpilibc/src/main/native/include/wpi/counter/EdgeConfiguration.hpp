// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

namespace wpi {
/**
 * Edge configuration.
 */
enum class EdgeConfiguration {
  /// Rising edge configuration.
  kRisingEdge = 0,
  /// Falling edge configuration.
  kFallingEdge = 1,
};
}  // namespace wpi
