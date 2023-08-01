// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <string_view>

#include <wpi/SymbolExports.h>

#include "frc/apriltag/AprilTagFieldLayout.h"

namespace frc {

enum class AprilTagField {
  k2022RapidReact,
  k2023ChargedUp,

  // This is a placeholder for denoting the last supported field. This should
  // always be the last entry in the enum and should not be used by users
  kNumFields,
};

/**
 * Loads an AprilTagFieldLayout from a predefined field
 *
 * @param field The predefined field
 */
WPILIB_DLLEXPORT AprilTagFieldLayout
LoadAprilTagLayoutField(AprilTagField field);

}  // namespace frc
