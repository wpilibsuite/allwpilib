// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

namespace frc {
/**
 * Compressor config type.
 */
enum class CompressorConfigType {
  /// Disabled.
  Disabled = 0,
  /// Digital.
  Digital = 1,
  /// Analog.
  Analog = 2,
  /// Hybrid.
  Hybrid = 3
};

}  // namespace frc
