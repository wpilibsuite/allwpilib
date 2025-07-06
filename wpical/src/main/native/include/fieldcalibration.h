// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <optional>
#include <string>

#include <frc/apriltag/AprilTagFieldLayout.h>

#include "cameracalibration.h"

namespace fieldcalibration {
std::optional<frc::AprilTagFieldLayout> calibrate(
    std::string inputDirPath, cameracalibration::CameraModel& cameraModel,
    const frc::AprilTagFieldLayout& idealLayout, int pinnedTagId,
    bool showDebugWindow);
}  // namespace fieldcalibration
