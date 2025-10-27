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
  Eigen::Matrix<double, 3, 3> intrinsic_matrix;
  Eigen::Matrix<double, 8, 1> distortion_coefficients;
  double avg_reprojection_error;
};

int calibrate(const std::string& input_video, CameraModel& camera_model,
              float square_width, float marker_width, int board_width,
              int board_height, bool show_debug_window);
int calibrate(const std::string& input_video, CameraModel& camera_model,
              float square_width, float marker_width, int board_width,
              int board_height, double imagerWidthPixels,
              double imagerHeightPixels, bool show_debug_window);
int calibrate(const std::string& input_video, CameraModel& camera_model,
              float square_width, int board_width, int board_height,
              double imagerWidthPixels, double imagerHeightPixels,
              bool show_debug_window);
static void dumpJson(CameraModel& camera_model,
                     const std::string& output_file_path) {
  std::vector<double> camera_matrix(camera_model.intrinsic_matrix.data(),
                                    camera_model.intrinsic_matrix.data() +
                                        camera_model.intrinsic_matrix.size());
  std::vector<double> distortion_coefficients(
      camera_model.distortion_coefficients.data(),
      camera_model.distortion_coefficients.data() +
          camera_model.distortion_coefficients.size());

  wpi::json result = {
      {"camera_matrix", camera_matrix},
      {"distortion_coefficients", distortion_coefficients},
      {"avg_reprojection_error", camera_model.avg_reprojection_error}};

  std::ofstream output_file(output_file_path);
  output_file << result.dump(4) << std::endl;
  output_file.close();
}
}  // namespace cameracalibration
