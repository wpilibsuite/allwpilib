// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <string>

#include <gtest/gtest.h>

#include "cameracalibration.hpp"
#include "fieldcalibration.hpp"
#include "path_lookup.hpp"
#include "wpi/util/json.hpp"

const std::string projectRootPath = PROJECT_ROOT_PATH;

const char* const tmpdir_c_str = std::getenv("TEST_TMPDIR");

const std::string calSavePath =
    tmpdir_c_str == nullptr
        ? (projectRootPath.substr(
               0, projectRootPath.find("/src/main/native/assets")) +
           "/build")
        : std::string(tmpdir_c_str);
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
      LookupPath(projectRootPath + "/testcalibration" + fileSuffix),
      cameraModel, 0.709f, 0.551f, 12, 8, false);
  cameracalibration::dumpJson(cameraModel,
                              calSavePath + "/cameracalibration.json");
  EXPECT_EQ(ret, 0);
}

TEST(Camera_CalibrationTest, OpenCV_Atypical) {
  int ret = cameracalibration::calibrate(
      LookupPath(projectRootPath + videoLocation + "/long" + fileSuffix),
      cameraModel, 0.709f, 0.551f, 12, 8, false);
  EXPECT_EQ(ret, 1);
}

TEST(Camera_CalibrationTest, MRcal_Typical) {
  int ret = cameracalibration::calibrate(
      LookupPath(projectRootPath + "/testcalibration" + fileSuffix),
      cameraModel, .709f, 12, 8, 1080, 1920, false);
  EXPECT_EQ(ret, 0);
}

TEST(Camera_CalibrationTest, MRcal_Atypical) {
  int ret = cameracalibration::calibrate(
      LookupPath(projectRootPath + videoLocation + "/short" + fileSuffix),
      cameraModel, 0.709f, 12, 8, 1080, 1920, false);
  EXPECT_EQ(ret, 1);
}

TEST(Field_CalibrationTest, Typical) {
  int ret = fieldcalibration::calibrate(
      LookupPath(projectRootPath + videoLocation), output_json,
      calSavePath + "/cameracalibration.json",
      LookupPath(projectRootPath + "/2024-crescendo.json"), 3, false);
  EXPECT_EQ(ret, 0);
}

TEST(Field_CalibrationTest, Atypical_Bad_Camera_Model_Directory) {
  int ret = fieldcalibration::calibrate(
      LookupPath(projectRootPath + videoLocation), output_json,
      LookupPath(projectRootPath + videoLocation + "/long" + fileSuffix),
      LookupPath(projectRootPath + "/2024-crescendo.json"), 3, false);
  EXPECT_EQ(ret, 1);
}

TEST(Field_CalibrationTest, Atypical_Bad_Ideal_JSON) {
  int ret = fieldcalibration::calibrate(
      LookupPath(projectRootPath + videoLocation), output_json,
      calSavePath + "/cameracalibration.json",
      calSavePath + "/cameracalibration.json", 3, false);
  EXPECT_EQ(ret, 1);
}

TEST(Field_CalibrationTest, Atypical_Bad_Input_Directory) {
  int ret = fieldcalibration::calibrate(
      LookupPath(projectRootPath + ""), output_json,
      calSavePath + "/cameracalibration.json",
      LookupPath(projectRootPath + "/2024-crescendo.json"), 3, false);
  EXPECT_EQ(ret, 1);
}

TEST(Field_CalibrationTest, Atypical_Bad_Pinned_Tag) {
  int ret = fieldcalibration::calibrate(
      LookupPath(projectRootPath + videoLocation), output_json,
      calSavePath + "/cameracalibration.json",
      LookupPath(projectRootPath + "/2024-crescendo.json"), 42, false);
  EXPECT_EQ(ret, 1);
}

TEST(Field_CalibrationTest, Atypical_Bad_Pinned_Tag_Negative) {
  int ret = fieldcalibration::calibrate(
      LookupPath(projectRootPath + videoLocation), output_json,
      calSavePath + "/cameracalibration.json",
      LookupPath(projectRootPath + "/2024-crescendo.json"), -1, false);
  EXPECT_EQ(ret, 1);
}
