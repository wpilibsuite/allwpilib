// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <catch2/catch_test_macros.hpp>

#include "wpi/math/trajectory/DifferentialTrajectory.hpp"
#include "wpi/math/trajectory/HolonomicSample.hpp"
#include "wpi/math/trajectory/HolonomicTrajectory.hpp"
#include "wpi/math/trajectory/TestTrajectory.hpp"
#include "wpi/util/json.hpp"

using namespace wpi::math;

TEST_CASE("SampleJsonTest TestBaseSample", "[wpimath]") {
  TrajectoryConfig config{12_fps, 12_fps_sq};
  auto splineTrajectory = TestTrajectory::GetTrajectory(config);

  for (const auto& splineSample : splineTrajectory.Samples()) {
    // Convert DrivetrainSplineSample to HolonomicSample
    HolonomicSample sample{splineSample.time, splineSample.pose,
                           splineSample.velocity, splineSample.acceleration};

    wpi::util::json json;
    to_json(json, sample);

    HolonomicSample deserializedSample = json.get<HolonomicSample>();

    CHECK(sample.time == deserializedSample.time);
    CHECK(sample.pose == deserializedSample.pose);
    CHECK(sample.velocity == deserializedSample.velocity);
    CHECK(sample.acceleration == deserializedSample.acceleration);
  }
}

TEST_CASE("SampleJsonTest TestFromJson", "[wpimath]") {
  TrajectoryConfig config{12_fps, 12_fps_sq};
  auto splineTrajectory = TestTrajectory::GetTrajectory(config);

  for (const auto& splineSample : splineTrajectory.Samples()) {
    // Convert DrivetrainSplineSample to HolonomicSample
    HolonomicSample sample{splineSample.time, splineSample.pose,
                           splineSample.velocity, splineSample.acceleration};

    wpi::util::json json;
    to_json(json, sample);

    HolonomicSample deserializedSample = json.get<HolonomicSample>();

    CHECK(sample.time == deserializedSample.time);
    CHECK(sample.pose == deserializedSample.pose);
    CHECK(sample.velocity == deserializedSample.velocity);
    CHECK(sample.acceleration == deserializedSample.acceleration);
  }
}

TEST_CASE("SampleJsonTest TestDifferentialSamples", "[wpimath]") {
  TrajectoryConfig config{12_fps, 12_fps_sq};
  auto splineTrajectory = TestTrajectory::GetTrajectory(config);

  DifferentialDriveKinematics kinematics{0.5_m};

  for (const auto& splineSample : splineTrajectory.Samples()) {
    // Convert DrivetrainSplineSample to HolonomicSample
    HolonomicSample trajectorySample{splineSample.time, splineSample.pose,
                                     splineSample.velocity,
                                     splineSample.acceleration};
    // Convert HolonomicSample to DifferentialSample
    DifferentialSample sample{trajectorySample, kinematics};

    wpi::util::json json;
    to_json(json, sample);

    DifferentialSample deserializedSample = json.get<DifferentialSample>();

    CHECK(sample.time == deserializedSample.time);
    CHECK(sample.pose == deserializedSample.pose);
    CHECK(sample.velocity == deserializedSample.velocity);
    CHECK(sample.acceleration == deserializedSample.acceleration);
    CHECK(sample.leftVelocity == deserializedSample.leftVelocity);
    CHECK(sample.rightVelocity == deserializedSample.rightVelocity);
  }
}
