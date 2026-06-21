// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/math/controller/AntiTipping.hpp"

#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>

#include "wpi/math/TestAssertions.hpp"

static constexpr double kTolerance = 1e-6;

// Shared constructor parameters used by all tests
static constexpr wpi::units::unit_t<wpi::math::AntiTipping::kp_unit> kKp{0.1};
static constexpr wpi::units::radian_t kThreshold = 3_deg;
static constexpr wpi::units::meters_per_second_t kMaxSpeed = 2_mps;

TEST_CASE("AntiTippingTest BelowThresholdGeneratesNoCorrection", "[wpimath]") {
  wpi::math::AntiTipping antiTipping{kKp, kThreshold, kMaxSpeed};
  auto correction =
      antiTipping.Calculate(wpi::math::Rotation3d{1_deg, 1_deg, 0_deg});

  CHECK_NEAR(0.0, correction.vx.value(), kTolerance);
  CHECK_NEAR(0.0, correction.vy.value(), kTolerance);
}

TEST_CASE("AntiTippingTest ForwardTipDrivesForward", "[wpimath]") {
  wpi::math::AntiTipping antiTipping{kKp, kThreshold, kMaxSpeed};
  auto correction =
      antiTipping.Calculate(wpi::math::Rotation3d{0_deg, 10_deg, 0_deg});

  CHECK(correction.vx.value() > 0.0);
  CHECK_NEAR(0.0, correction.vy.value(), kTolerance);
}

TEST_CASE("AntiTippingTest BackwardTipDrivesBackward", "[wpimath]") {
  wpi::math::AntiTipping antiTipping{kKp, kThreshold, kMaxSpeed};
  auto correction =
      antiTipping.Calculate(wpi::math::Rotation3d{0_deg, -10_deg, 0_deg});

  CHECK(correction.vx.value() < 0.0);
  CHECK_NEAR(0.0, correction.vy.value(), kTolerance);
}

TEST_CASE("AntiTippingTest RightRollDrivesRight", "[wpimath]") {
  wpi::math::AntiTipping antiTipping{kKp, kThreshold, kMaxSpeed};
  auto correction =
      antiTipping.Calculate(wpi::math::Rotation3d{15_deg, 0_deg, 0_deg});

  CHECK_NEAR(0.0, correction.vx.value(), kTolerance);
  CHECK(correction.vy.value() < 0.0);
}

TEST_CASE("AntiTippingTest LeftRollDrivesLeft", "[wpimath]") {
  wpi::math::AntiTipping antiTipping{kKp, kThreshold, kMaxSpeed};
  auto correction =
      antiTipping.Calculate(wpi::math::Rotation3d{-15_deg, 0_deg, 0_deg});

  CHECK_NEAR(0.0, correction.vx.value(), kTolerance);
  CHECK(correction.vy.value() > 0.0);
}
