// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/apriltag/AprilTagDetection.hpp"

#include <type_traits>

#ifdef _WIN32
#pragma warning(disable : 4200)
#elif defined(__clang__)
#pragma clang diagnostic ignored "-Wc99-extensions"
#elif defined(__GNUC__)
#pragma GCC diagnostic ignored "-Wpedantic"
#endif

#include "apriltag.h"

using namespace frc;

static_assert(sizeof(AprilTagDetection) == sizeof(apriltag_detection_t),
              "structure sizes don't match");
static_assert(std::is_standard_layout_v<AprilTagDetection>,
              "AprilTagDetection is not standard layout?");

std::string_view AprilTagDetection::GetFamily() const {
  return static_cast<const apriltag_family_t*>(family)->name;
}

std::span<const double, 9> AprilTagDetection::GetHomography() const {
  return std::span<const double, 9>{static_cast<matd_t*>(H)->data, 9};
}

Eigen::Matrix3d AprilTagDetection::GetHomographyMatrix() const {
  return Eigen::Map<Eigen::Matrix<double, 3, 3, Eigen::RowMajor>>{
      static_cast<matd_t*>(H)->data};
}
