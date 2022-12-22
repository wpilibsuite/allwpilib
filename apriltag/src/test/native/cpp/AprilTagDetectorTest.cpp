// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/apriltag/AprilTagDetector.h"
#include "gtest/gtest.h"

using namespace frc;

TEST(AprilTagDetectorTest, NumThreads) {
  AprilTagDetector detector;
  ASSERT_EQ(detector.GetNumThreads(), 1);
  detector.SetNumThreads(2);
  ASSERT_EQ(detector.GetNumThreads(), 2);
}

TEST(AprilTagDetectorTest, Add16h5) {
  AprilTagDetector detector;
  ASSERT_TRUE(detector.AddFamily("tag16h5"));
  // duplicate addition is also okay
  ASSERT_TRUE(detector.AddFamily("tag16h5"));
}

TEST(AprilTagDetectorTest, Add25h9) {
  AprilTagDetector detector;
  ASSERT_TRUE(detector.AddFamily("tag25h9"));
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
