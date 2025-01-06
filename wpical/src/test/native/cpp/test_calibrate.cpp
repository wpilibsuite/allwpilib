// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <cameracalibration.h>
#include <fieldcalibration.h>

#include <string>

#include <gtest/gtest.h>
#include <wpi/json.h>

const std::string projectRootPath = PROJECT_ROOT_PATH;
const std::string calSavePath =
    projectRootPath.substr(0,
                           projectRootPath.find("/src/main/native/resources")) +
    "/build";
cameracalibration::CameraModel cameraModel = {
    .intrinsic_matrix = Eigen::Matrix<double, 3, 3>::Identity(),
    .distortion_coefficients = Eigen::Matrix<double, 8, 1>::Zero(),
    .avg_reprojection_error = 0.0};

wpi::json output_json;

#ifdef __linux__
const std::string fileSuffix = ".avi";
const std::string videoLocation = "/altfieldvideo";
#else
const std::string fileSuffix = ".mp4";
const std::string videoLocation = "/fieldvideo";
#endif
TEST(Camera_CalibrationTest, OpenCV_Typical) {
  int ret = cameracalibration::calibrate(
      projectRootPath + "/testcalibration" + fileSuffix, cameraModel, 0.709f,
      0.551f, 12, 8, false);
  cameracalibration::dumpJson(cameraModel,
                              calSavePath + "/cameracalibration.json");
  EXPECT_EQ(ret, 0);
}

TEST(Camera_CalibrationTest, OpenCV_Atypical) {
  int ret = cameracalibration::calibrate(
      projectRootPath + videoLocation + "/long" + fileSuffix, cameraModel,
      0.709f, 0.551f, 12, 8, false);
  EXPECT_EQ(ret, 1);
}

TEST(Camera_CalibrationTest, MRcal_Typical) {
  int ret = cameracalibration::calibrate(
      projectRootPath + "/testcalibration" + fileSuffix, cameraModel, .709f, 12,
      8, 1080, 1920, false);
  EXPECT_EQ(ret, 0);
}

TEST(Camera_CalibrationTest, MRcal_Atypical) {
  int ret = cameracalibration::calibrate(
      projectRootPath + videoLocation + "/short" + fileSuffix, cameraModel,
      0.709f, 12, 8, 1080, 1920, false);
  EXPECT_EQ(ret, 1);
}

TEST(Field_CalibrationTest, Typical) {
  int ret = fieldcalibration::calibrate(
      projectRootPath + videoLocation, output_json,
      calSavePath + "/cameracalibration.json",
      projectRootPath + "/2024-crescendo.json", 3, false);
  EXPECT_EQ(ret, 0);
}

TEST(Field_CalibrationTest, Atypical_Bad_Camera_Model_Directory) {
  int ret = fieldcalibration::calibrate(
      projectRootPath + videoLocation, output_json,
      projectRootPath + videoLocation + "/long" + fileSuffix,
      projectRootPath + "/2024-crescendo.json", 3, false);
  EXPECT_EQ(ret, 1);
}

TEST(Field_CalibrationTest, Atypical_Bad_Ideal_JSON) {
  int ret = fieldcalibration::calibrate(
      projectRootPath + videoLocation, output_json,
      calSavePath + "/cameracalibration.json",
      calSavePath + "/cameracalibration.json", 3, false);
  EXPECT_EQ(ret, 1);
}

TEST(Field_CalibrationTest, Atypical_Bad_Input_Directory) {
  int ret = fieldcalibration::calibrate(
      projectRootPath + "", output_json,
      calSavePath + "/cameracalibration.json",
      projectRootPath + "/2024-crescendo.json", 3, false);
  EXPECT_EQ(ret, 1);
}

TEST(Field_CalibrationTest, Atypical_Bad_Pinned_Tag) {
  int ret = fieldcalibration::calibrate(
      projectRootPath + videoLocation, output_json,
      calSavePath + "/cameracalibration.json",
      projectRootPath + "/2024-crescendo.json", 42, false);
  EXPECT_EQ(ret, 1);
}

TEST(Field_CalibrationTest, Atypical_Bad_Pinned_Tag_Negative) {
  int ret = fieldcalibration::calibrate(
      projectRootPath + videoLocation, output_json,
      calSavePath + "/cameracalibration.json",
      projectRootPath + "/2024-crescendo.json", -1, false);
  EXPECT_EQ(ret, 1);
}
