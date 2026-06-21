// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/math/interpolation/TimeInterpolatableBuffer.hpp"

#include <cmath>

#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>

#include "wpi/math/geometry/Pose2d.hpp"
#include "wpi/math/geometry/Rotation2d.hpp"
#include "wpi/units/time.hpp"

TEST_CASE("TimeInterpolatableBufferTest AddSample", "[wpimath]") {
  wpi::math::TimeInterpolatableBuffer<wpi::math::Rotation2d> buffer{10_s};

  // No entries
  buffer.AddSample(1_s, 0_rad);
  CHECK(buffer.Sample(1_s).value() == 0_rad);

  // New entry at start of container
  buffer.AddSample(0_s, 1_rad);
  CHECK(buffer.Sample(0_s).value() == 1_rad);

  // New entry in middle of container
  buffer.AddSample(0.5_s, 0.5_rad);
  CHECK(buffer.Sample(0.5_s).value() == 0.5_rad);

  // Override sample
  buffer.AddSample(0.5_s, 1_rad);
  CHECK(buffer.Sample(0.5_s).value() == 1_rad);
}

TEST_CASE("TimeInterpolatableBufferTest Interpolation", "[wpimath]") {
  wpi::math::TimeInterpolatableBuffer<wpi::math::Rotation2d> buffer{10_s};

  buffer.AddSample(0_s, 0_rad);
  CHECK(buffer.Sample(0_s).value() == 0_rad);
  buffer.AddSample(1_s, 1_rad);
  CHECK(buffer.Sample(0.5_s).value() == 0.5_rad);
  CHECK(buffer.Sample(1_s).value() == 1_rad);
  buffer.AddSample(3_s, 2_rad);
  CHECK(buffer.Sample(2_s).value() == 1.5_rad);

  buffer.AddSample(10.5_s, 2_rad);
  CHECK(buffer.Sample(0_s) == 1_rad);
}

TEST_CASE("TimeInterpolatableBufferTest Pose2d", "[wpimath]") {
  wpi::math::TimeInterpolatableBuffer<wpi::math::Pose2d> buffer{10_s};

  // We expect to be at (1 - 1/std::sqrt(2), 1/std::sqrt(2), 45deg) at t=0.5
  buffer.AddSample(0_s, wpi::math::Pose2d{0_m, 0_m, 90_deg});
  buffer.AddSample(1_s, wpi::math::Pose2d{1_m, 1_m, 0_deg});
  wpi::math::Pose2d sample = buffer.Sample(0.5_s).value();

  CHECK(std::abs(sample.X().value() - (1.0 - 1.0 / std::sqrt(2.0))) < 0.01);
  CHECK(std::abs(sample.Y().value() - (1.0 / std::sqrt(2.0))) < 0.01);
  CHECK(std::abs(sample.Rotation().Degrees().value() - 45.0) < 0.01);
}
