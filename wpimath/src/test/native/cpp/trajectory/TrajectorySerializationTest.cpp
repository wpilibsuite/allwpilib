// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <utility>
#include <vector>

#include <gtest/gtest.h>

#include "wpi/math/trajectory/DifferentialTrajectory.hpp"
#include "wpi/math/trajectory/HolonomicTrajectory.hpp"
#include "wpi/math/trajectory/TestTrajectory.hpp"
#include "wpi/util/json.hpp"

using namespace wpi::math;

TEST(TrajectorySerializationTest, TestJsonSerialization) {
  TrajectoryConfig config{12_fps, 12_fps_sq};
  auto splineTrajectory = TestTrajectory::GetTrajectory(config);

  // Convert SplineTrajectory to HolonomicTrajectory
  std::vector<TrajectorySample> samples;
  for (const auto& splineSample : splineTrajectory.Samples()) {
    samples.emplace_back(splineSample.timestamp, splineSample.pose,
                         splineSample.velocity, splineSample.acceleration);
  }
  HolonomicTrajectory trajectory{std::move(samples)};

  // Serialize to JSON
  wpi::util::json json;
  to_json(json, trajectory);

  // Deserialize from JSON
  auto samples_out = json.at("samples").get<std::vector<TrajectorySample>>();
  HolonomicTrajectory deserializedTrajectory{std::move(samples_out)};

  // Verify they are equal
  EXPECT_EQ(trajectory.TotalTime(), deserializedTrajectory.TotalTime());
  EXPECT_EQ(trajectory.Samples().size(),
            deserializedTrajectory.Samples().size());

  for (size_t i = 0; i < trajectory.Samples().size(); ++i) {
    const auto& original = trajectory.Samples()[i];
    const auto& deserialized = deserializedTrajectory.Samples()[i];

    EXPECT_EQ(original.timestamp, deserialized.timestamp);
    EXPECT_EQ(original.pose, deserialized.pose);
    EXPECT_EQ(original.velocity, deserialized.velocity);
    EXPECT_EQ(original.acceleration, deserialized.acceleration);
  }
}

TEST(TrajectorySerializationTest, TestDifferentialSerialization) {
  TrajectoryConfig config{12_fps, 12_fps_sq};
  auto splineTrajectory = TestTrajectory::GetTrajectory(config);

  DifferentialDriveKinematics kinematics{0.5_m};
  std::vector<DifferentialSample> samples;
  for (const auto& splineSample : splineTrajectory.Samples()) {
    TrajectorySample trajectorySample{splineSample.timestamp, splineSample.pose,
                                      splineSample.velocity,
                                      splineSample.acceleration};
    samples.emplace_back(trajectorySample, kinematics);
  }

  DifferentialTrajectory trajectory{std::move(samples)};

  // Serialize to JSON
  wpi::util::json json;
  to_json(json, trajectory);

  // Deserialize from JSON
  auto samples_out = json.at("samples").get<std::vector<DifferentialSample>>();
  DifferentialTrajectory deserializedTrajectory{std::move(samples_out)};

  // Verify they are equal
  EXPECT_EQ(trajectory.TotalTime(), deserializedTrajectory.TotalTime());
  EXPECT_EQ(trajectory.Samples().size(),
            deserializedTrajectory.Samples().size());

  for (size_t i = 0; i < trajectory.Samples().size(); ++i) {
    const auto& original = trajectory.Samples()[i];
    const auto& deserialized = deserializedTrajectory.Samples()[i];

    EXPECT_EQ(original.timestamp, deserialized.timestamp);
    EXPECT_EQ(original.pose, deserialized.pose);
    EXPECT_EQ(original.velocity, deserialized.velocity);
    EXPECT_EQ(original.acceleration, deserialized.acceleration);
    EXPECT_EQ(original.leftSpeed, deserialized.leftSpeed);
    EXPECT_EQ(original.rightSpeed, deserialized.rightSpeed);
  }
}
