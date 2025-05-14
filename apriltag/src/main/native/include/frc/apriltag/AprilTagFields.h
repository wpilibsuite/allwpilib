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
  RAPID_REACT_2022,
  /// 2023 Charged Up.
  CHARGED_UP_2023,
  /// 2024 Crescendo.
  CRESCENDO_2024,
  /// 2025 Reefscape AndyMark (see TU12).
  REEFSCAPE_2025_ANDYMARK,
  /// 2025 Reefscape Welded (see TU12).
  REEFSCAPE_2025_WELDED,
  /// Alias to the current game.
  DEFAULT_FIELD = REEFSCAPE_2025_WELDED,

  // This is a placeholder for denoting the last supported field. This should
  // always be the last entry in the enum and should not be used by users
  NUM_FIELDS,
};

}  // namespace frc
