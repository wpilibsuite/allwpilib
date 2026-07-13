// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <chrono>
#include <cstdlib>
#include <optional>
#include <string>
#include <system_error>
#include <thread>

#include <catch2/catch_test_macros.hpp>

#include "cameracalibration.hpp"
#include "fieldcalibration.hpp"
#include "path_lookup.hpp"
#include "wpi/apriltag/AprilTagFieldLayout.hpp"
#include "wpi/apriltag/AprilTagFields.hpp"
#include "wpi/util/fs.hpp"
#include "wpi/util/json.hpp"
#include "wpi/util/raw_ostream.hpp"

namespace {

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

wpical::CameraModel GetCameraModel() {
  static std::optional<wpical::CameraModel> cameraModel;
  if (cameraModel) {
    return *cameraModel;
  }

  auto path = LookupPath(projectRootPath + "/testcalibration" + fileSuffix);
  auto calibrator = wpical::CameraCalibrator(4, 0.709, 0.551, 12, 8, path);
  while (!calibrator.IsFinished()) {
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
  }
  auto ret = calibrator.GetCameraModel();
  REQUIRE(ret != std::nullopt);
  cameraModel = *ret;
  return *cameraModel;
}

}  // namespace

TEST_CASE("CameraCalibrationTest Typical", "[wpical]") {
  auto ret = GetCameraModel();
  std::error_code ec;
  wpi::util::raw_fd_ostream output_file(calSavePath + "/cameracalibration.json",
                                        ec, fs::OF_Text);
  wpi::util::json(ret).marshal(output_file, true);
}

TEST_CASE("CameraCalibrationTest Atypical", "[wpical]") {
  auto path =
      LookupPath(projectRootPath + videoLocation + "/short" + fileSuffix);
  auto calibrator = wpical::CameraCalibrator(4, 0.709, 0.551, 12, 8, path);
  while (!calibrator.IsFinished()) {
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
  }
  auto ret = calibrator.GetCameraModel();
  CHECK(ret == std::nullopt);
}

TEST_CASE("FieldCalibrationTest Typical", "[wpical]") {
  auto model = GetCameraModel();
  auto ret =
      wpical::calibrate(LookupPath(projectRootPath + videoLocation), model,
                        wpi::apriltag::AprilTagFieldLayout::LoadField(
                            wpi::apriltag::AprilTagField::k2024Crescendo),
                        3, false);
  REQUIRE(ret != std::nullopt);
}

TEST_CASE("FieldCalibrationTest Atypical_Bad_Camera_Model", "[wpical]") {
  wpical::CameraModel model{};
  auto ret =
      wpical::calibrate(LookupPath(projectRootPath + videoLocation), model,
                        wpi::apriltag::AprilTagFieldLayout::LoadField(
                            wpi::apriltag::AprilTagField::k2024Crescendo),
                        3, false);
  CHECK(ret == std::nullopt);
}

TEST_CASE("FieldCalibrationTest Atypical_Bad_Field_Layout", "[wpical]") {
  auto model = GetCameraModel();
  auto ret =
      wpical::calibrate(LookupPath(projectRootPath + videoLocation), model,
                        wpi::apriltag::AprilTagFieldLayout{}, 3, false);
  CHECK(ret == std::nullopt);
}

TEST_CASE("FieldCalibrationTest Atypical_Bad_Input_Directory", "[wpical]") {
  auto model = GetCameraModel();
  auto ret =
      wpical::calibrate(LookupPath(projectRootPath), model,
                        wpi::apriltag::AprilTagFieldLayout::LoadField(
                            wpi::apriltag::AprilTagField::k2024Crescendo),
                        3, false);
  CHECK(ret == std::nullopt);
}

TEST_CASE("FieldCalibrationTest Atypical_Bad_Pinned_Tag", "[wpical]") {
  auto model = GetCameraModel();
  auto ret =
      wpical::calibrate(LookupPath(projectRootPath + videoLocation), model,
                        wpi::apriltag::AprilTagFieldLayout::LoadField(
                            wpi::apriltag::AprilTagField::k2024Crescendo),
                        42, false);
  CHECK(ret == std::nullopt);
}

TEST_CASE("FieldCalibrationTest Atypical_Bad_Pinned_Tag_Negative", "[wpical]") {
  auto model = GetCameraModel();
  auto ret =
      wpical::calibrate(LookupPath(projectRootPath + videoLocation), model,
                        wpi::apriltag::AprilTagFieldLayout::LoadField(
                            wpi::apriltag::AprilTagField::k2024Crescendo),
                        -1, false);
  CHECK(ret == std::nullopt);
}
