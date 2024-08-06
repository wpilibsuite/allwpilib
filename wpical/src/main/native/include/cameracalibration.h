// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <string_view>

#include <mrcal_wrapper.h>
#include <opencv2/aruco.hpp>
#include <opencv2/aruco/charuco.hpp>
#include <opencv2/objdetect/aruco_board.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/videoio.hpp>
#include <wpi/json.h>

namespace cameracalibration {
wpi::json calibrate(std::string_view input_video, float square_width,
                    float marker_width, int board_width, int board_height);
wpi::json calibrate(std::string_view input_video, float square_width,
                    int board_width, int board_height, double imagerWidthPixels,
                    double imagerHeightPixels, double focal_length_guess);
}  // namespace cameracalibration
