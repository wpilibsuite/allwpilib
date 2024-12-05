// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <cameracalibration.h>
#include <fieldcalibration.h>

#include <iostream>
#include <string>

#include <gtest/gtest.h>
#include <wpi/json.h>

const std::string projectRootPath = "C:\\Users\\matth\\Documents\\GitHub\\allwpilib\\wpical\\src\\main\\native\\resources";

TEST(Camera_CalibrationTest, OpenCV_Typical) {
  int ret = cameracalibration::calibrate(
      projectRootPath + "/testcalibration.mp4", 0.709f, 0.551f, 12, 8,
      true);
  EXPECT_EQ(ret, 0);
}

TEST(Camera_CalibrationTest, OpenCV_Atypical) {
  int ret = cameracalibration::calibrate(
      projectRootPath + "/fieldvideo/long.mp4", 0.709f, 0.551f, 12, 8,
      false);
  EXPECT_EQ(ret, 1);
}

TEST(Camera_CalibrationTest, MRcal_Typical) {
  int ret = cameracalibration::calibrate(
      projectRootPath + "/testcalibration.mp4", 0.709f, 12, 8, 1080, 1920,
      false);
  EXPECT_EQ(ret, 0);
}

TEST(Camera_CalibrationTest, MRcal_Atypical) {
  int ret = cameracalibration::calibrate(
      projectRootPath + "/fieldvideo/short.mp4", 0.709f, 12, 8, 1080,
      1920, false);
  EXPECT_EQ(ret, 1);
}

TEST(Field_CalibrationTest, Typical) {
  int ret = fieldcalibration::calibrate(
      projectRootPath + "/fieldvideo", projectRootPath + "",
      projectRootPath + "/camera calibration.json",
      projectRootPath + "/2024-crescendo.json", 3, 15, false);
  EXPECT_EQ(ret, 0);
}

TEST(Field_CalibrationTest, Atypical_Bad_Camera_Model_Directory) {
  int ret = fieldcalibration::calibrate(
      projectRootPath + "/fieldvideo", projectRootPath + "",
      projectRootPath + "/fieldvideo/long.mp4",
      projectRootPath + "/2024-crescendo.json", 3, 15, false);
  EXPECT_EQ(ret, 1);
}

TEST(Field_CalibrationTest, Atypical_Bad_Ideal_JSON) {
  int ret = fieldcalibration::calibrate(
      projectRootPath + "/fieldvideo", projectRootPath + "",
      projectRootPath + "/camera calibration.json",
      projectRootPath + "/camera calibration.json", 3, 15, false);
  EXPECT_EQ(ret, 1);
}

TEST(Field_CalibrationTest, Atypical_Bad_Input_Directory) {
  int ret = fieldcalibration::calibrate(
      projectRootPath + "", projectRootPath + "",
      projectRootPath + "/camera calibration.json",
      projectRootPath + "/2024-crescendo.json", 3, 15, false);
  EXPECT_EQ(ret, 1);
}
