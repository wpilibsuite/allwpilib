// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/math/trajectory/ExponentialProfile.hpp"

#include <tuple>
#include <vector>

#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>

#include "wpi/math/controller/SimpleMotorFeedforward.hpp"
#include "wpi/units/acceleration.hpp"
#include "wpi/units/length.hpp"
#include "wpi/units/math.hpp"
#include "wpi/units/velocity.hpp"
#include "wpi/units/voltage.hpp"

static constexpr auto kDt = 10_ms;
static constexpr auto kV = 2.5629_V / 1_mps;
static constexpr auto kA = 0.43277_V / 1_mps_sq;

#define CHECK_NEAR_UNITS(val1, val2, eps) \
  CHECK(wpi::units::math::abs(val1 - val2) <= eps)

wpi::math::ExponentialProfile<wpi::units::meter, wpi::units::volts>::State
CheckDynamics(
    wpi::math::ExponentialProfile<wpi::units::meter, wpi::units::volts> profile,
    wpi::math::ExponentialProfile<wpi::units::meter,
                                  wpi::units::volts>::Constraints constraints,
    wpi::math::SimpleMotorFeedforward<wpi::units::meter> feedforward,
    wpi::math::ExponentialProfile<wpi::units::meter, wpi::units::volts>::State
        current,
    wpi::math::ExponentialProfile<wpi::units::meter, wpi::units::volts>::State
        goal) {
  auto next = profile.Calculate(kDt, current, goal);
  auto signal = feedforward.Calculate(current.velocity, next.velocity);

  CHECK(wpi::units::math::abs(signal) <= (constraints.maxInput + 1e-9_V));

  return next;
}

TEST_CASE("ExponentialProfileTest ReachesGoal", "[wpimath]") {
  wpi::math::ExponentialProfile<wpi::units::meter,
                                wpi::units::volts>::Constraints constraints{
      12_V, -kV / kA, 1 / kA};
  wpi::math::ExponentialProfile<wpi::units::meter, wpi::units::volts> profile{
      constraints};
  wpi::math::SimpleMotorFeedforward<wpi::units::meter> feedforward{
      0_V, 2.5629_V / 1_mps, 0.43277_V / 1_mps_sq, kDt};
  wpi::math::ExponentialProfile<wpi::units::meter, wpi::units::volts>::State
      goal{10_m, 0_mps};
  wpi::math::ExponentialProfile<wpi::units::meter, wpi::units::volts>::State
      state{0_m, 0_mps};

  for (int i = 0; i < 450; ++i) {
    state = CheckDynamics(profile, constraints, feedforward, state, goal);
  }
  CHECK(state == goal);
}

// Tests that decreasing the maximum velocity in the middle when it is already
// moving faster than the new max is handled correctly
TEST_CASE("ExponentialProfileTest PosContinuousUnderVelChange", "[wpimath]") {
  wpi::math::ExponentialProfile<wpi::units::meter,
                                wpi::units::volts>::Constraints constraints{
      12_V, -kV / kA, 1 / kA};
  wpi::math::ExponentialProfile<wpi::units::meter, wpi::units::volts> profile{
      constraints};
  wpi::math::SimpleMotorFeedforward<wpi::units::meter> feedforward{
      0_V, 2.5629_V / 1_mps, 0.43277_V / 1_mps_sq, kDt};
  wpi::math::ExponentialProfile<wpi::units::meter, wpi::units::volts>::State
      goal{10_m, 0_mps};
  wpi::math::ExponentialProfile<wpi::units::meter, wpi::units::volts>::State
      state{0_m, 0_mps};

  for (int i = 0; i < 300; ++i) {
    if (i == 150) {
      constraints.maxInput = 9_V;
      profile =
          wpi::math::ExponentialProfile<wpi::units::meter, wpi::units::volts>{
              constraints};
    }

    state = CheckDynamics(profile, constraints, feedforward, state, goal);
  }
  CHECK(state == goal);
}

// Tests that decreasing the maximum velocity in the middle when it is already
// moving faster than the new max is handled correctly
TEST_CASE("ExponentialProfileTest PosContinuousUnderVelChangeBackward",
          "[wpimath]") {
  wpi::math::ExponentialProfile<wpi::units::meter,
                                wpi::units::volts>::Constraints constraints{
      12_V, -kV / kA, 1 / kA};
  wpi::math::ExponentialProfile<wpi::units::meter, wpi::units::volts> profile{
      constraints};
  wpi::math::SimpleMotorFeedforward<wpi::units::meter> feedforward{
      0_V, 2.5629_V / 1_mps, 0.43277_V / 1_mps_sq, kDt};
  wpi::math::ExponentialProfile<wpi::units::meter, wpi::units::volts>::State
      goal{-10_m, 0_mps};
  wpi::math::ExponentialProfile<wpi::units::meter, wpi::units::volts>::State
      state{0_m, 0_mps};

  for (int i = 0; i < 300; ++i) {
    if (i == 150) {
      constraints.maxInput = 9_V;
      profile =
          wpi::math::ExponentialProfile<wpi::units::meter, wpi::units::volts>{
              constraints};
    }

    state = CheckDynamics(profile, constraints, feedforward, state, goal);
  }
  CHECK(state == goal);
}

// There is some somewhat tricky code for dealing with going backwards
TEST_CASE("ExponentialProfileTest Backwards", "[wpimath]") {
  wpi::math::ExponentialProfile<wpi::units::meter,
                                wpi::units::volts>::Constraints constraints{
      12_V, -kV / kA, 1 / kA};
  wpi::math::ExponentialProfile<wpi::units::meter, wpi::units::volts> profile{
      constraints};
  wpi::math::SimpleMotorFeedforward<wpi::units::meter> feedforward{
      0_V, 2.5629_V / 1_mps, 0.43277_V / 1_mps_sq, kDt};
  wpi::math::ExponentialProfile<wpi::units::meter, wpi::units::volts>::State
      goal{-10_m, 0_mps};
  wpi::math::ExponentialProfile<wpi::units::meter, wpi::units::volts>::State
      state;

  for (int i = 0; i < 400; ++i) {
    state = CheckDynamics(profile, constraints, feedforward, state, goal);
  }
  CHECK(state == goal);
}

TEST_CASE("ExponentialProfileTest TimingToCurrent", "[wpimath]") {
  wpi::math::ExponentialProfile<wpi::units::meter,
                                wpi::units::volts>::Constraints constraints{
      12_V, -kV / kA, 1 / kA};
  wpi::math::ExponentialProfile<wpi::units::meter, wpi::units::volts> profile{
      constraints};
  wpi::math::SimpleMotorFeedforward<wpi::units::meter> feedforward{
      0_V, 2.5629_V / 1_mps, 0.43277_V / 1_mps_sq, kDt};
  wpi::math::ExponentialProfile<wpi::units::meter, wpi::units::volts>::State
      goal{2_m, 0_mps};
  wpi::math::ExponentialProfile<wpi::units::meter, wpi::units::volts>::State
      state{0_m, 0_mps};

  for (int i = 0; i < 900; ++i) {
    state = CheckDynamics(profile, constraints, feedforward, state, goal);
    CHECK_NEAR_UNITS(profile.TimeLeftUntil(state, state), 0_s, 2e-2_s);
  }

  CHECK(state == goal);
}

TEST_CASE("ExponentialProfileTest TimingToGoal", "[wpimath]") {
  wpi::math::ExponentialProfile<wpi::units::meter,
                                wpi::units::volts>::Constraints constraints{
      12_V, -kV / kA, 1 / kA};
  wpi::math::ExponentialProfile<wpi::units::meter, wpi::units::volts> profile{
      constraints};
  wpi::math::SimpleMotorFeedforward<wpi::units::meter> feedforward{
      0_V, 2.5629_V / 1_mps, 0.43277_V / 1_mps_sq, kDt};
  wpi::math::ExponentialProfile<wpi::units::meter, wpi::units::volts>::State
      goal{2_m, 0_mps};
  wpi::math::ExponentialProfile<wpi::units::meter, wpi::units::volts>::State
      state{0_m, 0_mps};

  auto prediction = profile.TimeLeftUntil(state, goal);
  auto reachedGoal = false;

  for (int i = 0; i < 900; ++i) {
    state = CheckDynamics(profile, constraints, feedforward, state, goal);
    if (!reachedGoal && state == goal) {
      CHECK_NEAR_UNITS(prediction, i * 10_ms, 250_ms);
      reachedGoal = true;
    }
  }

  CHECK(state == goal);
}

TEST_CASE("ExponentialProfileTest TimingToNegativeGoal", "[wpimath]") {
  wpi::math::ExponentialProfile<wpi::units::meter,
                                wpi::units::volts>::Constraints constraints{
      12_V, -kV / kA, 1 / kA};
  wpi::math::ExponentialProfile<wpi::units::meter, wpi::units::volts> profile{
      constraints};
  wpi::math::SimpleMotorFeedforward<wpi::units::meter> feedforward{
      0_V, 2.5629_V / 1_mps, 0.43277_V / 1_mps_sq, kDt};
  wpi::math::ExponentialProfile<wpi::units::meter, wpi::units::volts>::State
      goal{-2_m, 0_mps};
  wpi::math::ExponentialProfile<wpi::units::meter, wpi::units::volts>::State
      state{0_m, 0_mps};

  auto prediction = profile.TimeLeftUntil(state, goal);
  auto reachedGoal = false;

  for (int i = 0; i < 900; ++i) {
    state = CheckDynamics(profile, constraints, feedforward, state, goal);
    if (!reachedGoal && state == goal) {
      CHECK_NEAR_UNITS(prediction, i * 10_ms, 250_ms);
      reachedGoal = true;
    }
  }

  CHECK(state == goal);
}
