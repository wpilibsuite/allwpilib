// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/apriltag/AprilTagPoseEstimator.hpp"

#include <Eigen/QR>

#include "wpi/apriltag/AprilTagDetection.hpp"

#ifdef _WIN32
#pragma warning(disable : 4200)
#elif defined(__clang__)
#pragma clang diagnostic ignored "-Wc99-extensions"
#elif defined(__GNUC__)
#pragma GCC diagnostic ignored "-Wpedantic"
#endif

#include "apriltag.h"
#include "apriltag_pose.h"

using namespace wpi::apriltag;

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

static wpi::math::Transform3d MakePose(const apriltag_pose_t& pose) {
  if (!pose.R || !pose.t) {
    return {};
  }
  return {wpi::math::Translation3d{wpi::units::meter_t{pose.t->data[0]},
                                   wpi::units::meter_t{pose.t->data[1]},
                                   wpi::units::meter_t{pose.t->data[2]}},
          wpi::math::Rotation3d{OrthogonalizeRotationMatrix(
              Eigen::Map<Eigen::Matrix<double, 3, 3, Eigen::RowMajor>>{
                  pose.R->data})}};
}

static apriltag_detection_info_t MakeDetectionInfo(
    const apriltag_detection_t* det,
    const AprilTagPoseEstimator::Config& config) {
  return {const_cast<apriltag_detection_t*>(det),
          config.tagSize.value(),
          config.fx,
          config.fy,
          config.cx,
          config.cy};
}

static apriltag_detection_t MakeBasicDet(
    std::span<const double, 9> homography,
    const std::span<const double, 8>* corners) {
  apriltag_detection_t detection;
  detection.H = matd_create(3, 3);
  std::memcpy(detection.H->data, homography.data(), 9 * sizeof(double));
  if (corners) {
    for (int i = 0; i < 4; i++) {
      detection.p[i][0] = (*corners)[i * 2];
      detection.p[i][1] = (*corners)[i * 2 + 1];
    }
  }
  return detection;
}

static wpi::math::Transform3d DoEstimateHomography(
    const apriltag_detection_t* detection,
    const AprilTagPoseEstimator::Config& config) {
  auto info = MakeDetectionInfo(detection, config);
  apriltag_pose_t pose;
  estimate_pose_for_tag_homography(&info, &pose);
  return MakePose(pose);
}

wpi::math::Transform3d AprilTagPoseEstimator::EstimateHomography(
    const AprilTagDetection& detection) const {
  return DoEstimateHomography(
      reinterpret_cast<const apriltag_detection_t*>(&detection), m_config);
}

wpi::math::Transform3d AprilTagPoseEstimator::EstimateHomography(
    std::span<const double, 9> homography) const {
  auto detection = MakeBasicDet(homography, nullptr);
  auto rv = DoEstimateHomography(&detection, m_config);
  matd_destroy(detection.H);
  return rv;
}

static AprilTagPoseEstimate DoEstimateOrthogonalIteration(
    const apriltag_detection_t* detection,
    const AprilTagPoseEstimator::Config& config, int nIters) {
  auto info = MakeDetectionInfo(detection, config);
  apriltag_pose_t pose1, pose2;
  double err1, err2;
  estimate_tag_pose_orthogonal_iteration(&info, &err1, &pose1, &err2, &pose2,
                                         nIters, 1e-7);
  return {MakePose(pose1), MakePose(pose2), err1, err2};
}

AprilTagPoseEstimate AprilTagPoseEstimator::EstimateOrthogonalIteration(
    const AprilTagDetection& detection, int nIters) const {
  return DoEstimateOrthogonalIteration(
      reinterpret_cast<const apriltag_detection_t*>(&detection), m_config,
      nIters);
}

AprilTagPoseEstimate AprilTagPoseEstimator::EstimateOrthogonalIteration(
    std::span<const double, 9> homography, std::span<const double, 8> corners,
    int nIters) const {
  auto detection = MakeBasicDet(homography, &corners);
  auto rv = DoEstimateOrthogonalIteration(&detection, m_config, nIters);
  matd_destroy(detection.H);
  return rv;
}

static wpi::math::Transform3d DoEstimate(
    const apriltag_detection_t* detection,
    const AprilTagPoseEstimator::Config& config) {
  auto info = MakeDetectionInfo(detection, config);
  apriltag_pose_t pose;
  estimate_tag_pose(&info, &pose);
  return MakePose(pose);
}

wpi::math::Transform3d AprilTagPoseEstimator::Estimate(
    const AprilTagDetection& detection) const {
  return DoEstimate(reinterpret_cast<const apriltag_detection_t*>(&detection),
                    m_config);
}

wpi::math::Transform3d AprilTagPoseEstimator::Estimate(
    std::span<const double, 9> homography,
    std::span<const double, 8> corners) const {
  auto detection = MakeBasicDet(homography, &corners);
  auto rv = DoEstimate(&detection, m_config);
  matd_destroy(detection.H);
  return rv;
}
