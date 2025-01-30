// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <fstream>
#include <string>
#include <vector>

#include <Eigen/Core>
#include <wpi/json.h>

namespace cameracalibration {
struct CameraModel {
  Eigen::Matrix<double, 3, 3> intrinsicMatrix;
  Eigen::Matrix<double, 8, 1> distortionCoefficients;
  double avgReprojectionError;
};

std::optional<cameracalibration::CameraModel> calibrate(
    const std::string& input_video, float square_width, float marker_width,
    int board_width, int board_height, bool show_debug_window);
int calibrate(const std::string& input_video, CameraModel& camera_model,
              float square_width, float marker_width, int board_width,
              int board_height, double imagerWidthPixels,
              double imagerHeightPixels, bool show_debug_window);
std::optional<cameracalibration::CameraModel> calibrate(
    const std::string& input_video, float square_width, int board_width,
    int board_height, double imagerWidthPixels, double imagerHeightPixels,
    bool show_debug_window);

void to_json(wpi::json& json, const CameraModel& cameraModel);

void from_json(const wpi::json& json, CameraModel& cameraModel);
}  // namespace cameracalibration
