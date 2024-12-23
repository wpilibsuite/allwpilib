// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <functional>
#include <map>
#include <string>
#include <tuple>
#include <vector>

#include <Eigen/Core>
#include <Eigen/Geometry>
#include <ceres/ceres.h>
#include <opencv2/calib3d.hpp>
#include <opencv2/core/eigen.hpp>
#include <opencv2/core/utils/logger.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/videoio.hpp>
#include <wpi/json.h>

#include "apriltag.h"

struct Pose {
  Eigen::Vector3d p;
  Eigen::Quaterniond q;
  EIGEN_MAKE_ALIGNED_OPERATOR_NEW
};

struct Constraint {
  int id_begin;
  int id_end;
  Pose t_begin_end;
  EIGEN_MAKE_ALIGNED_OPERATOR_NEW
};

struct CameraModel {
  Eigen::Matrix<double, 3, 3> intrinsic_matrix;
  Eigen::Matrix<double, 8, 1> distortion_coefficients;
};

namespace fieldcalibration {
static CameraModel load_camera_model(std::string path);
static CameraModel load_camera_model(wpi::json json_data);
static std::map<int, wpi::json> load_ideal_map(std::string path);
std::vector<Eigen::Vector3d> get_model_corners(double tag_size);
static Eigen::Matrix<double, 4, 4> get_tag_transform(
    std::map<int, wpi::json>& ideal_map, int tag_id);
static Eigen::Matrix<double, 4, 4> estimate_tag_pose(
    apriltag_detection_t* tag_detection,
    const Eigen::Matrix<double, 3, 3>& camera_matrix,
    const Eigen::Matrix<double, 8, 1>& camera_distortion, double tag_size);
static void draw_tag_cube(cv::Mat& frame,
                          Eigen::Matrix<double, 4, 4> camera_to_tag,
                          const Eigen::Matrix<double, 3, 3>& camera_matrix,
                          const Eigen::Matrix<double, 8, 1>& camera_distortion,
                          double tag_size);
static bool process_video_file(
    apriltag_detector_t* tag_detector,
    const Eigen::Matrix<double, 3, 3>& camera_matrix,
    const Eigen::Matrix<double, 8, 1>& camera_distortion, double tag_size,
    const std::string& path,
    std::map<int, Pose, std::less<int>,
             Eigen::aligned_allocator<std::pair<const int, Pose>>>& poses,
    std::vector<Constraint, Eigen::aligned_allocator<Constraint>>& constraints,
    bool show_debug_window);
int calibrate(std::string input_dir_path, std::string output_file_path,
              std::string camera_model_path, std::string ideal_map_path,
              int pinned_tag_id, bool show_debug_window);
}  // namespace fieldcalibration
