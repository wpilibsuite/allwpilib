// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/apriltag/AprilTagDetector.hpp"

#include <catch2/catch_test_macros.hpp>

using namespace wpi::apriltag;

TEST_CASE("AprilTagDetectorTest ConfigDefaults", "[apriltag][detector]") {
  AprilTagDetector detector;
  auto config = detector.GetConfig();
  REQUIRE(config == AprilTagDetector::Config{});
}

TEST_CASE("AprilTagDetectorTest QtpDefaults", "[apriltag][detector]") {
  AprilTagDetector detector;
  auto params = detector.GetQuadThresholdParameters();
  REQUIRE(params == AprilTagDetector::QuadThresholdParameters{});
}

TEST_CASE("AprilTagDetectorTest SetConfigNumThreads", "[apriltag][detector]") {
  AprilTagDetector detector;
  detector.SetConfig({.numThreads = 2});
  auto config = detector.GetConfig();
  REQUIRE(config.numThreads == 2);
}

TEST_CASE("AprilTagDetectorTest QtpMinClusterPixels", "[apriltag][detector]") {
  AprilTagDetector detector;
  detector.SetQuadThresholdParameters({.minClusterPixels = 8});
  auto params = detector.GetQuadThresholdParameters();
  REQUIRE(params.minClusterPixels == 8);
}

TEST_CASE("AprilTagDetectorTest Add16h5", "[apriltag][detector]") {
  AprilTagDetector detector;
  REQUIRE(detector.AddFamily("tag16h5"));
  // duplicate addition is also okay
  REQUIRE(detector.AddFamily("tag16h5"));
}

TEST_CASE("AprilTagDetectorTest Add36h11", "[apriltag][detector]") {
  AprilTagDetector detector;
  REQUIRE(detector.AddFamily("tag36h11"));
}

TEST_CASE("AprilTagDetectorTest AddMultiple", "[apriltag][detector]") {
  AprilTagDetector detector;
  REQUIRE(detector.AddFamily("tag16h5"));
  REQUIRE(detector.AddFamily("tag36h11"));
}

TEST_CASE("AprilTagDetectorTest RemoveFamily", "[apriltag][detector]") {
  AprilTagDetector detector;
  // okay to remove non-existent family
  detector.RemoveFamily("tag16h5");

  // add and remove
  detector.AddFamily("tag16h5");
  detector.RemoveFamily("tag16h5");
}
