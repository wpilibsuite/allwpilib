// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <string>

#include <wpi/json.h>

namespace fieldcalibration {
int calibrate(std::string inputDirPath, wpi::json& output_json,
              std::string camera_model_path, std::string ideal_map_path,
              int pinnedTagId, bool showDebugWindow);
}  // namespace fieldcalibration
