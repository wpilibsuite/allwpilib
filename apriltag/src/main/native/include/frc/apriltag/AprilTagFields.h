// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <string_view>

#include <wpi/SymbolExports.h>

namespace frc {

/**
 * Loadable AprilTag field layouts.
 */
enum class AprilTagField {
  /// 2022 Rapid React.
  k2022RapidReact,
  /// 2023 Charged Up.
  k2023ChargedUp,
  /// 2024 Crescendo.
  k2024Crescendo,
  /// 2025 Reefscape.
  k2025Reefscape,
  /// Alias to the current game.
  kDefaultField = k2025Reefscape,

  // This is a placeholder for denoting the last supported field. This should
  // always be the last entry in the enum and should not be used by users
  kNumFields,
};

}  // namespace frc
