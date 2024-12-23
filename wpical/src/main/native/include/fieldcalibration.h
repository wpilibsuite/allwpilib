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

namespace fieldcalibration {
int calibrate(std::string input_dir_path, std::string output_file_path,
              std::string camera_model_path, std::string ideal_map_path,
              int pinned_tag_id, bool show_debug_window);
}  // namespace fieldcalibration
