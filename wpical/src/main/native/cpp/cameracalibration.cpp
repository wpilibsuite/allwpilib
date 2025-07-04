// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "cameracalibration.h"

#include <fstream>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

#include <Eigen/Core>
#include <mrcal_wrapper.h>
#include <opencv2/core/eigen.hpp>
#include <opencv2/objdetect/aruco_board.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/videoio.hpp>

cameracalibration::CameraModel cameracalibration::MrcalResultToCameraModel(
    mrcal_result& stats) {
  // Camera matrix and distortion coefficients
  std::vector<double> cameraMatrix = {
      // fx 0 cx
      stats.intrinsics[0], 0, stats.intrinsics[2],
      // 0 fy cy
      0, stats.intrinsics[1], stats.intrinsics[3],
      // 0 0 1
      0, 0, 1};

  std::vector<double> distortionCoefficients = {
      stats.intrinsics[4],  stats.intrinsics[5], stats.intrinsics[6],
      stats.intrinsics[7],  stats.intrinsics[8], stats.intrinsics[9],
      stats.intrinsics[10], stats.intrinsics[11]};

  return {Eigen::Matrix<double, 3, 3>(cameraMatrix.data()),
          Eigen::Matrix<double, 8, 1>(distortionCoefficients.data()),
          stats.rms_error};
}

void cameracalibration::to_json(wpi::json& json,
                                const CameraModel& cameraModel) {
  std::vector<double> cameraMatrix(
      cameraModel.intrinsicMatrix.data(),
      cameraModel.intrinsicMatrix.data() + cameraModel.intrinsicMatrix.size());
  std::vector<double> distortionCoefficients(
      cameraModel.distortionCoefficients.data(),
      cameraModel.distortionCoefficients.data() +
          cameraModel.distortionCoefficients.size());
  json = {{"camera_matrix", cameraMatrix},
          {"distortion_coefficients", distortionCoefficients},
          {"avg_reprojection_error", cameraModel.avgReprojectionError}};
}

void cameracalibration::from_json(const wpi::json& json,
                                  CameraModel& cameraModel) {
  bool isCalibdb = json.contains("camera");
  Eigen::Matrix3d cameraMatrix;
  std::vector<double> distortionCoeffs;
  auto mat = json.at("camera_matrix");
  auto distortionCoefficients = json.at("distortion_coefficients");
  if (isCalibdb) {
    // OpenCV format has data key
    if (mat.contains("data")) {
      auto data = mat.at("data").get<std::vector<double>>();
      cameraMatrix = Eigen::Matrix<double, 3, 3, Eigen::RowMajor>{data.data()};
    } else {
      for (int i = 0; i < cameraMatrix.rows(); i++) {
        for (int j = 0; j < cameraMatrix.cols(); j++) {
          cameraMatrix(i, j) = mat[i][j];
        }
      }
    }

    // OpenCV format has data key
    if (distortionCoefficients.contains("data")) {
      distortionCoeffs =
          distortionCoefficients.at("data").get<std::vector<double>>();
    } else {
      distortionCoeffs = distortionCoefficients.get<std::vector<double>>();
    }
  } else {
    cameraMatrix = Eigen::Matrix<double, 3, 3, Eigen::RowMajor>{
        mat.get<std::vector<double>>().data()};
    distortionCoeffs = distortionCoefficients.get<std::vector<double>>();
  }
  // CalibDB generates JSONs with 5 values. Just zero out the remaining 3 to get
  // it to 8
  distortionCoeffs.resize(8, 0);
  cameraModel = {cameraMatrix,
                 Eigen::Matrix<double, 8, 1>{distortionCoeffs.data()},
                 json.at("avg_reprojection_error")};
}
