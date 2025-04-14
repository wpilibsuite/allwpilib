// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <cameracalibration.h>
#include <fieldcalibration.h>

#include <fstream>
#include <optional>
#include <string>

#include <frc/apriltag/AprilTagFieldLayout.h>
#include <frc/apriltag/AprilTagFields.h>
#include <gtest/gtest.h>
#include <wpi/json.h>

const std::string projectRootPath = PROJECT_ROOT_PATH;
const std::string calSavePath =
    projectRootPath.substr(0, projectRootPath.find("/src/main/native/assets")) +
    "/build";

#ifdef __linux__
const std::string fileSuffix = ".avi";
const std::string videoLocation = "/altfieldvideo";
#else
const std::string fileSuffix = ".mp4";
const std::string videoLocation = "/fieldvideo";
#endif
TEST(Camera_CalibrationTest, OpenCV_Typical) {
  auto ret = cameracalibration::calibrate(
      projectRootPath + "/testcalibration" + fileSuffix, 0.709f, 0.551f, 12, 8,
      false);
  EXPECT_NE(ret, std::nullopt);
  std::ofstream output_file(calSavePath + "/cameracalibration.json");
  output_file << wpi::json(ret.value()).dump(4) << std::endl;
}

TEST(Camera_CalibrationTest, OpenCV_Atypical) {
  auto ret = cameracalibration::calibrate(
      projectRootPath + videoLocation + "/long" + fileSuffix, 0.709f, 0.551f,
      12, 8, false);
  EXPECT_EQ(ret, std::nullopt);
}

TEST(Camera_CalibrationTest, MRcal_Typical) {
  auto ret = cameracalibration::calibrate(
      projectRootPath + "/testcalibration" + fileSuffix, .709f, 12, 8, 1080,
      1920, false);
  EXPECT_NE(ret, std::nullopt);
  std::ofstream output_file(calSavePath + "/cameracalibration.json");
  output_file << wpi::json(ret.value()).dump(4) << std::endl;
}

TEST(Camera_CalibrationTest, MRcal_Atypical) {
  auto ret = cameracalibration::calibrate(
      projectRootPath + videoLocation + "/short" + fileSuffix, 0.709f, 12, 8,
      1080, 1920, false);
  EXPECT_EQ(ret, std::nullopt);
}

TEST(Field_CalibrationTest, Typical) {
  auto model =
      wpi::json::parse(std::ifstream(calSavePath + "/cameracalibration.json"))
          .get<cameracalibration::CameraModel>();
  auto ret = fieldcalibration::calibrate(
      projectRootPath + videoLocation, model,
      frc::AprilTagFieldLayout::LoadField(frc::AprilTagField::k2024Crescendo),
      3, false);
  EXPECT_NE(ret, std::nullopt);
}

TEST(Field_CalibrationTest, Atypical_Bad_Camera_Model) {
  cameracalibration::CameraModel model{};
  auto ret = fieldcalibration::calibrate(
      projectRootPath + videoLocation, model,
      frc::AprilTagFieldLayout::LoadField(frc::AprilTagField::k2024Crescendo),
      3, false);
  EXPECT_EQ(ret, std::nullopt);
}

TEST(Field_CalibrationTest, Atypical_Bad_Field_Layout) {
  auto model =
      wpi::json::parse(std::ifstream(calSavePath + "/cameracalibration.json"))
          .get<cameracalibration::CameraModel>();
  auto ret = fieldcalibration::calibrate(projectRootPath + videoLocation, model,
                                         frc::AprilTagFieldLayout{}, 3, false);
  EXPECT_EQ(ret, std::nullopt);
}

TEST(Field_CalibrationTest, Atypical_Bad_Input_Directory) {
  auto model =
      wpi::json::parse(std::ifstream(calSavePath + "/cameracalibration.json"))
          .get<cameracalibration::CameraModel>();
  auto ret = fieldcalibration::calibrate(
      projectRootPath, model,
      frc::AprilTagFieldLayout::LoadField(frc::AprilTagField::k2024Crescendo),
      3, false);
  EXPECT_EQ(ret, std::nullopt);
}

TEST(Field_CalibrationTest, Atypical_Bad_Pinned_Tag) {
  auto model =
      wpi::json::parse(std::ifstream(calSavePath + "/cameracalibration.json"))
          .get<cameracalibration::CameraModel>();
  auto ret = fieldcalibration::calibrate(
      projectRootPath + videoLocation, model,
      frc::AprilTagFieldLayout::LoadField(frc::AprilTagField::k2024Crescendo),
      42, false);
  EXPECT_EQ(ret, std::nullopt);
}

TEST(Field_CalibrationTest, Atypical_Bad_Pinned_Tag_Negative) {
  auto model =
      wpi::json::parse(std::ifstream(calSavePath + "/cameracalibration.json"))
          .get<cameracalibration::CameraModel>();
  auto ret = fieldcalibration::calibrate(
      projectRootPath + videoLocation, model,
      frc::AprilTagFieldLayout::LoadField(frc::AprilTagField::k2024Crescendo),
      -1, false);
  EXPECT_EQ(ret, std::nullopt);
}
