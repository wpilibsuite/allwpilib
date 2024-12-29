// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "fieldcalibration.hpp"

#include <filesystem>
#include <functional>
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
#include "wpi/apriltag/AprilTag.hpp"
#include "wpi/apriltag/AprilTagDetector.hpp"
#include "wpi/apriltag/AprilTagDetector_cv.hpp"
#include "wpi/apriltag/AprilTagFieldLayout.hpp"
#include "wpi/math/geometry/Rotation3d.hpp"
#include "wpi/math/geometry/Translation3d.hpp"
#include "gtsam_meme/wpical_gtsam.h"

inline Eigen::Matrix4d EstimateTagPose(
    std::span<double, 8> tagCorners,
    const wpical::CameraModel& cameraModel, double tagSize) {
  cv::Mat cameraMatrix;
  cv::Mat cameraDistortion;

  cv::eigen2cv(cameraModel.intrinsicMatrix, cameraMatrix);
  cv::eigen2cv(cameraModel.distortionCoefficients, cameraDistortion);

  std::array<cv::Point2d, 4> corners;
  for (int i = 0; i < 4; i++) {
    corners[i] = {tagCorners[i * 2], tagCorners[i * 2 + 1]};
  }

  std::vector<cv::Point3f> points3dBoxBase = {
      cv::Point3f(-tagSize / 2.0, tagSize / 2.0, 0.0),
      cv::Point3f(tagSize / 2.0, tagSize / 2.0, 0.0),
      cv::Point3f(tagSize / 2.0, -tagSize / 2.0, 0.0),
      cv::Point3f(-tagSize / 2.0, -tagSize / 2.0, 0.0)};

  std::vector<double> rVec;
  std::vector<double> tVec;

  cv::solvePnP(points3dBoxBase, corners, cameraMatrix, cameraDistortion, rVec,
               tVec, false, cv::SOLVEPNP_SQPNP);

  cv::Mat rMat;
  cv::Rodrigues(rVec, rMat);

  Eigen::Matrix<double, 4, 4> cameraToTag{
      {rMat.at<double>(0, 0), rMat.at<double>(0, 1), rMat.at<double>(0, 2),
       tVec.at(0)},
      {rMat.at<double>(1, 0), rMat.at<double>(1, 1), rMat.at<double>(1, 2),
       tVec.at(1)},
      {rMat.at<double>(2, 0), rMat.at<double>(2, 1), rMat.at<double>(2, 2),
       tVec.at(2)},
      {0.0, 0.0, 0.0, 1.0}};

  return cameraToTag;
}

inline void DrawTagCube(cv::Mat& frame, Eigen::Matrix4d cameraToTag,
                        const wpical::CameraModel& cameraModel,
                        double tagSize) {
  cv::Mat cameraMatrix;
  cv::Mat cameraDistortion;

  cv::eigen2cv(cameraModel.intrinsicMatrix, cameraMatrix);
  cv::eigen2cv(cameraModel.distortionCoefficients, cameraDistortion);

  std::vector<cv::Point3f> points3dBoxBase = {
      cv::Point3f(-tagSize / 2.0, tagSize / 2.0, 0.0),
      cv::Point3f(tagSize / 2.0, tagSize / 2.0, 0.0),
      cv::Point3f(tagSize / 2.0, -tagSize / 2.0, 0.0),
      cv::Point3f(-tagSize / 2.0, -tagSize / 2.0, 0.0)};

  std::vector<cv::Point3f> points3dBoxTop = {
      cv::Point3f(-tagSize / 2.0, tagSize / 2.0, -tagSize),
      cv::Point3f(tagSize / 2.0, tagSize / 2.0, -tagSize),
      cv::Point3f(tagSize / 2.0, -tagSize / 2.0, -tagSize),
      cv::Point3f(-tagSize / 2.0, -tagSize / 2.0, -tagSize)};

  std::vector<cv::Point2f> points2dBoxBase = {
      cv::Point2f(0.0, 0.0), cv::Point2f(0.0, 0.0), cv::Point2f(0.0, 0.0),
      cv::Point2f(0.0, 0.0)};

  std::vector<cv::Point2f> points2dBoxTop = {
      cv::Point2f(0.0, 0.0), cv::Point2f(0.0, 0.0), cv::Point2f(0.0, 0.0),
      cv::Point2f(0.0, 0.0)};

  Eigen::Matrix<double, 3, 3> rVec = cameraToTag.block<3, 3>(0, 0);
  Eigen::Matrix<double, 3, 1> tVec = cameraToTag.block<3, 1>(0, 3);

  cv::Mat rVecCv;
  cv::Mat tVecCv;

  cv::eigen2cv(rVec, rVecCv);
  cv::eigen2cv(tVec, tVecCv);

  cv::projectPoints(points3dBoxBase, rVecCv, tVecCv, cameraMatrix,
                    cameraDistortion, points2dBoxBase);
  cv::projectPoints(points3dBoxTop, rVecCv, tVecCv, cameraMatrix,
                    cameraDistortion, points2dBoxTop);

  for (int i = 0; i < 4; i++) {
    cv::Point2f& pointBase = points2dBoxBase.at(i);
    cv::Point2f& pointTop = points2dBoxTop.at(i);

    cv::line(frame, pointBase, pointTop, cv::Scalar(0, 255, 255), 5);

    int next = (i + 1) % 4;
    cv::Point2f& pointBaseNext = points2dBoxBase.at(next);
    cv::Point2f& pointTopNext = points2dBoxTop.at(next);

    cv::line(frame, pointBase, pointBaseNext, cv::Scalar(0, 255, 255), 5);
    cv::line(frame, pointTop, pointTopNext, cv::Scalar(0, 255, 255), 5);
  }
}

/**
 * Convert a video file to a list of keyframes
 */
inline bool ProcessVideoFile(
    wpi::apriltag::AprilTagDetector& detector,
    const wpical::CameraModel& cameraModel, double tagSize,
    const std::string& path, gtsam::Key& keyframe,
    wpical::KeyframeMap& outputMap, bool show_debug_window) {
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
      cv::Mat camCalCv(cameraModel.intrinsicMatrix.rows(), cameraModel.intrinsicMatrix.cols(), CV_64F);
      cv::Mat camDistCv(cameraModel.distortionCoefficients.rows(), cameraModel.distortionCoefficients.cols(),
                        CV_64F);

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

  for (const auto& entry :
       std::filesystem::directory_iterator(inputDirPath)) {
    // Ignore hidden files
    if (entry.path().filename().string()[0] == '.') {
      continue;
    }

    const std::string path = entry.path().string();

    bool success = ProcessVideoFile(
        detector, cameraModel, TAG_SIZE.to<double>(),
        path, keyframe, outputMap, showDebugWindow);

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
