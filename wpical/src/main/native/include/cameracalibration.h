// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <string>

namespace cameracalibration {
int calibrate(const std::string& input_video, float square_width,
              float marker_width, int board_width, int board_height,
              bool show_debug_window);
int calibrate(const std::string& input_video, float square_width,
              float marker_width, int board_width, int board_height,
              double imagerWidthPixels, double imagerHeightPixels,
              bool show_debug_window);
int calibrate(const std::string& input_video, float square_width,
              int board_width, int board_height, double imagerWidthPixels,
              double imagerHeightPixels, bool show_debug_window);
}  // namespace cameracalibration
