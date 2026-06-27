// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/math/trajectory/TrajectorySample.hpp"

#include <numbers>

#include <gtest/gtest.h>

#include "wpi/math/geometry/Pose2d.hpp"
#include "wpi/math/geometry/Rotation2d.hpp"
#include "wpi/math/geometry/Transform2d.hpp"
#include "wpi/math/geometry/Translation2d.hpp"
#include "wpi/math/kinematics/ChassisAccelerations.hpp"
#include "wpi/math/kinematics/ChassisVelocities.hpp"
#include "wpi/math/kinematics/DifferentialDriveKinematics.hpp"
#include "wpi/math/trajectory/DifferentialSample.hpp"
#include "wpi/math/trajectory/SplineSample.hpp"
#include "wpi/units/acceleration.hpp"
#include "wpi/units/angle.hpp"
#include "wpi/units/angular_acceleration.hpp"
#include "wpi/units/angular_velocity.hpp"
#include "wpi/units/curvature.hpp"
#include "wpi/units/length.hpp"
#include "wpi/units/math.hpp"
#include "wpi/units/time.hpp"
#include "wpi/units/velocity.hpp"
#include "wpi/util/MathExtras.hpp"

#define EXPECT_NEAR_UNITS(val1, val2, eps) \
  EXPECT_LE(wpi::units::math::abs(val1 - val2), eps)

TEST(TrajectorySampleTest, KinematicInterpolateAtStart) {
  wpi::math::TrajectorySample start{
      0_s, wpi::math::Pose2d{},
      wpi::math::ChassisVelocities{1_mps, 0_mps, 0_rad_per_s},
      wpi::math::ChassisAccelerations{}};
  wpi::math::TrajectorySample end{
      1_s, wpi::math::Pose2d{1_m, 0_m, 0_rad},
      wpi::math::ChassisVelocities{2_mps, 0_mps, 0_rad_per_s},
      wpi::math::ChassisAccelerations{}};

  auto interpolated = wpi::math::KinematicInterpolate(start, end, 0);

  EXPECT_EQ(start.timestamp, interpolated.timestamp);
  EXPECT_EQ(start.pose, interpolated.pose);
  EXPECT_EQ(start.velocity, interpolated.velocity);
  EXPECT_EQ(start.acceleration, interpolated.acceleration);
}

TEST(TrajectorySampleTest, KinematicInterpolateAtEnd) {
  wpi::math::TrajectorySample start{
      0_s, wpi::math::Pose2d{},
      wpi::math::ChassisVelocities{1_mps, 0_mps, 0_rad_per_s},
      wpi::math::ChassisAccelerations{}};
  wpi::math::TrajectorySample end{
      1_s, wpi::math::Pose2d{1_m, 0_m, 0_rad},
      wpi::math::ChassisVelocities{2_mps, 0_mps, 0_rad_per_s},
      wpi::math::ChassisAccelerations{}};

  auto interpolated = wpi::math::KinematicInterpolate(start, end, 1);

  EXPECT_EQ(end.timestamp, interpolated.timestamp);
  EXPECT_EQ(end.pose, interpolated.pose);
  EXPECT_EQ(end.velocity, interpolated.velocity);
  EXPECT_EQ(end.acceleration, interpolated.acceleration);
}

TEST(TrajectorySampleTest, KinematicInterpolateMidpoint) {
  wpi::math::TrajectorySample start{
      0_s, wpi::math::Pose2d{},
      wpi::math::ChassisVelocities{1_mps, 0_mps, 0_rad_per_s},
      wpi::math::ChassisAccelerations{0.2_mps_sq, 0_mps_sq, 0_rad_per_s_sq}};
  wpi::math::TrajectorySample end{
      2_s, wpi::math::Pose2d{2_m, 0_m, 0_rad},
      wpi::math::ChassisVelocities{2_mps, 0_mps, 0_rad_per_s},
      wpi::math::ChassisAccelerations{0.6_mps_sq, 0_mps_sq, 0_rad_per_s_sq}};

  auto interpolated = wpi::math::KinematicInterpolate(start, end, 0.5);

  // Absolute interpolated timestamp.
  EXPECT_NEAR_UNITS(1_s, interpolated.timestamp, 1e-9_s);

  // Elapsed time from the start sample.
  auto deltaT = 1_s;

  // vₖ₊₁ = vₖ + aₖΔt
  auto expectedVx = start.velocity.vx + start.acceleration.ax * deltaT;

  // xₖ₊₁ = xₖ + vₖΔt + ½a(Δt)²
  auto expectedX = start.pose.X() + start.velocity.vx * deltaT +
                   0.5 * start.acceleration.ax * deltaT * deltaT;

  // Linear interpolation of acceleration.
  auto expectedAx =
      wpi::util::Lerp(start.acceleration.ax, end.acceleration.ax, 0.5);

  EXPECT_NEAR_UNITS(expectedX, interpolated.pose.X(), 1e-9_m);
  EXPECT_NEAR_UNITS(expectedVx, interpolated.velocity.vx, 1e-9_mps);
  EXPECT_NEAR_UNITS(expectedAx, interpolated.acceleration.ax, 1e-9_mps_sq);
}

TEST(TrajectorySampleTest, KinematicInterpolateWithAcceleration) {
  wpi::math::TrajectorySample start{
      0_s, wpi::math::Pose2d{},
      wpi::math::ChassisVelocities{0_mps, 0_mps, 0_rad_per_s},
      wpi::math::ChassisAccelerations{1_mps_sq, 0_mps_sq, 0_rad_per_s_sq}};
  wpi::math::TrajectorySample end{
      1_s, wpi::math::Pose2d{0.5_m, 0_m, 0_rad},
      wpi::math::ChassisVelocities{1_mps, 0_mps, 0_rad_per_s},
      wpi::math::ChassisAccelerations{}};

  auto interpolated = wpi::math::KinematicInterpolate(start, end, 0.5);

  EXPECT_NEAR_UNITS(0.5_s, interpolated.timestamp, 1e-9_s);

  auto deltaT = 0.5_s;

  // vₖ₊₁ = 0 + 1.0 * 0.5 = 0.5
  EXPECT_NEAR_UNITS(0.5_mps, interpolated.velocity.vx, 1e-9_mps);

  // xₖ₊₁ = 0 + 0 + ½ * 1.0 * 0.25 = 0.125
  auto expectedX = 0.5 * start.acceleration.ax * deltaT * deltaT;
  EXPECT_NEAR_UNITS(expectedX, interpolated.pose.X(), 1e-9_m);

  // Linear interpolation of acceleration at t=0.5: 1.0 + (0 - 1.0) * 0.5 = 0.5
  EXPECT_NEAR_UNITS(0.5_mps_sq, interpolated.acceleration.ax, 1e-9_mps_sq);
}

TEST(TrajectorySampleTest, KinematicInterpolateAngularVelocity) {
  wpi::math::TrajectorySample start{
      0_s, wpi::math::Pose2d{},
      wpi::math::ChassisVelocities{1_mps, 0_mps, 0_rad_per_s},
      wpi::math::ChassisAccelerations{0_mps_sq, 0_mps_sq, 0.1_rad_per_s_sq}};
  wpi::math::TrajectorySample end{
      1_s, wpi::math::Pose2d{1_m, 0_m, 90_deg},
      wpi::math::ChassisVelocities{
          1_mps, 0_mps, wpi::units::radians_per_second_t{std::numbers::pi / 2}},
      wpi::math::ChassisAccelerations{0_mps_sq, 0_mps_sq, 0.5_rad_per_s_sq}};

  auto interpolated = wpi::math::KinematicInterpolate(start, end, 0.5);

  EXPECT_NEAR_UNITS(0.5_s, interpolated.timestamp, 1e-9_s);

  auto deltaT = 0.5_s;

  // ωₖ₊₁ = ωₖ + αₖΔt
  auto expectedOmega = start.velocity.omega + start.acceleration.alpha * deltaT;

  // Linear interpolation of angular acceleration.
  auto expectedAlpha =
      wpi::util::Lerp(start.acceleration.alpha, end.acceleration.alpha, 0.5);

  // θₖ₊₁ = θₖ + ωₖΔt + ½α(Δt)²
  auto expectedTheta = start.pose.Rotation().Radians() +
                       start.velocity.omega * deltaT +
                       0.5 * start.acceleration.alpha * deltaT * deltaT;

  EXPECT_NEAR_UNITS(expectedOmega, interpolated.velocity.omega, 1e-9_rad_per_s);
  EXPECT_NEAR_UNITS(expectedAlpha, interpolated.acceleration.alpha,
                    1e-9_rad_per_s_sq);
  EXPECT_NEAR_UNITS(expectedTheta, interpolated.pose.Rotation().Radians(),
                    1e-9_rad);
}

TEST(TrajectorySampleTest, KinematicInterpolateNegativeVelocity) {
  wpi::math::TrajectorySample start{
      0_s, wpi::math::Pose2d{1_m, 0_m, 0_rad},
      wpi::math::ChassisVelocities{-1_mps, 0_mps, 0_rad_per_s},
      wpi::math::ChassisAccelerations{-0.5_mps_sq, 0_mps_sq, 0_rad_per_s_sq}};
  wpi::math::TrajectorySample end{
      1_s, wpi::math::Pose2d{0_m, 0_m, 0_rad},
      wpi::math::ChassisVelocities{-2_mps, 0_mps, 0_rad_per_s},
      wpi::math::ChassisAccelerations{}};

  auto interpolated = wpi::math::KinematicInterpolate(start, end, 0.5);

  EXPECT_NEAR_UNITS(0.5_s, interpolated.timestamp, 1e-9_s);

  // xₖ₊₁ = xₖ + vₖΔt + ½a(Δt)²
  auto deltaT = 0.5_s;
  auto expectedX = start.pose.X() + start.velocity.vx * deltaT +
                   0.5 * start.acceleration.ax * deltaT * deltaT;
  EXPECT_NEAR_UNITS(expectedX, interpolated.pose.X(), 1e-9_m);
}

TEST(TrajectorySampleTest, KinematicInterpolateMonotonicity) {
  wpi::math::TrajectorySample start{
      0_s, wpi::math::Pose2d{},
      wpi::math::ChassisVelocities{1_mps, 0_mps, 0_rad_per_s},
      wpi::math::ChassisAccelerations{}};
  wpi::math::TrajectorySample end{
      1_s, wpi::math::Pose2d{1_m, 0_m, 0_rad},
      wpi::math::ChassisVelocities{2_mps, 0_mps, 0_rad_per_s},
      wpi::math::ChassisAccelerations{}};

  auto prev = start;
  for (int t = 1; t <= 10; t++) {
    auto curr = wpi::math::KinematicInterpolate(start, end, t / 10.0);
    EXPECT_GE(curr.pose.X(), prev.pose.X() - 1e-9_m);
    EXPECT_GE(curr.timestamp, prev.timestamp - 1e-9_s);
    prev = curr;
  }
}

TEST(TrajectorySampleTest, KinematicInterpolateNonzeroStartTimestamp) {
  // Regression test: integration must use the elapsed time from the start
  // sample (Δt), not the absolute interpolated timestamp. This only differs
  // when the start sample's timestamp is nonzero.
  wpi::math::TrajectorySample start{
      10_s, wpi::math::Pose2d{},
      wpi::math::ChassisVelocities{1_mps, 0_mps, 0_rad_per_s},
      wpi::math::ChassisAccelerations{2_mps_sq, 0_mps_sq, 0_rad_per_s_sq}};
  wpi::math::TrajectorySample end{
      12_s, wpi::math::Pose2d{8_m, 0_m, 0_rad},
      wpi::math::ChassisVelocities{5_mps, 0_mps, 0_rad_per_s},
      wpi::math::ChassisAccelerations{2_mps_sq, 0_mps_sq, 0_rad_per_s_sq}};

  auto interpolated = wpi::math::KinematicInterpolate(start, end, 0.5);

  // Absolute interpolated timestamp.
  EXPECT_NEAR_UNITS(11_s, interpolated.timestamp, 1e-9_s);

  // vₖ₊₁ = vₖ + aₖΔt = 1 + 2*1 = 3 (would be 1 + 2*11 = 23 with the bug)
  EXPECT_NEAR_UNITS(3_mps, interpolated.velocity.vx, 1e-9_mps);

  // xₖ₊₁ = xₖ + vₖΔt + ½aₖ(Δt)² = 0 + 1*1 + ½*2*1 = 2
  EXPECT_NEAR_UNITS(2_m, interpolated.pose.X(), 1e-9_m);
}

TEST(TrajectorySampleTest, KinematicInterpolateZeroTime) {
  wpi::math::TrajectorySample start{
      0_s, wpi::math::Pose2d{},
      wpi::math::ChassisVelocities{1_mps, 0_mps, 0_rad_per_s},
      wpi::math::ChassisAccelerations{}};
  wpi::math::TrajectorySample end{
      0_s, wpi::math::Pose2d{1_m, 0_m, 0_rad},
      wpi::math::ChassisVelocities{2_mps, 0_mps, 0_rad_per_s},
      wpi::math::ChassisAccelerations{}};

  // Should handle zero time difference gracefully.
  auto interpolated = wpi::math::KinematicInterpolate(start, end, 0.5);
  EXPECT_NEAR_UNITS(0_s, interpolated.timestamp, 1e-9_s);
}

TEST(TrajectorySampleTest, SplineSampleStoresFieldRelativeVelocity) {
  // A SplineSample is built from path-relative (forward) scalars but stores
  // velocity/acceleration in the field frame. For a robot facing +90 degrees
  // moving forward, the field velocity should point along +y.
  wpi::units::meters_per_second_t forwardVelocity{2.0};
  wpi::units::meters_per_second_squared_t forwardAcceleration{1.5};
  wpi::units::curvature_t curvature{0.25};
  wpi::math::SplineSample sample{0_s, wpi::math::Pose2d{0_m, 0_m, 90_deg},
                                 forwardVelocity, forwardAcceleration,
                                 curvature};

  // Field-relative: forward speed rotated into +y.
  EXPECT_NEAR_UNITS(0_mps, sample.velocity.vx, 1e-9_mps);
  EXPECT_NEAR_UNITS(forwardVelocity, sample.velocity.vy, 1e-9_mps);
  // Omega is frame-invariant and equals forward * curvature.
  EXPECT_NEAR_UNITS(forwardVelocity * curvature, sample.velocity.omega,
                    1e-9_rad_per_s);

  EXPECT_NEAR_UNITS(0_mps_sq, sample.acceleration.ax, 1e-9_mps_sq);
  EXPECT_NEAR_UNITS(forwardAcceleration, sample.acceleration.ay, 1e-9_mps_sq);

  // The projection accessors recover the path-relative scalars.
  EXPECT_NEAR_UNITS(forwardVelocity, sample.ForwardVelocity(), 1e-9_mps);
  EXPECT_NEAR_UNITS(forwardAcceleration, sample.ForwardAcceleration(),
                    1e-9_mps_sq);
}

TEST(TrajectorySampleTest, TransformRotatesVelocityAndAcceleration) {
  // Field-relative velocity/acceleration must rotate with the transform's
  // rotation.
  wpi::math::TrajectorySample sample{
      0_s, wpi::math::Pose2d{0_m, 0_m, 0_rad},
      wpi::math::ChassisVelocities{1_mps, 0_mps, 0.5_rad_per_s},
      wpi::math::ChassisAccelerations{2_mps_sq, 0_mps_sq, 0.3_rad_per_s_sq}};

  auto transformed = sample.Transform(
      wpi::math::Transform2d{wpi::math::Translation2d{3_m, 4_m}, 90_deg});

  EXPECT_NEAR_UNITS(3_m, transformed.pose.X(), 1e-9_m);
  EXPECT_NEAR_UNITS(4_m, transformed.pose.Y(), 1e-9_m);
  EXPECT_NEAR_UNITS(wpi::units::radian_t{std::numbers::pi / 2},
                    transformed.pose.Rotation().Radians(), 1e-9_rad);

  // Velocity/acceleration vectors rotate by +90 degrees; angular terms are
  // unchanged.
  EXPECT_NEAR_UNITS(0_mps, transformed.velocity.vx, 1e-9_mps);
  EXPECT_NEAR_UNITS(1_mps, transformed.velocity.vy, 1e-9_mps);
  EXPECT_NEAR_UNITS(0.5_rad_per_s, transformed.velocity.omega, 1e-9_rad_per_s);
  EXPECT_NEAR_UNITS(0_mps_sq, transformed.acceleration.ax, 1e-9_mps_sq);
  EXPECT_NEAR_UNITS(2_mps_sq, transformed.acceleration.ay, 1e-9_mps_sq);
  EXPECT_NEAR_UNITS(0.3_rad_per_s_sq, transformed.acceleration.alpha,
                    1e-9_rad_per_s_sq);
}

TEST(TrajectorySampleTest, RelativeToRotatesVelocityAndAcceleration) {
  // relativeTo re-expresses the sample in a frame rotated by the other pose's
  // rotation, so field-relative velocity/acceleration rotate by the negative of
  // it.
  wpi::math::TrajectorySample sample{
      0_s, wpi::math::Pose2d{1_m, 2_m, 90_deg},
      wpi::math::ChassisVelocities{0_mps, 1_mps, 0.5_rad_per_s},
      wpi::math::ChassisAccelerations{0_mps_sq, 2_mps_sq, 0.3_rad_per_s_sq}};

  auto relative = sample.RelativeTo(wpi::math::Pose2d{1_m, 2_m, 90_deg});

  EXPECT_NEAR_UNITS(0_m, relative.pose.X(), 1e-9_m);
  EXPECT_NEAR_UNITS(0_m, relative.pose.Y(), 1e-9_m);
  EXPECT_NEAR_UNITS(0_rad, relative.pose.Rotation().Radians(), 1e-9_rad);

  // Velocity/acceleration vectors rotate by -90 degrees; angular terms are
  // unchanged.
  EXPECT_NEAR_UNITS(1_mps, relative.velocity.vx, 1e-9_mps);
  EXPECT_NEAR_UNITS(0_mps, relative.velocity.vy, 1e-9_mps);
  EXPECT_NEAR_UNITS(0.5_rad_per_s, relative.velocity.omega, 1e-9_rad_per_s);
  EXPECT_NEAR_UNITS(2_mps_sq, relative.acceleration.ax, 1e-9_mps_sq);
  EXPECT_NEAR_UNITS(0_mps_sq, relative.acceleration.ay, 1e-9_mps_sq);
  EXPECT_NEAR_UNITS(0.3_rad_per_s_sq, relative.acceleration.alpha,
                    1e-9_rad_per_s_sq);
}

TEST(TrajectorySampleTest, SplineSampleTransformPreservesForwardScalars) {
  // Rotating the sample rotates both the heading and the field velocity, so the
  // heading-relative forward scalars (and curvature) are invariant.
  wpi::math::SplineSample sample{0_s, wpi::math::Pose2d{1_m, 2_m, 20_deg},
                                 2_mps, 1.5_mps_sq, 0.25_rad / 1_m};

  auto transformed = sample.Transform(
      wpi::math::Transform2d{wpi::math::Translation2d{3_m, 4_m}, 35_deg});
  auto relative = sample.RelativeTo(wpi::math::Pose2d{0_m, 0_m, -15_deg});

  for (const auto& s : {transformed, relative}) {
    EXPECT_NEAR_UNITS(2_mps, s.ForwardVelocity(), 1e-9_mps);
    EXPECT_NEAR_UNITS(1.5_mps_sq, s.ForwardAcceleration(), 1e-9_mps_sq);
    EXPECT_NEAR_UNITS(0.25_rad / 1_m, s.curvature, 1e-9_rad / 1_m);
  }
}

TEST(TrajectorySampleTest, DifferentialSampleTransformPreservesWheelSpeeds) {
  // Wheel speeds are frame-invariant, so they survive a transform unchanged
  // while the field-relative velocity rotates.
  wpi::math::DifferentialDriveKinematics kinematics{0.5_m};
  wpi::math::DifferentialSample sample{
      0_s, wpi::math::Pose2d{0_m, 0_m, 0_rad},
      wpi::math::ChassisVelocities{2_mps, 0_mps, 0.5_rad_per_s},
      wpi::math::ChassisAccelerations{1_mps_sq, 0_mps_sq, 0.2_rad_per_s_sq},
      kinematics};

  auto transformed = sample.Transform(
      wpi::math::Transform2d{wpi::math::Translation2d{0_m, 0_m}, 90_deg});

  EXPECT_NEAR_UNITS(sample.leftSpeed, transformed.leftSpeed, 1e-9_mps);
  EXPECT_NEAR_UNITS(sample.rightSpeed, transformed.rightSpeed, 1e-9_mps);

  // The field velocity rotates by +90 degrees.
  EXPECT_NEAR_UNITS(0_mps, transformed.velocity.vx, 1e-9_mps);
  EXPECT_NEAR_UNITS(2_mps, transformed.velocity.vy, 1e-9_mps);
  EXPECT_NEAR_UNITS(0.5_rad_per_s, transformed.velocity.omega, 1e-9_rad_per_s);
}
