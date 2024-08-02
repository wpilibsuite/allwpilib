// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#ifndef FIELDCALIBRATION_H
#define FIELDCALIBRATION_H

#include <glog/export.h>
#include <ceres/ceres.h>

#include <filesystem>
#include <fstream>
#include <map>
#include <string>
#include <tuple>
#include <vector>

#include <Eigen/Core>
#include <Eigen/Geometry>
#include <nlohmann/json.hpp>
#include <opencv2/calib3d.hpp>
#include <opencv2/core/eigen.hpp>
#include <opencv2/core/utils/logger.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/videoio.hpp>

extern "C"
{
#include "apriltag/apriltag.h"
#include "apriltag/tag36h11.h"
} // extern "C"

struct Pose
{
  Eigen::Vector3d p;
  Eigen::Quaterniond q;
  EIGEN_MAKE_ALIGNED_OPERATOR_NEW
};

struct Constraint
{
  int id_begin;
  int id_end;
  Pose t_begin_end;
  EIGEN_MAKE_ALIGNED_OPERATOR_NEW
};

namespace fieldcalibration
{
  std::tuple<Eigen::Matrix<double, 3, 3>, Eigen::Matrix<double, 8, 1>> load_camera_model(std::string path);
  std::tuple<Eigen::Matrix<double, 3, 3>, Eigen::Matrix<double, 8, 1>> load_camera_model(nlohmann::json json_data);
  std::map<int, nlohmann::json> load_ideal_map(std::string path);
  Eigen::Matrix<double, 4, 4> get_tag_transform(std::map<int, nlohmann::json> &ideal_map, int tag_id);
  Eigen::Matrix<double, 4, 4> estimate_tag_pose(
      apriltag_detection_t *tag_detection,
      const Eigen::Matrix<double, 3, 3> &camera_matrix,
      const Eigen::Matrix<double, 8, 1> &camera_distortion, double tag_size);
  void draw_tag_cube(cv::Mat &frame, Eigen::Matrix<double, 4, 4> camera_to_tag,
                     const Eigen::Matrix<double, 3, 3> &camera_matrix,
                     const Eigen::Matrix<double, 8, 1> &camera_distortion,
                     double tag_size);
  bool process_video_file(
      apriltag_detector_t *tag_detector, int detection_fps,
      const Eigen::Matrix<double, 3, 3> &camera_matrix,
      const Eigen::Matrix<double, 8, 1> &camera_distortion, double tag_size,
      const std::string &path,
      std::map<int, Pose, std::less<int>,
               Eigen::aligned_allocator<std::pair<const int, Pose>>> &poses,
      std::vector<Constraint, Eigen::aligned_allocator<Constraint>> &constraints);
  int calibrate(std::string input_dir_path, std::string output_file_path,
                std::string camera_model_path, std::string ideal_map_path,
                int pinned_tag_id, int detection_fps);

  int calibrate(std::string input_dir_path, std::string output_file_path,
                nlohmann::json camera_model, std::string ideal_map_path,
                int pinned_tag_id, int detection_fps);
}
#endif // FIELDCALIBRATION_H
