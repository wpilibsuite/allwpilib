// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/math/trajectory/HolonomicSample.hpp"

#include <numbers>

#include <catch2/catch_test_macros.hpp>

#include "wpi/math/geometry/Pose2d.hpp"
#include "wpi/math/geometry/Rotation2d.hpp"
#include "wpi/math/geometry/Transform2d.hpp"
#include "wpi/math/geometry/Translation2d.hpp"
#include "wpi/math/kinematics/ChassisAccelerations.hpp"
#include "wpi/math/kinematics/ChassisVelocities.hpp"
#include "wpi/math/kinematics/DifferentialDriveKinematics.hpp"
#include "wpi/math/trajectory/DifferentialSample.hpp"
#include "wpi/math/trajectory/DrivetrainSplineSample.hpp"
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

#define CHECK_NEAR_UNITS(val1, val2, eps) \
  CHECK(wpi::units::math::abs((val1) - (val2)) <= (eps))

TEST_CASE("HolonomicSampleTest KinematicInterpolateAtStart", "[wpimath]") {
  wpi::math::HolonomicSample start{
      0_s, wpi::math::Pose2d{},
      wpi::math::ChassisVelocities{1_mps, 0_mps, 0_rad_per_s},
      wpi::math::ChassisAccelerations{}};
  wpi::math::HolonomicSample end{
      1_s, wpi::math::Pose2d{1_m, 0_m, 0_rad},
      wpi::math::ChassisVelocities{2_mps, 0_mps, 0_rad_per_s},
      wpi::math::ChassisAccelerations{}};

  auto interpolated = wpi::math::KinematicInterpolate(start, end, 0);

  CHECK(start.time == interpolated.time);
  CHECK(start.pose == interpolated.pose);
  CHECK(start.velocity == interpolated.velocity);
  CHECK(start.acceleration == interpolated.acceleration);
}

TEST_CASE("HolonomicSampleTest KinematicInterpolateAtEnd", "[wpimath]") {
  wpi::math::HolonomicSample start{
      0_s, wpi::math::Pose2d{},
      wpi::math::ChassisVelocities{1_mps, 0_mps, 0_rad_per_s},
      wpi::math::ChassisAccelerations{}};
  wpi::math::HolonomicSample end{
      1_s, wpi::math::Pose2d{1_m, 0_m, 0_rad},
      wpi::math::ChassisVelocities{2_mps, 0_mps, 0_rad_per_s},
      wpi::math::ChassisAccelerations{}};

  auto interpolated = wpi::math::KinematicInterpolate(start, end, 1);

  CHECK(end.time == interpolated.time);
  CHECK(end.pose == interpolated.pose);
  CHECK(end.velocity == interpolated.velocity);
  CHECK(end.acceleration == interpolated.acceleration);
}

TEST_CASE("HolonomicSampleTest KinematicInterpolateMidpoint", "[wpimath]") {
  wpi::math::HolonomicSample start{
      0_s, wpi::math::Pose2d{},
      wpi::math::ChassisVelocities{1_mps, 0_mps, 0_rad_per_s},
      wpi::math::ChassisAccelerations{0.2_mps_sq, 0_mps_sq, 0_rad_per_s_sq}};
  wpi::math::HolonomicSample end{
      2_s, wpi::math::Pose2d{2_m, 0_m, 0_rad},
      wpi::math::ChassisVelocities{2_mps, 0_mps, 0_rad_per_s},
      wpi::math::ChassisAccelerations{0.6_mps_sq, 0_mps_sq, 0_rad_per_s_sq}};

  auto interpolated = wpi::math::KinematicInterpolate(start, end, 0.5);

  // Absolute interpolated time.
  CHECK_NEAR_UNITS(1_s, interpolated.time, 1e-9_s);

  // Elapsed time from the start sample.
  auto deltaT = 1_s;

  // xₖ₊₁ = xₖ + vₖΔt + ½a(Δt)²
  auto expectedX = start.pose.X() + start.velocity.vx * deltaT +
                   0.5 * start.acceleration.ax * deltaT * deltaT;

  // vₖ₊₁ = vₖ + aₖΔt
  auto expectedVx = start.velocity.vx + start.acceleration.ax * deltaT;

  // Constant acceleration
  auto expectedAx = start.acceleration.ax;

  CHECK_NEAR_UNITS(expectedX, interpolated.pose.X(), 1e-9_m);
  CHECK_NEAR_UNITS(expectedVx, interpolated.velocity.vx, 1e-9_mps);
  CHECK_NEAR_UNITS(expectedAx, interpolated.acceleration.ax, 1e-9_mps_sq);
}

TEST_CASE("HolonomicSampleTest KinematicInterpolateWithAcceleration",
          "[wpimath]") {
  wpi::math::HolonomicSample start{
      0_s, wpi::math::Pose2d{},
      wpi::math::ChassisVelocities{0_mps, 0_mps, 0_rad_per_s},
      wpi::math::ChassisAccelerations{1_mps_sq, 0_mps_sq, 0_rad_per_s_sq}};
  wpi::math::HolonomicSample end{
      1_s, wpi::math::Pose2d{0.5_m, 0_m, 0_rad},
      wpi::math::ChassisVelocities{1_mps, 0_mps, 0_rad_per_s},
      wpi::math::ChassisAccelerations{}};

  auto interpolated = wpi::math::KinematicInterpolate(start, end, 0.5);

  CHECK_NEAR_UNITS(0.5_s, interpolated.time, 1e-9_s);

  auto deltaT = 0.5_s;

  // xₖ₊₁ = 0 + 0 + ½ * 1.0 * 0.25 = 0.125
  auto expectedX = 0.5 * start.acceleration.ax * deltaT * deltaT;
  CHECK_NEAR_UNITS(expectedX, interpolated.pose.X(), 1e-9_m);

  // vₖ₊₁ = 0 + 1.0 * 0.5 = 0.5
  CHECK_NEAR_UNITS(0.5_mps, interpolated.velocity.vx, 1e-9_mps);

  // Constant acceleration
  CHECK_NEAR_UNITS(1_mps_sq, interpolated.acceleration.ax, 1e-9_mps_sq);
}

TEST_CASE("HolonomicSampleTest KinematicInterpolateAngularVelocity",
          "[wpimath]") {
  wpi::math::HolonomicSample start{
      0_s, wpi::math::Pose2d{},
      wpi::math::ChassisVelocities{1_mps, 0_mps, 0_rad_per_s},
      wpi::math::ChassisAccelerations{0_mps_sq, 0_mps_sq, 0.1_rad_per_s_sq}};
  wpi::math::HolonomicSample end{
      1_s, wpi::math::Pose2d{1_m, 0_m, 90_deg},
      wpi::math::ChassisVelocities{
          1_mps, 0_mps, wpi::units::radians_per_second_t{std::numbers::pi / 2}},
      wpi::math::ChassisAccelerations{0_mps_sq, 0_mps_sq, 0.5_rad_per_s_sq}};

  auto interpolated = wpi::math::KinematicInterpolate(start, end, 0.5);

  CHECK_NEAR_UNITS(0.5_s, interpolated.time, 1e-9_s);

  auto deltaT = 0.5_s;

  // θₖ₊₁ = θₖ + ωₖΔt + ½α(Δt)²
  auto expectedTheta = start.pose.Rotation().Radians() +
                       start.velocity.omega * deltaT +
                       0.5 * start.acceleration.alpha * deltaT * deltaT;

  // ωₖ₊₁ = ωₖ + αₖΔt
  auto expectedOmega = start.velocity.omega + start.acceleration.alpha * deltaT;

  // Constant angular acceleration
  auto expectedAlpha = start.acceleration.alpha;

  CHECK_NEAR_UNITS(expectedOmega, interpolated.velocity.omega, 1e-9_rad_per_s);
  CHECK_NEAR_UNITS(expectedAlpha, interpolated.acceleration.alpha,
                   1e-9_rad_per_s_sq);
  CHECK_NEAR_UNITS(expectedTheta, interpolated.pose.Rotation().Radians(),
                   1e-9_rad);
}

TEST_CASE("HolonomicSampleTest KinematicInterpolateNegativeVelocity",
          "[wpimath]") {
  wpi::math::HolonomicSample start{
      0_s, wpi::math::Pose2d{1_m, 0_m, 0_rad},
      wpi::math::ChassisVelocities{-1_mps, 0_mps, 0_rad_per_s},
      wpi::math::ChassisAccelerations{-0.5_mps_sq, 0_mps_sq, 0_rad_per_s_sq}};
  wpi::math::HolonomicSample end{
      1_s, wpi::math::Pose2d{0_m, 0_m, 0_rad},
      wpi::math::ChassisVelocities{-2_mps, 0_mps, 0_rad_per_s},
      wpi::math::ChassisAccelerations{}};

  auto interpolated = wpi::math::KinematicInterpolate(start, end, 0.5);

  CHECK_NEAR_UNITS(0.5_s, interpolated.time, 1e-9_s);

  // xₖ₊₁ = xₖ + vₖΔt + ½a(Δt)²
  auto deltaT = 0.5_s;
  auto expectedX = start.pose.X() + start.velocity.vx * deltaT +
                   0.5 * start.acceleration.ax * deltaT * deltaT;
  CHECK_NEAR_UNITS(expectedX, interpolated.pose.X(), 1e-9_m);
}

TEST_CASE("HolonomicSampleTest KinematicInterpolateMonotonicity",
          "[wpimath]") {
  wpi::math::HolonomicSample start{
      0_s, wpi::math::Pose2d{},
      wpi::math::ChassisVelocities{1_mps, 0_mps, 0_rad_per_s},
      wpi::math::ChassisAccelerations{}};
  wpi::math::HolonomicSample end{
      1_s, wpi::math::Pose2d{1_m, 0_m, 0_rad},
      wpi::math::ChassisVelocities{2_mps, 0_mps, 0_rad_per_s},
      wpi::math::ChassisAccelerations{}};

  auto prev = start;
  for (int t = 1; t <= 10; t++) {
    auto curr = wpi::math::KinematicInterpolate(start, end, t / 10.0);
    CHECK(curr.pose.X() >= prev.pose.X() - 1e-9_m);
    CHECK(curr.time >= prev.time - 1e-9_s);
    prev = curr;
  }
}

TEST_CASE("HolonomicSampleTest KinematicInterpolateNonzeroStartTimestamp",
          "[wpimath]") {
  // Regression test: integration must use the elapsed time from the start
  // sample (Δt), not the absolute interpolated time. This only differs
  // when the start sample's time is nonzero.
  wpi::math::HolonomicSample start{
      10_s, wpi::math::Pose2d{},
      wpi::math::ChassisVelocities{1_mps, 0_mps, 0_rad_per_s},
      wpi::math::ChassisAccelerations{2_mps_sq, 0_mps_sq, 0_rad_per_s_sq}};
  wpi::math::HolonomicSample end{
      12_s, wpi::math::Pose2d{8_m, 0_m, 0_rad},
      wpi::math::ChassisVelocities{5_mps, 0_mps, 0_rad_per_s},
      wpi::math::ChassisAccelerations{2_mps_sq, 0_mps_sq, 0_rad_per_s_sq}};

  auto interpolated = wpi::math::KinematicInterpolate(start, end, 0.5);

  // Absolute interpolated time.
  CHECK_NEAR_UNITS(11_s, interpolated.time, 1e-9_s);

  // vₖ₊₁ = vₖ + aₖΔt = 1 + 2*1 = 3 (would be 1 + 2*11 = 23 with the bug)
  CHECK_NEAR_UNITS(3_mps, interpolated.velocity.vx, 1e-9_mps);

  // xₖ₊₁ = xₖ + vₖΔt + ½aₖ(Δt)² = 0 + 1*1 + ½*2*1 = 2
  CHECK_NEAR_UNITS(2_m, interpolated.pose.X(), 1e-9_m);
}

TEST_CASE("HolonomicSampleTest KinematicInterpolateZeroTime", "[wpimath]") {
  wpi::math::HolonomicSample start{
      0_s, wpi::math::Pose2d{},
      wpi::math::ChassisVelocities{1_mps, 0_mps, 0_rad_per_s},
      wpi::math::ChassisAccelerations{}};
  wpi::math::HolonomicSample end{
      0_s, wpi::math::Pose2d{1_m, 0_m, 0_rad},
      wpi::math::ChassisVelocities{2_mps, 0_mps, 0_rad_per_s},
      wpi::math::ChassisAccelerations{}};

  // Should handle zero time difference gracefully.
  auto interpolated = wpi::math::KinematicInterpolate(start, end, 0.5);
  CHECK_NEAR_UNITS(0_s, interpolated.time, 1e-9_s);
}

TEST_CASE("HolonomicSampleTest SplineSampleStoresFieldRelativeVelocity",
          "[wpimath]") {
  // A DrivetrainSplineSample is built from path-relative (forward) scalars but
  // stores velocity/acceleration in the field frame. For a robot facing +90
  // degrees moving forward, the field velocity should point along +y.
  wpi::units::meters_per_second_t forwardVelocity{2.0};
  wpi::units::meters_per_second_squared_t forwardAcceleration{1.5};
  wpi::units::curvature_t curvature{0.25};
  wpi::math::DrivetrainSplineSample sample{
      0_s, wpi::math::Pose2d{0_m, 0_m, 90_deg}, forwardVelocity,
      forwardAcceleration, curvature};

  // Field-relative: forward speed rotated into +y.
  CHECK_NEAR_UNITS(0_mps, sample.velocity.vx, 1e-9_mps);
  CHECK_NEAR_UNITS(forwardVelocity, sample.velocity.vy, 1e-9_mps);
  // Omega is frame-invariant and equals forward * curvature.
  CHECK_NEAR_UNITS(forwardVelocity * curvature, sample.velocity.omega,
                   1e-9_rad_per_s);

  CHECK_NEAR_UNITS(0_mps_sq, sample.acceleration.ax, 1e-9_mps_sq);
  CHECK_NEAR_UNITS(forwardAcceleration, sample.acceleration.ay, 1e-9_mps_sq);

  // The projection accessors recover the path-relative scalars.
  CHECK_NEAR_UNITS(forwardVelocity, sample.ForwardVelocity(), 1e-9_mps);
  CHECK_NEAR_UNITS(forwardAcceleration, sample.ForwardAcceleration(),
                   1e-9_mps_sq);
}

TEST_CASE("HolonomicSampleTest TransformRotatesVelocityAndAcceleration",
          "[wpimath]") {
  // Field-relative velocity/acceleration must rotate with the transform's
  // rotation.
  wpi::math::HolonomicSample sample{
      0_s, wpi::math::Pose2d{0_m, 0_m, 0_rad},
      wpi::math::ChassisVelocities{1_mps, 0_mps, 0.5_rad_per_s},
      wpi::math::ChassisAccelerations{2_mps_sq, 0_mps_sq, 0.3_rad_per_s_sq}};

  auto transformed = sample.Transform(
      wpi::math::Transform2d{wpi::math::Translation2d{3_m, 4_m}, 90_deg});

  CHECK_NEAR_UNITS(3_m, transformed.pose.X(), 1e-9_m);
  CHECK_NEAR_UNITS(4_m, transformed.pose.Y(), 1e-9_m);
  CHECK_NEAR_UNITS(wpi::units::radian_t{std::numbers::pi / 2},
                   transformed.pose.Rotation().Radians(), 1e-9_rad);

  // Velocity/acceleration vectors rotate by +90 degrees; angular terms are
  // unchanged.
  CHECK_NEAR_UNITS(0_mps, transformed.velocity.vx, 1e-9_mps);
  CHECK_NEAR_UNITS(1_mps, transformed.velocity.vy, 1e-9_mps);
  CHECK_NEAR_UNITS(0.5_rad_per_s, transformed.velocity.omega, 1e-9_rad_per_s);
  CHECK_NEAR_UNITS(0_mps_sq, transformed.acceleration.ax, 1e-9_mps_sq);
  CHECK_NEAR_UNITS(2_mps_sq, transformed.acceleration.ay, 1e-9_mps_sq);
  CHECK_NEAR_UNITS(0.3_rad_per_s_sq, transformed.acceleration.alpha,
                   1e-9_rad_per_s_sq);
}

TEST_CASE("HolonomicSampleTest RelativeToRotatesVelocityAndAcceleration",
          "[wpimath]") {
  // relativeTo re-expresses the sample in a frame rotated by the other pose's
  // rotation, so field-relative velocity/acceleration rotate by the negative of
  // it.
  wpi::math::HolonomicSample sample{
      0_s, wpi::math::Pose2d{1_m, 2_m, 90_deg},
      wpi::math::ChassisVelocities{0_mps, 1_mps, 0.5_rad_per_s},
      wpi::math::ChassisAccelerations{0_mps_sq, 2_mps_sq, 0.3_rad_per_s_sq}};

  auto relative = sample.RelativeTo(wpi::math::Pose2d{1_m, 2_m, 90_deg});

  CHECK_NEAR_UNITS(0_m, relative.pose.X(), 1e-9_m);
  CHECK_NEAR_UNITS(0_m, relative.pose.Y(), 1e-9_m);
  CHECK_NEAR_UNITS(0_rad, relative.pose.Rotation().Radians(), 1e-9_rad);

  // Velocity/acceleration vectors rotate by -90 degrees; angular terms are
  // unchanged.
  CHECK_NEAR_UNITS(1_mps, relative.velocity.vx, 1e-9_mps);
  CHECK_NEAR_UNITS(0_mps, relative.velocity.vy, 1e-9_mps);
  CHECK_NEAR_UNITS(0.5_rad_per_s, relative.velocity.omega, 1e-9_rad_per_s);
  CHECK_NEAR_UNITS(2_mps_sq, relative.acceleration.ax, 1e-9_mps_sq);
  CHECK_NEAR_UNITS(0_mps_sq, relative.acceleration.ay, 1e-9_mps_sq);
  CHECK_NEAR_UNITS(0.3_rad_per_s_sq, relative.acceleration.alpha,
                   1e-9_rad_per_s_sq);
}

TEST_CASE("HolonomicSampleTest SplineSampleTransformPreservesForwardScalars",
          "[wpimath]") {
  // Rotating the sample rotates both the heading and the field velocity, so the
  // heading-relative forward scalars (and curvature) are invariant.
  wpi::math::DrivetrainSplineSample sample{0_s,
                                           wpi::math::Pose2d{1_m, 2_m, 20_deg},
                                           2_mps, 1.5_mps_sq, 0.25_rad / 1_m};

  auto transformed = sample.Transform(
      wpi::math::Transform2d{wpi::math::Translation2d{3_m, 4_m}, 35_deg});
  auto relative = sample.RelativeTo(wpi::math::Pose2d{0_m, 0_m, -15_deg});

  for (const auto& s : {transformed, relative}) {
    CHECK_NEAR_UNITS(2_mps, s.ForwardVelocity(), 1e-9_mps);
    CHECK_NEAR_UNITS(1.5_mps_sq, s.ForwardAcceleration(), 1e-9_mps_sq);
    CHECK_NEAR_UNITS(0.25_rad / 1_m, s.curvature, 1e-9_rad / 1_m);
  }
}

TEST_CASE(
    "HolonomicSampleTest DifferentialSampleTransformPreservesWheelSpeeds",
    "[wpimath]") {
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

  CHECK_NEAR_UNITS(sample.leftVelocity, transformed.leftVelocity, 1e-9_mps);
  CHECK_NEAR_UNITS(sample.rightVelocity, transformed.rightVelocity, 1e-9_mps);

  // The field velocity rotates by +90 degrees.
  CHECK_NEAR_UNITS(0_mps, transformed.velocity.vx, 1e-9_mps);
  CHECK_NEAR_UNITS(2_mps, transformed.velocity.vy, 1e-9_mps);
  CHECK_NEAR_UNITS(0.5_rad_per_s, transformed.velocity.omega, 1e-9_rad_per_s);
}
