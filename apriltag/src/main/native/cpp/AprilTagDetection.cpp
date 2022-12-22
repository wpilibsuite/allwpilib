// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/apriltag/AprilTagDetection.h"

#include <algorithm>
#include <type_traits>
#include <version>

#include <Eigen/QR>

#ifdef _WIN32
#pragma warning(disable : 4200)
#elif defined(__clang__)
#pragma clang diagnostic ignored "-Wc99-extensions"
#elif defined(__GNUC__)
#pragma GCC diagnostic ignored "-Wpedantic"
#endif

#include "apriltag.h"
#include "apriltag_pose.h"

using namespace frc;

static_assert(sizeof(AprilTagDetection) == sizeof(apriltag_detection_t),
              "structure sizes don't match");
static_assert(std::is_standard_layout_v<AprilTagDetection>,
              "AprilTagDetection is not standard layout?");
#ifdef __cpp_lib_is_layout_compatible
static_assert(
    std::is_layout_compatible_v<AprilTagDetection, apriltag_detection_t>,
    "not layout compatible");
#endif

static Eigen::Matrix3d OrthogonalizeRotationMatrix(
    const Eigen::Matrix3d& input) {
  Eigen::HouseholderQR<Eigen::Matrix3d> qr{input};

  Eigen::Matrix3d Q = qr.householderQ();
  Eigen::Matrix3d R = qr.matrixQR().triangularView<Eigen::Upper>();

  // Fix signs in R if they're < 0 so it's close to an identity matrix
  // (our QR decomposition implementation sometimes flips the signs of
  // columns)
  for (int colR = 0; colR < 3; ++colR) {
    if (R(colR, colR) < 0) {
      for (int rowQ = 0; rowQ < 3; ++rowQ) {
        Q(rowQ, colR) = -Q(rowQ, colR);
      }
    }
  }

  return Q;
}

static Transform3d MakePose(const apriltag_pose_t& pose) {
  if (!pose.R || !pose.t) {
    return {};
  }
  return {Translation3d{units::meter_t{pose.t->data[0]},
                        units::meter_t{pose.t->data[1]},
                        units::meter_t{pose.t->data[2]}},
          Rotation3d{OrthogonalizeRotationMatrix(
              Eigen::Map<Eigen::Matrix3d>{pose.R->data})}};
}

static apriltag_detection_info_t MakeDetectionInfo(
    const AprilTagDetection* det,
    const AprilTagDetection::PoseEstimatorConfig& config) {
  return {reinterpret_cast<apriltag_detection_t*>(
              const_cast<AprilTagDetection*>(det)),
          config.tagSize.value(),
          config.fx,
          config.fy,
          config.cx,
          config.cy};
}

std::string_view AprilTagDetection::GetFamily() const {
  return static_cast<const apriltag_family_t*>(family)->name;
}

Eigen::Matrix3d AprilTagDetection::GetHomographyMatrix() const {
  return Eigen::Matrix3d{static_cast<matd_t*>(H)->data};
}

Transform3d AprilTagDetection::EstimatePoseHomography(
    const PoseEstimatorConfig& config) const {
  auto info = MakeDetectionInfo(this, config);
  apriltag_pose_t pose;
  estimate_pose_for_tag_homography(&info, &pose);
  return MakePose(pose);
}

AprilTagDetection::PoseEstimate
AprilTagDetection::EstimatePoseOrthogonalIteration(
    const PoseEstimatorConfig& config, int nIters) const {
  auto info = MakeDetectionInfo(this, config);
  apriltag_pose_t pose1, pose2;
  double err1, err2;
  estimate_tag_pose_orthogonal_iteration(&info, &err1, &pose1, &err2, &pose2,
                                         nIters);
  return {MakePose(pose1), MakePose(pose2), err1, err2};
}

Transform3d AprilTagDetection::EstimatePose(
    const PoseEstimatorConfig& config) const {
  auto info = MakeDetectionInfo(this, config);
  apriltag_pose_t pose;
  estimate_tag_pose(&info, &pose);
  return MakePose(pose);
}

double AprilTagDetection::PoseEstimate::GetAmbiguity() const {
  auto min = (std::min)(error1, error2);
  auto max = (std::max)(error1, error2);

  if (max > 0) {
    return min / max;
  } else {
    return -1;
  }
}
