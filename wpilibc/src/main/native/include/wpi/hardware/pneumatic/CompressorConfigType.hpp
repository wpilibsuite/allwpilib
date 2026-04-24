// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

namespace wpi {
/**
 * Compressor config type.
 */
enum class CompressorConfigType {
  /// Disabled.
  DISABLED = 0,
  /// Digital.
  DIGITAL = 1,
  /// Analog.
  ANALOG = 2,
  /// Hybrid.
  HYBRID = 3
};

}  // namespace wpi
