// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <fstream>
#include <optional>
#include <string>

#include <gtest/gtest.h>

#include "cameracalibration.hpp"
#include "fieldcalibration.hpp"
#include "path_lookup.hpp"
#include "wpi/apriltag/AprilTagFieldLayout.hpp"
#include "wpi/apriltag/AprilTagFields.hpp"
#include "wpi/util/json.hpp"

const std::string projectRootPath = PROJECT_ROOT_PATH;

const char* const tmpdir_c_str = std::getenv("TEST_TMPDIR");

const std::string calSavePath =
    tmpdir_c_str == nullptr
        ? (projectRootPath.substr(
               0, projectRootPath.find("/src/main/native/assets")) +
           "/build")
        : std::string(tmpdir_c_str);

#ifdef __linux__
const std::string fileSuffix = ".avi";
const std::string videoLocation = "/altfieldvideo";
#else
const std::string fileSuffix = ".mp4";
const std::string videoLocation = "/fieldvideo";
#endif

TEST(CameraCalibrationTest, Typical) {
  auto path = LookupPath(projectRootPath + "/testcalibration" + fileSuffix);
  auto calibrator = wpical::CameraCalibrator(4, 0.709, 0.551, 12, 8, path);
  while (!calibrator.IsFinished()) {
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
  }
  auto ret = calibrator.GetCameraModel();
  EXPECT_NE(ret, std::nullopt);
  std::ofstream output_file(calSavePath + "/cameracalibration.json");
  output_file << wpi::util::json(ret.value()).dump(4) << std::endl;
}

TEST(CameraCalibrationTest, Atypical) {
  auto path =
      LookupPath(projectRootPath + videoLocation + "/short" + fileSuffix);
  auto calibrator = wpical::CameraCalibrator(4, 0.709, 0.551, 12, 8, path);
  while (!calibrator.IsFinished()) {
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
  }
  auto ret = calibrator.GetCameraModel();
  EXPECT_EQ(ret, std::nullopt);
}

TEST(FieldCalibrationTest, Typical) {
  auto model = wpi::util::json::parse(
                   std::ifstream(calSavePath + "/cameracalibration.json"))
                   .get<wpical::CameraModel>();
  auto ret =
      wpical::calibrate(LookupPath(projectRootPath + videoLocation), model,
                        wpi::apriltag::AprilTagFieldLayout::LoadField(
                            wpi::apriltag::AprilTagField::k2024Crescendo),
                        3, false);
  EXPECT_NE(ret, std::nullopt);
}

TEST(FieldCalibrationTest, Atypical_Bad_Camera_Model) {
  wpical::CameraModel model{};
  auto ret =
      wpical::calibrate(LookupPath(projectRootPath + videoLocation), model,
                        wpi::apriltag::AprilTagFieldLayout::LoadField(
                            wpi::apriltag::AprilTagField::k2024Crescendo),
                        3, false);
  EXPECT_EQ(ret, std::nullopt);
}

TEST(FieldCalibrationTest, Atypical_Bad_Field_Layout) {
  auto model = wpi::util::json::parse(
                   std::ifstream(calSavePath + "/cameracalibration.json"))
                   .get<wpical::CameraModel>();
  auto ret =
      wpical::calibrate(LookupPath(projectRootPath + videoLocation), model,
                        wpi::apriltag::AprilTagFieldLayout{}, 3, false);
  EXPECT_EQ(ret, std::nullopt);
}

TEST(FieldCalibrationTest, Atypical_Bad_Input_Directory) {
  auto model = wpi::util::json::parse(
                   std::ifstream(calSavePath + "/cameracalibration.json"))
                   .get<wpical::CameraModel>();
  auto ret =
      wpical::calibrate(LookupPath(projectRootPath), model,
                        wpi::apriltag::AprilTagFieldLayout::LoadField(
                            wpi::apriltag::AprilTagField::k2024Crescendo),
                        3, false);
  EXPECT_EQ(ret, std::nullopt);
}

TEST(FieldCalibrationTest, Atypical_Bad_Pinned_Tag) {
  auto model = wpi::util::json::parse(
                   std::ifstream(calSavePath + "/cameracalibration.json"))
                   .get<wpical::CameraModel>();
  auto ret =
      wpical::calibrate(LookupPath(projectRootPath + videoLocation), model,
                        wpi::apriltag::AprilTagFieldLayout::LoadField(
                            wpi::apriltag::AprilTagField::k2024Crescendo),
                        42, false);
  EXPECT_EQ(ret, std::nullopt);
}

TEST(FieldCalibrationTest, Atypical_Bad_Pinned_Tag_Negative) {
  auto model = wpi::util::json::parse(
                   std::ifstream(calSavePath + "/cameracalibration.json"))
                   .get<wpical::CameraModel>();
  auto ret =
      wpical::calibrate(LookupPath(projectRootPath + videoLocation), model,
                        wpi::apriltag::AprilTagFieldLayout::LoadField(
                            wpi::apriltag::AprilTagField::k2024Crescendo),
                        -1, false);
  EXPECT_EQ(ret, std::nullopt);
}
