// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once
#include <gtsam/geometry/Cal3_S2.h>
#include <gtsam/geometry/Pose3.h>
#include <gtsam/slam/expressions.h>
#include <units/length.h>

#include <map>
#include <vector>

#include <frc/apriltag/AprilTagFieldLayout.h>
#include <opencv2/core/types.hpp>

#include "TagDetection.h"

namespace wpical {
using CameraMatrix = Eigen::Matrix<double, 3, 3>;
using DistortionMatrix = Eigen::Matrix<double, 8, 1>;

class GtsamApriltagMap {
 public:
  GtsamApriltagMap(const frc::AprilTagFieldLayout& layout,
                   units::meter_t tagWidth);

  /**
   * The pose of tag {id} in the field map, or empty if not found.
   */
  const std::optional<gtsam::Pose3> WorldToTag(const int id) const;

  /*
   * List of corners mapped from 3d space (meters) to the 2d camera screen
   * (pixels).
   *  ⟶ +X  3 ----- 2
   *  |      |       |
   *  V      |       |
   *  +Y     0 ----- 1
   * In object space, we have this order, with origin at the center and +X out
   * of the tag
   */
  inline const std::vector<cv::Point3f>& TagToCornersCv() const {
    return tagToCornersCv;
  }

  /**
   * A list of autodiff Expressions for the location of this tag's 4 corners in
   * the field, given an Expression for the tag origin's pose.
   */
  const std::vector<gtsam::Point3_> WorldToCornersFactor(
      const gtsam::Pose3_ worldTtag) const;

 private:
  std::vector<gtsam::Point3> tagToCorners;
  std::vector<cv::Point3f> tagToCornersCv;
  std::map<int, gtsam::Pose3> worldTtags;
};

/**
 * Create an Expression for where we would see {worldPcorner} in an image
 * captured by a camera with {cameraCal} located at {worldTcamera_fac}.
 */
gtsam::Point2_ PredictLandmarkImageLocationFactor(
    gtsam::Pose3_ worldTcamera_fac, gtsam::Cal3_S2_ cameraCal,
    gtsam::Point3_ worldPcorner);

/**
 * Generate a seed pose for our camera, given a single tag detection. Note that
 * tag corners must be externally undistorted.
 */
std::optional<gtsam::Pose3> EstimateWorldTCam_SingleTag(
    TagDetection result, GtsamApriltagMap aprilTags, CameraMatrix camMat);

}  // namespace wpical
