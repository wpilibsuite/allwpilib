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

TEST(CameraCalibrationTest, Typical) {
  auto calibrator = wpical::CameraCalibrator(4, 0.709, 0.551, 12, 8);
  std::string path = projectRootPath + "/testcalibration" + fileSuffix;
  calibrator.ProcessVideo(path);
  while (!calibrator.IsFinished()) {
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
  }
  auto ret = calibrator.GetCameraModel();
  EXPECT_NE(ret, std::nullopt);
  std::ofstream output_file(calSavePath + "/cameracalibration.json");
  output_file << wpi::json(ret.value()).dump(4) << std::endl;
}

TEST(CameraCalibrationTest, Atypical) {
  auto calibrator = wpical::CameraCalibrator(4, 0.709, 0.551, 12, 8);
  std::string path = projectRootPath + videoLocation + "/short" + fileSuffix;
  calibrator.ProcessVideo(path);
  while (!calibrator.IsFinished()) {
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
  }
  auto ret = calibrator.GetCameraModel();
  EXPECT_EQ(ret, std::nullopt);
}

TEST(FieldCalibrationTest, Typical) {
  auto model =
      wpi::json::parse(std::ifstream(calSavePath + "/cameracalibration.json"))
          .get<wpical::CameraModel>();
  auto ret = wpical::calibrate(
      projectRootPath + videoLocation, model,
      frc::AprilTagFieldLayout::LoadField(frc::AprilTagField::k2024Crescendo),
      3, false);
  EXPECT_NE(ret, std::nullopt);
}

TEST(FieldCalibrationTest, Atypical_Bad_Camera_Model) {
  wpical::CameraModel model{};
  auto ret = wpical::calibrate(
      projectRootPath + videoLocation, model,
      frc::AprilTagFieldLayout::LoadField(frc::AprilTagField::k2024Crescendo),
      3, false);
  EXPECT_EQ(ret, std::nullopt);
}

TEST(FieldCalibrationTest, Atypical_Bad_Field_Layout) {
  auto model =
      wpi::json::parse(std::ifstream(calSavePath + "/cameracalibration.json"))
          .get<wpical::CameraModel>();
  auto ret = wpical::calibrate(projectRootPath + videoLocation, model,
                               frc::AprilTagFieldLayout{}, 3, false);
  EXPECT_EQ(ret, std::nullopt);
}

TEST(FieldCalibrationTest, Atypical_Bad_Input_Directory) {
  auto model =
      wpi::json::parse(std::ifstream(calSavePath + "/cameracalibration.json"))
          .get<wpical::CameraModel>();
  auto ret = wpical::calibrate(
      projectRootPath, model,
      frc::AprilTagFieldLayout::LoadField(frc::AprilTagField::k2024Crescendo),
      3, false);
  EXPECT_EQ(ret, std::nullopt);
}

TEST(FieldCalibrationTest, Atypical_Bad_Pinned_Tag) {
  auto model =
      wpi::json::parse(std::ifstream(calSavePath + "/cameracalibration.json"))
          .get<wpical::CameraModel>();
  auto ret = wpical::calibrate(
      projectRootPath + videoLocation, model,
      frc::AprilTagFieldLayout::LoadField(frc::AprilTagField::k2024Crescendo),
      42, false);
  EXPECT_EQ(ret, std::nullopt);
}

TEST(FieldCalibrationTest, Atypical_Bad_Pinned_Tag_Negative) {
  auto model =
      wpi::json::parse(std::ifstream(calSavePath + "/cameracalibration.json"))
          .get<wpical::CameraModel>();
  auto ret = wpical::calibrate(
      projectRootPath + videoLocation, model,
      frc::AprilTagFieldLayout::LoadField(frc::AprilTagField::k2024Crescendo),
      -1, false);
  EXPECT_EQ(ret, std::nullopt);
}
