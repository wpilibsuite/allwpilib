// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <string>

#include "cameracalibration.h"

namespace fieldcalibration {
int calibrate(std::string input_dir_path, std::string output_file_path,
              std::string camera_model_path, std::string ideal_map_path,
              int pinned_tag_id, bool show_debug_window);
}  // namespace fieldcalibration
