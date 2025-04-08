// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "fieldcalibration.h"

#include <algorithm>
#include <filesystem>
#include <fstream>
#include <functional>
#include <iostream>
#include <map>
#include <string>
#include <utility>
#include <vector>

#include <Eigen/Core>
#include <Eigen/Geometry>
#include <frc/apriltag/AprilTagDetection.h>
#include <frc/apriltag/AprilTagFieldLayout.h>
#include <opencv2/calib3d.hpp>
#include <opencv2/core/eigen.hpp>
#include <opencv2/core/types.hpp>
#include <opencv2/core/utils/logger.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/videoio.hpp>

#include "apriltag.h"
#include "gtsam_meme/wpical_gtsam.h"
#include "tag36h11.h"
#include "cameracalibration.h"

inline Eigen::Matrix4d EstimateTagPose(
    std::span<double, 8> tagCorners,
    const cameracalibration::CameraModel& cameraModel, double tagSize) {
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

  cameracalibration::CameraModel camera_model{camera_matrix, camera_distortion, -1};
  return camera_model;
}

inline cameracalibration::CameraModel load_camera_model(wpi::json json_data) {
  // Camera matrix
  Eigen::Matrix<double, 3, 3> camera_matrix;

  for (int i = 0; i < camera_matrix.rows(); i++) {
    for (int j = 0; j < camera_matrix.cols(); j++) {
      camera_matrix(i, j) =
          json_data["camera_matrix"][(i * camera_matrix.cols()) + j];
    }
  }

  // Distortion coefficients
  Eigen::Matrix<double, 8, 1> camera_distortion;

  for (int i = 0; i < camera_distortion.rows(); i++) {
    for (int j = 0; j < camera_distortion.cols(); j++) {
      camera_distortion(i, j) = json_data["distortion_coefficients"]
                                         [(i * camera_distortion.cols()) + j];
    }
  }

  cameracalibration::CameraModel camera_model{camera_matrix, camera_distortion};
  return camera_model;
}

inline void draw_tag_cube(cv::Mat& frame,
                          Eigen::Matrix<double, 4, 4> camera_to_tag,
                          const Eigen::Matrix<double, 3, 3>& camera_matrix,
                          const Eigen::Matrix<double, 8, 1>& camera_distortion,
                          double tag_size) {
  cv::Mat camera_matrix_cv;
  cv::Mat camera_distortion_cv;

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
inline bool process_video_file(
    apriltag_detector_t* tag_detector,
    const Eigen::Matrix<double, 3, 3>& camera_matrix,
    const Eigen::Matrix<double, 8, 1>& camera_distortion, double tag_size,
    const std::string& path, gtsam::Key& keyframe,
    wpical::KeyframeMap& outputMap, bool show_debug_window) {
  // clear inputs
  outputMap.clear();

  if (show_debug_window) {
    cv::namedWindow("Processing Frame", cv::WINDOW_NORMAL);
  }
  cv::VideoCapture videoInput(path);

  if (!videoInput.isOpened()) {
    std::cout << "Unable to open video " << path << std::endl;
    return false;
  }

  // Reuse mats if we can - allocatiosn are expensive
  cv::Mat frame;
  cv::Mat frameGray;
  cv::Mat frameDebug;

  while (video_input.read(frame)) {
    std::cout << "Processing " << path << " - Frame " << keyframe << std::endl;

    // Convert color frame to grayscale frame
    cv::cvtColor(frame, frameGray, cv::COLOR_BGR2GRAY);

    // Clone color frame for debugging
    frameDebug = frame.clone();

    auto results = frc::AprilTagDetect(detector, frameGray);

    // Skip this loop if there are no tags detected
    if (results.empty()) {
      std::cout << "No tags detected" << std::endl;
      continue;
    }

    std::vector<TagDetection> tagsThisKeyframe;
    for (int i = 0; i < zarray_size(tag_detections); i++) {
      apriltag_detection_t* tag_detection_i;
      zarray_get(tag_detections, i, &tag_detection_i);

      // Convert to our data type. I don't love how complicated this is.
      auto atag = reinterpret_cast<frc::AprilTagDetection*>(tag_detection_i);
      auto tag_corners_cv = std::vector<cv::Point2d>{};
      for (int corn = 0; corn < 4; corn++) {
        tag_corners_cv.emplace_back(atag->GetCorner(corn).x,
                                    atag->GetCorner(corn).y);
      }

      // Undistort so gtsam doesn't have to deal with distortion
      cv::Mat camCalCv(camera_matrix.rows(), camera_matrix.cols(), CV_64F);
      cv::Mat camDistCv(camera_distortion.rows(), camera_distortion.cols(),
                        CV_64F);

      cv::eigen2cv(camera_matrix, camCalCv);
      cv::eigen2cv(camera_distortion, camDistCv);

      cv::undistortImagePoints(tag_corners_cv, tag_corners_cv, camCalCv,
                               camDistCv);

      TagDetection tag;
      tag.id = atag->GetId();

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

int fieldcalibration::calibrate(std::string input_dir_path,
                                wpi::json& output_json,
                                std::string camera_model_path,
                                std::string ideal_map_path, int pinned_tag_id,
                                bool show_debug_window) {
  // // Silence OpenCV logging
  // cv::utils::logging::setLogLevel(
  //     cv::utils::logging::LogLevel::LOG_LEVEL_SILENT);

  // Load camera model
  Eigen::Matrix3d camera_matrix;
  Eigen::Matrix<double, 8, 1> camera_distortion;

  try {
    auto camera_model = load_camera_model(camera_model_path);
    camera_matrix = camera_model.intrinsic_matrix;
    camera_distortion = camera_model.distortion_coefficients;
  } catch (...) {
    return 1;
  }

  // Convert intrinsics to gtsam-land. Order fx fy s u0 v0
  gtsam::Cal3_S2 gtsam_cal{
      camera_matrix(0, 0), camera_matrix(1, 1), 0,
      camera_matrix(0, 2), camera_matrix(1, 2),
  };

  // Load the seed field layout from disk
  frc::AprilTagFieldLayout idealTagLayout;
  try {
    wpi::json json = wpi::json::parse(std::ifstream(ideal_map_path));
    idealTagLayout = json.get<frc::AprilTagFieldLayout>();
  } catch (...) {
    std::cerr << "Could not deserialize" << ideal_map_path << std::endl;
    return 1;
  }

  // Load ideal field map
  std::map<int, wpi::json> ideal_map;

  bool pinned_tag_found = false;
  // Check if pinned tag is in ideal map
  for (const auto& [tag_id, tag_json] : ideal_map) {
    if (tag_id == pinned_tag_id) {
      pinned_tag_found = true;
      break;
    }
  }

  if (!pinnedTagFound) {
    return std::nullopt;
  }

  // Apriltag detector
  frc::AprilTagDetector detector;
  detector.SetConfig({.numThreads = 8});
  detector.AddFamily("tag36h11");

  // Write down keyframes from all our video files
  wpical::KeyframeMap outputMap;

  gtsam::Key keyframe{gtsam::symbol_shorthand::X(0)};

  constexpr units::meter_t TAG_SIZE = 0.1651_m;

  for (const auto& entry :
       std::filesystem::directory_iterator(input_dir_path)) {
    // Ignore hidden files
    if (entry.path().filename().string()[0] == '.') {
      continue;
    }

    const std::string path = entry.path().string();

    bool success = process_video_file(
        tag_detector, camera_matrix, camera_distortion, TAG_SIZE.to<double>(),
        path, keyframe, outputMap, show_debug_window);

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

  // Convert the output AprilTagFieldLayout to a json
  wpi::json observed_map_json = calResult.optimizedLayout;

  output_json = observed_map_json;

  return 0;
}
