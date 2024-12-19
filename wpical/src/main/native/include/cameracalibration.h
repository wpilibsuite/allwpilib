// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <fstream>
#include <string>

#include <mrcal_wrapper.h>
#include <opencv2/aruco.hpp>
#include <opencv2/aruco/charuco.hpp>
#include <opencv2/objdetect/aruco_board.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/videoio.hpp>
#include <wpi/json.h>

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
