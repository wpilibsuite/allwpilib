// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "gtsam_tag_map.h"

#include <map>
#include <vector>

#include <opencv2/calib3d.hpp>
#include <opencv2/core/eigen.hpp>

#include "pose_converters.h"

using namespace gtsam;
using namespace wpical;

static std::map<int, Pose3> TagLayoutToMap(
    const frc::AprilTagFieldLayout& layout) {
  std::map<int, Pose3> worldTtags;

  for (const frc::AprilTag& tag : layout.GetTags()) {
    worldTtags[tag.ID] = Pose3dToGtsamPose3(tag.pose);
  }

  return worldTtags;
}

static std::vector<Point3> MakeTagModel(double width) {
  return {
      {0, -width / 2.0, -width / 2.0},
      {0, width / 2.0, -width / 2.0},
      {0, width / 2.0, width / 2.0},
      {0, -width / 2.0, width / 2.0},
  };
}
static std::vector<cv::Point3f> MakeTagModelOpenCV(double width) {
  return {cv::Point3f(0, -width / 2.0, -width / 2.0),
          cv::Point3f(0, +width / 2.0, -width / 2.0),
          cv::Point3f(0, +width / 2.0, +width / 2.0),
          cv::Point3f(0, -width / 2.0, +width / 2.0)};
}

wpical::GtsamApriltagMap::GtsamApriltagMap(
    const frc::AprilTagFieldLayout& layout, units::meter_t tagWidth)
    : tagToCorners{MakeTagModel(tagWidth.to<double>())},
      tagToCornersCv{MakeTagModelOpenCV(tagWidth.to<double>())},
      worldTtags{TagLayoutToMap(layout)} {}

const std::optional<Pose3> wpical::GtsamApriltagMap::WorldToTag(
    const int id) const {
  if (auto it = worldTtags.find(id); it != worldTtags.end()) {
    return it->second;
  } else {
    return std::nullopt;
  }
}

const std::vector<Point3_> wpical::GtsamApriltagMap::WorldToCornersFactor(
    const Pose3_ worldTtag) const {
  std::vector<Point3_> out;
  out.reserve(tagToCorners.size());
  for (const auto& p : tagToCorners) {
    out.push_back(transformFrom(worldTtag, p));
  }

  return out;
}

gtsam::Point2_ wpical::PredictLandmarkImageLocationFactor(
    gtsam::Pose3_ worldTcamera_fac, gtsam::Cal3_S2_ cameraCal,
    gtsam::Point3_ worldPcorner) {
  // Tag corner 3D position in the camera frame
  const Point3_ camPcorner = transformTo(worldTcamera_fac, worldPcorner);
  // project from vector down to pinhole location, then uncalibrate to pixel
  // locations
  const Point2_ prediction =
      uncalibrate<Cal3_S2>(cameraCal, project(camPcorner));

  return prediction;
}

std::optional<gtsam::Pose3> wpical::EstimateWorldTCam_SingleTag(
    TagDetection tagToUse, wpical::GtsamApriltagMap aprilTags,
    CameraMatrix camMat) {
  Eigen::Matrix<double, 8, 1> distCoeffs = wpical::DistortionMatrix::Zero();

  const std::vector<cv::Point3f>& objectPoints = aprilTags.TagToCornersCv();

  std::vector<cv::Point2f> imagePoints;
  imagePoints.reserve(4);
  for (const auto& corner : tagToUse.corners) {
    imagePoints.emplace_back(corner.x, corner.y);
  }

  // eigen/cv marshalling
  cv::Mat cameraMatCV(camMat.rows(), camMat.cols(), cv::DataType<double>::type);
  cv::eigen2cv(camMat, cameraMatCV);
  cv::Mat distCoeffsMatCV(distCoeffs.rows(), distCoeffs.cols(),
                          cv::DataType<double>::type);
  cv::eigen2cv(distCoeffs, distCoeffsMatCV);

  // actually do solvepnp
  cv::Mat rvec(3, 1, cv::DataType<double>::type);
  cv::Mat tvec(3, 1, cv::DataType<double>::type);
  cv::solvePnP(objectPoints, imagePoints, cameraMatCV, distCoeffsMatCV, rvec,
               tvec);

  if (const auto w2tag = aprilTags.WorldToTag(tagToUse.id)) {
    auto camToTag = Pose3dToGtsamPose3(ToPose3d(tvec, rvec));
    auto tag2cam = camToTag.inverse();
    return w2tag.value().transformPoseFrom(tag2cam);
  } else {
    return std::nullopt;
  }
}
