// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <catch2/catch_test_macros.hpp>
#include <opencv2/core/mat.hpp>

#include "mrcal_wrapper.hpp"

// TODO add end-to-end calibration tests. tools/wpical currently covers that via
// test_result_is_exact.cpp; those cases should move here once the corner-cache
// fixtures are shared.

TEST_CASE("undistort_mrcal rejects a bad point array", "[mrcal]") {
  cv::Mat cameraMat{{3, 3},
                    {1000.0, 0.0, 640.0, 0.0, 1000.0, 360.0, 0.0, 0.0, 1.0}};
  cv::Mat distCoeffs = cv::Mat::zeros(8, 1, CV_64FC1);

  SECTION("wrong column count") {
    cv::Mat points = cv::Mat::zeros(4, 3, CV_64FC2);
    CHECK_FALSE(undistort_mrcal(&points, &cameraMat, &distCoeffs,
                                CameraLensModel::LENSMODEL_OPENCV8, 0, 0, 0,
                                0));
  }

  SECTION("wrong element type") {
    cv::Mat points = cv::Mat::zeros(4, 2, CV_32FC2);
    CHECK_FALSE(undistort_mrcal(&points, &cameraMat, &distCoeffs,
                                CameraLensModel::LENSMODEL_OPENCV8, 0, 0, 0,
                                0));
  }
}
