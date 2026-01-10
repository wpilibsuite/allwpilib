// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <gtest/gtest.h>

#include "wpi/math/trajectory/DifferentialTrajectory.hpp"
#include "wpi/math/trajectory/HolonomicTrajectory.hpp"
#include "wpi/math/trajectory/TestTrajectory.hpp"
#include "wpi/math/trajectory/TrajectorySample.hpp"
#include "wpi/util/json.hpp"

using namespace wpi::math;

TEST(SampleJsonTest, TestBaseSample) {
  TrajectoryConfig config{12_fps, 12_fps_sq};
  auto splineTrajectory = TestTrajectory::GetTrajectory(config);

  for (const auto& splineSample : splineTrajectory.Samples()) {
    // Convert SplineSample to TrajectorySample
    TrajectorySample sample{splineSample.timestamp, splineSample.pose,
                            splineSample.velocity, splineSample.acceleration};

    wpi::util::json json;
    to_json(json, sample);

    TrajectorySample deserializedSample = json.get<TrajectorySample>();

    EXPECT_EQ(sample.timestamp, deserializedSample.timestamp);
    EXPECT_EQ(sample.pose, deserializedSample.pose);
    EXPECT_EQ(sample.velocity, deserializedSample.velocity);
    EXPECT_EQ(sample.acceleration, deserializedSample.acceleration);
  }
}

TEST(SampleJsonTest, TestFromJson) {
  TrajectoryConfig config{12_fps, 12_fps_sq};
  auto splineTrajectory = TestTrajectory::GetTrajectory(config);

  for (const auto& splineSample : splineTrajectory.Samples()) {
    // Convert SplineSample to TrajectorySample
    TrajectorySample sample{splineSample.timestamp, splineSample.pose,
                            splineSample.velocity, splineSample.acceleration};

    wpi::util::json json;
    to_json(json, sample);

    TrajectorySample deserializedSample = json.get<TrajectorySample>();

    EXPECT_EQ(sample.timestamp, deserializedSample.timestamp);
    EXPECT_EQ(sample.pose, deserializedSample.pose);
    EXPECT_EQ(sample.velocity, deserializedSample.velocity);
    EXPECT_EQ(sample.acceleration, deserializedSample.acceleration);
  }
}

TEST(SampleJsonTest, TestDifferentialSamples) {
  TrajectoryConfig config{12_fps, 12_fps_sq};
  auto splineTrajectory = TestTrajectory::GetTrajectory(config);

  DifferentialDriveKinematics kinematics{0.5_m};

  for (const auto& splineSample : splineTrajectory.Samples()) {
    // Convert SplineSample to TrajectorySample
    TrajectorySample trajectorySample{splineSample.timestamp, splineSample.pose,
                                      splineSample.velocity,
                                      splineSample.acceleration};
    // Convert TrajectorySample to DifferentialSample
    DifferentialSample sample{trajectorySample, kinematics};

    wpi::util::json json;
    to_json(json, sample);

    DifferentialSample deserializedSample = json.get<DifferentialSample>();

    EXPECT_EQ(sample.timestamp, deserializedSample.timestamp);
    EXPECT_EQ(sample.pose, deserializedSample.pose);
    EXPECT_EQ(sample.velocity, deserializedSample.velocity);
    EXPECT_EQ(sample.acceleration, deserializedSample.acceleration);
    EXPECT_EQ(sample.leftSpeed, deserializedSample.leftSpeed);
    EXPECT_EQ(sample.rightSpeed, deserializedSample.rightSpeed);
  }
}
