// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <gtest/gtest.h>

#include "frc/apriltag/AprilTagDetector.h"

using namespace frc;

TEST(AprilTagDetectorTest, ConfigDefaults) {
  AprilTagDetector detector;
  auto config = detector.GetConfig();
  ASSERT_EQ(config, AprilTagDetector::Config{});
}

TEST(AprilTagDetectorTest, QtpDefaults) {
  AprilTagDetector detector;
  auto params = detector.GetQuadThresholdParameters();
  ASSERT_EQ(params, AprilTagDetector::QuadThresholdParameters{});
}

TEST(AprilTagDetectorTest, SetConfigNumThreads) {
  AprilTagDetector detector;
  detector.SetConfig({.numThreads = 2});
  auto config = detector.GetConfig();
  ASSERT_EQ(config.numThreads, 2);
}

TEST(AprilTagDetectorTest, QtpMinClusterPixels) {
  AprilTagDetector detector;
  detector.SetQuadThresholdParameters({.minClusterPixels = 8});
  auto params = detector.GetQuadThresholdParameters();
  ASSERT_EQ(params.minClusterPixels, 8);
}

TEST(AprilTagDetectorTest, Add16h5) {
  AprilTagDetector detector;
  ASSERT_TRUE(detector.AddFamily("tag16h5"));
  // duplicate addition is also okay
  ASSERT_TRUE(detector.AddFamily("tag16h5"));
}

TEST(AprilTagDetectorTest, Add36h11) {
  AprilTagDetector detector;
  ASSERT_TRUE(detector.AddFamily("tag36h11"));
}

TEST(AprilTagDetectorTest, AddMultiple) {
  AprilTagDetector detector;
  ASSERT_TRUE(detector.AddFamily("tag16h5"));
  ASSERT_TRUE(detector.AddFamily("tag36h11"));
}

TEST(AprilTagDetectorTest, RemoveFamily) {
  AprilTagDetector detector;
  // okay to remove non-existent family
  detector.RemoveFamily("tag16h5");

  // add and remove
  detector.AddFamily("tag16h5");
  detector.RemoveFamily("tag16h5");
}
