// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <cmath>

#include <gtest/gtest.h>

#include "units/time.h"
#include "wpi/math/geometry/Pose2d.h"
#include "wpi/math/geometry/Rotation2d.h"
#include "wpi/math/interpolation/TimeInterpolatableBuffer.h"

TEST(TimeInterpolatableBufferTest, AddSample) {
  wpi::math::TimeInterpolatableBuffer<wpi::math::Rotation2d> buffer{10_s};

  // No entries
  buffer.AddSample(1_s, 0_rad);
  EXPECT_TRUE(buffer.Sample(1_s).value() == 0_rad);

  // New entry at start of container
  buffer.AddSample(0_s, 1_rad);
  EXPECT_TRUE(buffer.Sample(0_s).value() == 1_rad);

  // New entry in middle of container
  buffer.AddSample(0.5_s, 0.5_rad);
  EXPECT_TRUE(buffer.Sample(0.5_s).value() == 0.5_rad);

  // Override sample
  buffer.AddSample(0.5_s, 1_rad);
  EXPECT_TRUE(buffer.Sample(0.5_s).value() == 1_rad);
}

TEST(TimeInterpolatableBufferTest, Interpolation) {
  wpi::math::TimeInterpolatableBuffer<wpi::math::Rotation2d> buffer{10_s};

  buffer.AddSample(0_s, 0_rad);
  EXPECT_TRUE(buffer.Sample(0_s).value() == 0_rad);
  buffer.AddSample(1_s, 1_rad);
  EXPECT_TRUE(buffer.Sample(0.5_s).value() == 0.5_rad);
  EXPECT_TRUE(buffer.Sample(1_s).value() == 1_rad);
  buffer.AddSample(3_s, 2_rad);
  EXPECT_TRUE(buffer.Sample(2_s).value() == 1.5_rad);

  buffer.AddSample(10.5_s, 2_rad);
  EXPECT_TRUE(buffer.Sample(0_s) == 1_rad);
}

TEST(TimeInterpolatableBufferTest, Pose2d) {
  wpi::math::TimeInterpolatableBuffer<wpi::math::Pose2d> buffer{10_s};

  // We expect to be at (1 - 1/std::sqrt(2), 1/std::sqrt(2), 45deg) at t=0.5
  buffer.AddSample(0_s, wpi::math::Pose2d{0_m, 0_m, 90_deg});
  buffer.AddSample(1_s, wpi::math::Pose2d{1_m, 1_m, 0_deg});
  wpi::math::Pose2d sample = buffer.Sample(0.5_s).value();

  EXPECT_TRUE(std::abs(sample.X().value() - (1.0 - 1.0 / std::sqrt(2.0))) <
              0.01);
  EXPECT_TRUE(std::abs(sample.Y().value() - (1.0 / std::sqrt(2.0))) < 0.01);
  EXPECT_TRUE(std::abs(sample.Rotation().Degrees().value() - 45.0) < 0.01);
}
