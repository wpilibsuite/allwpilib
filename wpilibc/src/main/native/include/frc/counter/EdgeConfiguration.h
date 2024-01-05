// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

namespace frc {
/**
 * Edge configuration.
 */
enum class EdgeConfiguration {
  /// No edge configuration (neither rising nor falling).
  kNone = 0,
  /// Rising edge configuration.
  kRisingEdge = 0x1,
  /// Falling edge configuration.
  kFallingEdge = 0x2,
  /// Both rising and falling edges configuration.
  kBoth = 0x3
};
}  // namespace frc
