// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

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
#include "wpi/math/trajectory/TrajectorySample.hpp"

namespace {
constexpr double kEpsilon = 1e-9;
}  // namespace

TEST(TrajectorySampleTest, KinematicInterpolateAtStart) {
  wpi::math::TrajectorySample start{0_s, wpi::math::Pose2d{},
                                    wpi::math::ChassisVelocities{1_mps, 0_mps,
                                                                 0_rad_per_s},
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
  wpi::math::TrajectorySample start{0_s, wpi::math::Pose2d{},
                                    wpi::math::ChassisVelocities{1_mps, 0_mps,
                                                                 0_rad_per_s},
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
  EXPECT_NEAR(1.0, interpolated.timestamp.value(), kEpsilon);

  // Elapsed time from the start sample.
  double deltaT = 1.0;

  // vₖ₊₁ = vₖ + aₖΔt
  double expectedVx =
      start.velocity.vx.value() + start.acceleration.ax.value() * deltaT;

  // xₖ₊₁ = xₖ + vₖΔt + ½a(Δt)²
  double expectedX = start.pose.X().value() +
                     start.velocity.vx.value() * deltaT +
                     0.5 * start.acceleration.ax.value() * deltaT * deltaT;

  // Linear interpolation of acceleration.
  double expectedAx =
      start.acceleration.ax.value() +
      (end.acceleration.ax.value() - start.acceleration.ax.value()) * 0.5;

  EXPECT_NEAR(expectedX, interpolated.pose.X().value(), kEpsilon);
  EXPECT_NEAR(expectedVx, interpolated.velocity.vx.value(), kEpsilon);
  EXPECT_NEAR(expectedAx, interpolated.acceleration.ax.value(), kEpsilon);
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

  EXPECT_NEAR(0.5, interpolated.timestamp.value(), kEpsilon);

  double deltaT = 0.5;

  // vₖ₊₁ = 0 + 1.0 * 0.5 = 0.5
  EXPECT_NEAR(0.5, interpolated.velocity.vx.value(), kEpsilon);

  // xₖ₊₁ = 0 + 0 + ½ * 1.0 * 0.25 = 0.125
  double expectedX = 0.5 * start.acceleration.ax.value() * deltaT * deltaT;
  EXPECT_NEAR(expectedX, interpolated.pose.X().value(), kEpsilon);

  // Linear interpolation of acceleration at t=0.5: 1.0 + (0 - 1.0) * 0.5 = 0.5
  EXPECT_NEAR(0.5, interpolated.acceleration.ax.value(), kEpsilon);
}

TEST(TrajectorySampleTest, KinematicInterpolateAngularVelocity) {
  wpi::math::TrajectorySample start{
      0_s, wpi::math::Pose2d{},
      wpi::math::ChassisVelocities{1_mps, 0_mps, 0_rad_per_s},
      wpi::math::ChassisAccelerations{0_mps_sq, 0_mps_sq, 0.1_rad_per_s_sq}};
  wpi::math::TrajectorySample end{
      1_s, wpi::math::Pose2d{1_m, 0_m, 90_deg},
      wpi::math::ChassisVelocities{1_mps, 0_mps,
                                   wpi::units::radians_per_second_t{
                                       std::numbers::pi / 2}},
      wpi::math::ChassisAccelerations{0_mps_sq, 0_mps_sq, 0.5_rad_per_s_sq}};

  auto interpolated = wpi::math::KinematicInterpolate(start, end, 0.5);

  EXPECT_NEAR(0.5, interpolated.timestamp.value(), kEpsilon);

  double deltaT = 0.5;

  // ωₖ₊₁ = ωₖ + αₖΔt
  double expectedOmega = start.velocity.omega.value() +
                         start.acceleration.alpha.value() * deltaT;

  // Linear interpolation of angular acceleration.
  double expectedAlpha =
      start.acceleration.alpha.value() +
      (end.acceleration.alpha.value() - start.acceleration.alpha.value()) * 0.5;

  // θₖ₊₁ = θₖ + ωₖΔt + ½α(Δt)²
  double expectedTheta = start.pose.Rotation().Radians().value() +
                         start.velocity.omega.value() * deltaT +
                         0.5 * start.acceleration.alpha.value() * deltaT * deltaT;

  EXPECT_NEAR(expectedOmega, interpolated.velocity.omega.value(), kEpsilon);
  EXPECT_NEAR(expectedAlpha, interpolated.acceleration.alpha.value(), kEpsilon);
  EXPECT_NEAR(expectedTheta, interpolated.pose.Rotation().Radians().value(),
              kEpsilon);
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

  EXPECT_NEAR(0.5, interpolated.timestamp.value(), kEpsilon);

  // xₖ₊₁ = xₖ + vₖΔt + ½a(Δt)²
  double deltaT = 0.5;
  double expectedX = start.pose.X().value() +
                     start.velocity.vx.value() * deltaT +
                     0.5 * start.acceleration.ax.value() * deltaT * deltaT;
  EXPECT_NEAR(expectedX, interpolated.pose.X().value(), kEpsilon);
}

TEST(TrajectorySampleTest, KinematicInterpolateMonotonicity) {
  wpi::math::TrajectorySample start{0_s, wpi::math::Pose2d{},
                                    wpi::math::ChassisVelocities{1_mps, 0_mps,
                                                                 0_rad_per_s},
                                    wpi::math::ChassisAccelerations{}};
  wpi::math::TrajectorySample end{
      1_s, wpi::math::Pose2d{1_m, 0_m, 0_rad},
      wpi::math::ChassisVelocities{2_mps, 0_mps, 0_rad_per_s},
      wpi::math::ChassisAccelerations{}};

  auto prev = start;
  for (int t = 1; t <= 10; t++) {
    auto curr = wpi::math::KinematicInterpolate(start, end, t / 10.0);
    EXPECT_GE(curr.pose.X().value(), prev.pose.X().value() - kEpsilon);
    EXPECT_GE(curr.timestamp.value(), prev.timestamp.value() - kEpsilon);
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
  EXPECT_NEAR(11.0, interpolated.timestamp.value(), kEpsilon);

  // vₖ₊₁ = vₖ + aₖΔt = 1 + 2*1 = 3 (would be 1 + 2*11 = 23 with the bug)
  EXPECT_NEAR(3.0, interpolated.velocity.vx.value(), kEpsilon);

  // xₖ₊₁ = xₖ + vₖΔt + ½aₖ(Δt)² = 0 + 1*1 + ½*2*1 = 2
  EXPECT_NEAR(2.0, interpolated.pose.X().value(), kEpsilon);
}

TEST(TrajectorySampleTest, KinematicInterpolateZeroTime) {
  wpi::math::TrajectorySample start{0_s, wpi::math::Pose2d{},
                                    wpi::math::ChassisVelocities{1_mps, 0_mps,
                                                                 0_rad_per_s},
                                    wpi::math::ChassisAccelerations{}};
  wpi::math::TrajectorySample end{
      0_s, wpi::math::Pose2d{1_m, 0_m, 0_rad},
      wpi::math::ChassisVelocities{2_mps, 0_mps, 0_rad_per_s},
      wpi::math::ChassisAccelerations{}};

  // Should handle zero time difference gracefully.
  auto interpolated = wpi::math::KinematicInterpolate(start, end, 0.5);
  EXPECT_NEAR(0.0, interpolated.timestamp.value(), kEpsilon);
}

TEST(TrajectorySampleTest, SplineSampleStoresFieldRelativeVelocity) {
  // A SplineSample is built from path-relative (forward) scalars but stores
  // velocity/acceleration in the field frame. For a robot facing +90 degrees
  // moving forward, the field velocity should point along +y.
  double forward = 2.0;
  double forwardAccel = 1.5;
  double curvature = 0.25;
  wpi::math::SplineSample sample{0.0, wpi::math::Pose2d{0_m, 0_m, 90_deg},
                                 forward, forwardAccel, curvature};

  // Field-relative: forward speed rotated into +y.
  EXPECT_NEAR(0.0, sample.velocity.vx.value(), kEpsilon);
  EXPECT_NEAR(forward, sample.velocity.vy.value(), kEpsilon);
  // Omega is frame-invariant and equals forward * curvature.
  EXPECT_NEAR(forward * curvature, sample.velocity.omega.value(), kEpsilon);

  EXPECT_NEAR(0.0, sample.acceleration.ax.value(), kEpsilon);
  EXPECT_NEAR(forwardAccel, sample.acceleration.ay.value(), kEpsilon);

  // The projection accessors recover the path-relative scalars.
  EXPECT_NEAR(forward, sample.ForwardVelocity().value(), kEpsilon);
  EXPECT_NEAR(forwardAccel, sample.ForwardAcceleration().value(), kEpsilon);
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

  EXPECT_NEAR(3.0, transformed.pose.X().value(), kEpsilon);
  EXPECT_NEAR(4.0, transformed.pose.Y().value(), kEpsilon);
  EXPECT_NEAR(std::numbers::pi / 2,
              transformed.pose.Rotation().Radians().value(), kEpsilon);

  // Velocity/acceleration vectors rotate by +90 degrees; angular terms are
  // unchanged.
  EXPECT_NEAR(0.0, transformed.velocity.vx.value(), kEpsilon);
  EXPECT_NEAR(1.0, transformed.velocity.vy.value(), kEpsilon);
  EXPECT_NEAR(0.5, transformed.velocity.omega.value(), kEpsilon);
  EXPECT_NEAR(0.0, transformed.acceleration.ax.value(), kEpsilon);
  EXPECT_NEAR(2.0, transformed.acceleration.ay.value(), kEpsilon);
  EXPECT_NEAR(0.3, transformed.acceleration.alpha.value(), kEpsilon);
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

  EXPECT_NEAR(0.0, relative.pose.X().value(), kEpsilon);
  EXPECT_NEAR(0.0, relative.pose.Y().value(), kEpsilon);
  EXPECT_NEAR(0.0, relative.pose.Rotation().Radians().value(), kEpsilon);

  // Velocity/acceleration vectors rotate by -90 degrees; angular terms are
  // unchanged.
  EXPECT_NEAR(1.0, relative.velocity.vx.value(), kEpsilon);
  EXPECT_NEAR(0.0, relative.velocity.vy.value(), kEpsilon);
  EXPECT_NEAR(0.5, relative.velocity.omega.value(), kEpsilon);
  EXPECT_NEAR(2.0, relative.acceleration.ax.value(), kEpsilon);
  EXPECT_NEAR(0.0, relative.acceleration.ay.value(), kEpsilon);
  EXPECT_NEAR(0.3, relative.acceleration.alpha.value(), kEpsilon);
}

TEST(TrajectorySampleTest, SplineSampleTransformPreservesForwardScalars) {
  // Rotating the sample rotates both the heading and the field velocity, so the
  // heading-relative forward scalars (and curvature) are invariant.
  wpi::math::SplineSample sample{0.0, wpi::math::Pose2d{1_m, 2_m, 20_deg}, 2.0,
                                 1.5, 0.25};

  auto transformed = sample.Transform(
      wpi::math::Transform2d{wpi::math::Translation2d{3_m, 4_m}, 35_deg});
  auto relative = sample.RelativeTo(wpi::math::Pose2d{0_m, 0_m, -15_deg});

  for (const auto& s : {transformed, relative}) {
    EXPECT_NEAR(2.0, s.ForwardVelocity().value(), kEpsilon);
    EXPECT_NEAR(1.5, s.ForwardAcceleration().value(), kEpsilon);
    EXPECT_NEAR(0.25, s.curvature.value(), kEpsilon);
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

  EXPECT_NEAR(sample.leftSpeed.value(), transformed.leftSpeed.value(),
              kEpsilon);
  EXPECT_NEAR(sample.rightSpeed.value(), transformed.rightSpeed.value(),
              kEpsilon);

  // The field velocity rotates by +90 degrees.
  EXPECT_NEAR(0.0, transformed.velocity.vx.value(), kEpsilon);
  EXPECT_NEAR(2.0, transformed.velocity.vy.value(), kEpsilon);
  EXPECT_NEAR(0.5, transformed.velocity.omega.value(), kEpsilon);
}
