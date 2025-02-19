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
#include <ceres/ceres.h>
#include <frc/apriltag/AprilTag.h>
#include <frc/apriltag/AprilTagDetector.h>
#include <frc/apriltag/AprilTagDetector_cv.h>
#include <frc/apriltag/AprilTagFieldLayout.h>
#include <frc/geometry/Rotation3d.h>
#include <frc/geometry/Translation3d.h>
#include <opencv2/calib3d.hpp>
#include <opencv2/core/eigen.hpp>
#include <opencv2/core/types.hpp>
#include <opencv2/core/utils/logger.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/videoio.hpp>

#include "cameracalibration.h"

struct Pose {
  Eigen::Vector3d p;
  Eigen::Quaterniond q;
  EIGEN_MAKE_ALIGNED_OPERATOR_NEW
};

struct Constraint {
  int idBegin;
  int idEnd;
  Pose tBeginEnd;
  EIGEN_MAKE_ALIGNED_OPERATOR_NEW
};

class PoseGraphError {
 public:
  explicit PoseGraphError(Pose t_ab_observed)
      : m_t_ab_observed(std::move(t_ab_observed)) {}

  template <typename T>
  bool operator()(const T* const p_a_ptr, const T* const q_a_ptr,
                  const T* const p_b_ptr, const T* const q_b_ptr,
                  T* residuals_ptr) const {
    // Tag A
    Eigen::Map<const Eigen::Matrix<T, 3, 1>> p_a(p_a_ptr);
    Eigen::Map<const Eigen::Quaternion<T>> q_a(q_a_ptr);

    // Tag B
    Eigen::Map<const Eigen::Matrix<T, 3, 1>> p_b(p_b_ptr);
    Eigen::Map<const Eigen::Quaternion<T>> q_b(q_b_ptr);

    // Rotation between tag A to tag B
    Eigen::Quaternion<T> q_a_inverse = q_a.conjugate();
    Eigen::Quaternion<T> q_ab_estimated = q_a_inverse * q_b;

    // Displacement between tag A and tag B in tag A's frame
    Eigen::Matrix<T, 3, 1> p_ab_estimated = q_a_inverse * (p_b - p_a);

    // Error between the orientations
    Eigen::Quaternion<T> delta_q =
        m_t_ab_observed.q.template cast<T>() * q_ab_estimated.conjugate();

    // Residuals
    Eigen::Map<Eigen::Matrix<T, 6, 1>> residuals(residuals_ptr);
    residuals.template block<3, 1>(0, 0) =
        p_ab_estimated - m_t_ab_observed.p.template cast<T>();
    residuals.template block<3, 1>(3, 0) = T(2.0) * delta_q.vec();

    return true;
  }

  static ceres::CostFunction* Create(const Pose& t_ab_observed) {
    return new ceres::AutoDiffCostFunction<PoseGraphError, 6, 3, 4, 3, 4>(
        new PoseGraphError(t_ab_observed));
  }

  EIGEN_MAKE_ALIGNED_OPERATOR_NEW

 private:
  const Pose m_t_ab_observed;
};

const double tagSizeMeters = 0.1651;

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
                        const cameracalibration::CameraModel& cameraModel,
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

inline bool ProcessVideoFile(
    frc::AprilTagDetector& detector,
    const cameracalibration::CameraModel& cameraModel, double tagSize,
    const std::string& path,
    std::map<int, Pose, std::less<int>,
             Eigen::aligned_allocator<std::pair<const int, Pose>>>& poses,
    std::vector<Constraint, Eigen::aligned_allocator<Constraint>>& constraints,
    bool showDebugWindow) {
  if (showDebugWindow) {
    cv::namedWindow("Processing Frame", cv::WINDOW_NORMAL);
  }
  cv::VideoCapture videoInput(path);

  if (!videoInput.isOpened()) {
    std::cout << "Unable to open video " << path << std::endl;
    return false;
  }

  cv::Mat frame;
  cv::Mat frameGray;
  cv::Mat frameDebug;

  int frameNum = 0;

  while (videoInput.read(frame)) {
    std::cout << "Processing " << path << " - Frame " << frameNum++
              << std::endl;

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

    // Find detection with the smallest tag ID
    auto tagDetectionMin = results.front();
    for (auto detection : results) {
      if (detection->GetId() < tagDetectionMin->GetId()) {
        tagDetectionMin = detection;
      }
    }
    std::array<double, 8> cornerBuf;
    Eigen::Matrix4d cameraToTagMin;
    try {
      cameraToTagMin = EstimateTagPose(tagDetectionMin->GetCorners(cornerBuf),
                                       cameraModel, tagSize);
    } catch (...) {
      // SQPNP failed, probably because the camera model is bad
      return false;
    }

    // Find transformation from smallest tag ID
    for (auto detection : results) {
      // Add tag ID to poses
      if (poses.find(detection->GetId()) == poses.end()) {
        poses[detection->GetId()] = {Eigen::Vector3d(0.0, 0.0, 0.0),
                                     Eigen::Quaterniond(1.0, 0.0, 0.0, 0.0)};
      }
      std::array<double, 8> corners;
      // Estimate camera to tag pose
      Eigen::Matrix4d cameraToTag;
      try {
        cameraToTag = EstimateTagPose(detection->GetCorners(corners),
                                      cameraModel, tagSize);
      } catch (...) {
        // SQPNP failed, probably because the camera model is bad
        return false;
      }
      // Draw debug cube
      if (showDebugWindow) {
        DrawTagCube(frameDebug, cameraToTag, cameraModel, tagSize);
      }

      // Skip finding transformation from smallest tag ID to itself
      if (detection->GetId() == tagDetectionMin->GetId()) {
        continue;
      }

      Eigen::Matrix4d tagMinToTag = cameraToTagMin.inverse() * cameraToTag;

      // Constraint
      Constraint constraint;
      constraint.idBegin = tagDetectionMin->GetId();
      constraint.idEnd = detection->GetId();
      constraint.tBeginEnd.p = tagMinToTag.block<3, 1>(0, 3);
      constraint.tBeginEnd.q =
          Eigen::Quaterniond(tagMinToTag.block<3, 3>(0, 0));

      constraints.push_back(constraint);
    }

    // Show debug
    if (showDebugWindow) {
      cv::imshow("Processing Frame", frameDebug);
      cv::waitKey(1);
    }
  }

  videoInput.release();
  if (showDebugWindow) {
    cv::destroyAllWindows();
  }

  return true;
}

std::optional<frc::AprilTagFieldLayout> fieldcalibration::calibrate(
    std::string inputDirPath, cameracalibration::CameraModel& cameraModel,
    const frc::AprilTagFieldLayout& idealLayout, int pinnedTagId,
    bool showDebugWindow) {
  // Silence OpenCV logging
  cv::utils::logging::setLogLevel(
      cv::utils::logging::LogLevel::LOG_LEVEL_SILENT);

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
  frc::AprilTagDetector detector;
  detector.SetConfig({.numThreads = 8});
  detector.AddFamily("tag36h11");

  // Find tag poses
  std::map<int, Pose, std::less<int>,
           Eigen::aligned_allocator<std::pair<const int, Pose>>>
      poses;
  std::vector<Constraint, Eigen::aligned_allocator<Constraint>> constraints;

  for (const auto& entry : std::filesystem::directory_iterator(inputDirPath)) {
    if (entry.path().filename().string()[0] == '.') {
      continue;
    }

    const std::string path = entry.path().string();

    bool success = ProcessVideoFile(detector, cameraModel, tagSizeMeters, path,
                                    poses, constraints, showDebugWindow);

    if (!success) {
      std::cout << "Unable to process video " << path << std::endl;
      return std::nullopt;
    }
  }

  // Build optimization problem
  ceres::Problem problem;
  ceres::Manifold* quaternionManifold = new ceres::EigenQuaternionManifold;

  for (const auto& constraint : constraints) {
    auto poseBeginIter = poses.find(constraint.idBegin);
    auto poseEndIter = poses.find(constraint.idEnd);

    ceres::CostFunction* costFunction =
        PoseGraphError::Create(constraint.tBeginEnd);

    problem.AddResidualBlock(
        costFunction, nullptr, poseBeginIter->second.p.data(),
        poseBeginIter->second.q.coeffs().data(), poseEndIter->second.p.data(),
        poseEndIter->second.q.coeffs().data());

    problem.SetManifold(poseBeginIter->second.q.coeffs().data(),
                        quaternionManifold);
    problem.SetManifold(poseEndIter->second.q.coeffs().data(),
                        quaternionManifold);
  }

  // Pin tag
  auto pinnedTagIter = poses.find(pinnedTagId);
  if (pinnedTagIter != poses.end()) {
    problem.SetParameterBlockConstant(pinnedTagIter->second.p.data());
    problem.SetParameterBlockConstant(pinnedTagIter->second.q.coeffs().data());
  }

  // Solve
  ceres::Solver::Options options;
  options.max_num_iterations = 200;
  options.linear_solver_type = ceres::SPARSE_NORMAL_CHOLESKY;
  options.num_threads = 10;

  ceres::Solver::Summary summary;
  ceres::Solve(options, &problem, &summary);

  std::cout << summary.BriefReport() << std::endl;

  Eigen::Matrix4d correctionA;
  correctionA << 0, 0, -1, 0, 1, 0, 0, 0, 0, -1, 0, 0, 0, 0, 0, 1;

  Eigen::Matrix4d correctionB;
  correctionB << 0, 1, 0, 0, 0, 0, -1, 0, -1, 0, 0, 0, 0, 0, 0, 1;

  Eigen::Matrix4d pinnedTagTransform =
      idealLayout.GetTagPose(pinnedTagId)->ToMatrix();

  std::vector<frc::AprilTag> tags;
  for (const auto& [tagId, pose] : poses) {
    // Transformation from pinned tag
    Eigen::Matrix4d transform = Eigen::Matrix4d::Identity();

    transform.block<3, 3>(0, 0) = pose.q.toRotationMatrix();
    transform.block<3, 1>(0, 3) = pose.p;

    // Transformation from world
    Eigen::Matrix4d correctedTransform =
        pinnedTagTransform * correctionA * transform * correctionB;
    tags.emplace_back(tagId, frc::Pose3d{correctedTransform});
  }
  return frc::AprilTagFieldLayout{tags, idealLayout.GetFieldLength(),
                                  idealLayout.GetFieldWidth()};
}
