// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "fieldcalibration.hpp"

#include <algorithm>
#include <filesystem>
#include <functional>
#include <iostream>
#include <map>
#include <string>
#include <utility>
#include <vector>

#include <Eigen/Core>
#include <Eigen/Geometry>
#include <ceres/ceres.h>
#include <opencv2/calib3d.hpp>
#include <opencv2/core/eigen.hpp>
#include <opencv2/core/types.hpp>
#include <opencv2/core/utils/logger.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/videoio.hpp>

#include "cameracalibration.hpp"
#include "gtsam_meme/wpical_gtsam.h"
#include "wpi/apriltag/AprilTag.hpp"
#include "wpi/apriltag/AprilTagDetector.hpp"
#include "wpi/apriltag/AprilTagDetector_cv.hpp"
#include "wpi/apriltag/AprilTagFieldLayout.hpp"

/**
 * Convert a video file to a list of keyframes
 */
inline bool ProcessVideoFile(wpi::apriltag::AprilTagDetector& detector,
                             const wpical::CameraModel& cameraModel,
                             double tagSize, const std::string& path,
                             gtsam::Key& keyframe,
                             wpical::KeyframeMap& outputMap,
                             bool showDebugWindow) {
  // clear inputs
  outputMap.clear();

  if (showDebugWindow) {
    cv::namedWindow("Processing Frame", cv::WINDOW_NORMAL);
  }
  cv::VideoCapture videoInput(path);

  if (!videoInput.isOpened()) {
    return false;
  }

  // Reuse mats if we can - allocatiosn are expensive
  cv::Mat frame;
  cv::Mat frameGray;
  cv::Mat frameDebug;

  while (videoInput.read(frame)) {
    std::cout << "Processing " << path << " - Frame " << keyframe << std::endl;

    // Convert color frame to grayscale frame
    cv::cvtColor(frame, frameGray, cv::COLOR_BGR2GRAY);

    // Clone color frame for debugging
    frameDebug = frame.clone();

    auto results = wpi::apriltag::AprilTagDetect(detector, frameGray);

    // Skip this loop if there are no tags detected
    if (results.empty()) {
      continue;
    }

    std::vector<TagDetection> tagsThisKeyframe;
    for (auto& atag : results) {
      auto tag_corners_cv = std::vector<cv::Point2d>{};
      for (int corn = 0; corn < 4; corn++) {
        tag_corners_cv.emplace_back(atag.GetCorner(corn).x,
                                    atag.GetCorner(corn).y);
      }

      // Undistort so gtsam doesn't have to deal with distortion
      cv::Mat camCalCv(cameraModel.intrinsicMatrix.rows(),
                       cameraModel.intrinsicMatrix.cols(), CV_64F);
      cv::Mat camDistCv(cameraModel.distortionCoefficients.rows(),
                        cameraModel.distortionCoefficients.cols(), CV_64F);

      cv::eigen2cv(cameraModel.intrinsicMatrix, camCalCv);
      cv::eigen2cv(cameraModel.distortionCoefficients, camDistCv);

      cv::undistortImagePoints(tag_corners_cv, tag_corners_cv, camCalCv,
                               camDistCv);

      TagDetection tag;
      tag.id = atag.GetId();

      tag.corners.resize(4);
      std::transform(tag_corners_cv.begin(), tag_corners_cv.end(),
                     tag.corners.begin(),
                     [](const auto& it) { return TargetCorner{it.x, it.y}; });

      tagsThisKeyframe.push_back(tag);
    }
    outputMap[keyframe] = tagsThisKeyframe;

    // Show debug
    if (showDebugWindow) {
      cv::imshow("Processing Frame", frameDebug);
      cv::waitKey(1);
    }

    // Keep track of the frame number
    keyframe++;
  }

  videoInput.release();
  if (showDebugWindow) {
    cv::destroyAllWindows();
  }

  return true;
}

wpical::FieldCalibrator::~FieldCalibrator() {
  if (m_processingThread.joinable()) {
    m_processingThread.detach();
  }
}

std::optional<wpi::apriltag::AprilTagFieldLayout> wpical::calibrate(
    std::string inputDirPath, wpical::CameraModel& cameraModel,
    const wpi::apriltag::AprilTagFieldLayout& idealLayout, int pinnedTagId,
    bool showDebugWindow) {
  // Silence OpenCV logging
  cv::utils::logging::setLogLevel(
      cv::utils::logging::LogLevel::LOG_LEVEL_SILENT);

  // Convert intrinsics to gtsam-land. Order fx fy s u0 v0
  gtsam::Cal3_S2 gtsam_cal{
      cameraModel.intrinsicMatrix(0, 0), cameraModel.intrinsicMatrix(1, 1), 0,
      cameraModel.intrinsicMatrix(0, 2), cameraModel.intrinsicMatrix(1, 2),
  };
  bool pinnedTagFound = false;
  // Check if pinned tag is in ideal layout
  for (const auto& tag : idealLayout.GetTags()) {
    if (tag.ID == pinnedTagId) {
      pinnedTagFound = true;
      break;
    }
  }

  if (!pinnedTagFound) {
    return std::nullopt;
  }

  // Apriltag detector
  wpi::apriltag::AprilTagDetector detector;
  detector.SetConfig({.numThreads = 8});
  detector.AddFamily("tag36h11");

  // Write down keyframes from all our video files
  wpical::KeyframeMap outputMap;

  gtsam::Key keyframe{gtsam::symbol_shorthand::X(0)};

  constexpr units::meter_t TAG_SIZE = 0.1651_m;

  for (const auto& entry : std::filesystem::directory_iterator(inputDirPath)) {
    // Ignore hidden files
    if (entry.path().filename().string()[0] == '.') {
      continue;
    }

    const std::string path = entry.path().string();

    bool success =
        ProcessVideoFile(detector, cameraModel, TAG_SIZE.to<double>(), path,
                         keyframe, outputMap, showDebugWindow);

    if (!success) {
      std::cout << "Unable to process video " << path << std::endl;
      return std::nullopt;
    }
  }

  wpical::GtsamApriltagMap layoutGuess{idealTagLayout, TAG_SIZE};

  // TODO - handle constraints more generally (ie, multiple, tunable)

  // Noise on our pose prior. order is rx, ry, rz, tx, ty, tz, and units are
  // [rad] and [m].
  // Guess ~1 degree and 5 mm for fun.
  using gtsam::Vector3;
  using gtsam::Vector6;
  Vector6 sigmas;
  sigmas << Vector3::Constant(0.015), Vector3::Constant(0.005);
  gtsam::SharedNoiseModel posePriorNoise =
      gtsam::noiseModel::Diagonal::Sigmas(sigmas);
  std::map<int32_t, std::pair<gtsam::Pose3, gtsam::SharedNoiseModel>> fixedTags{
      {pinned_tag_id,
       {layoutGuess.WorldToTag(pinned_tag_id).value(), posePriorNoise}}};

  // one pixel in u and v - TODO don't hardcode this
  gtsam::SharedNoiseModel cameraNoise{
      gtsam::noiseModel::Isotropic::Sigma(2, 1.0)};

  auto calResult = wpical::OptimizeLayout(layoutGuess, outputMap, gtsam_cal,
                                          fixedTags, cameraNoise);

  return calResult.optimizedLayout;
}
