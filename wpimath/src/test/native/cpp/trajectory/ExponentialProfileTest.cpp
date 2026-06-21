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

TEST_CASE("ExponentialProfileTest SwitchGoalInMiddle", "[wpimath]") {
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

  for (int i = 0; i < 50; ++i) {
    state = CheckDynamics(profile, constraints, feedforward, state, goal);
  }
  CHECK(state != goal);

  goal = {0.0_m, 0.0_mps};
  for (int i = 0; i < 100; ++i) {
    state = CheckDynamics(profile, constraints, feedforward, state, goal);
  }
  CHECK(state == goal);
}

// Checks to make sure that it hits top velocity on long trajectories
TEST_CASE("ExponentialProfileTest TopVelocity", "[wpimath]") {
  wpi::math::ExponentialProfile<wpi::units::meter,
                                wpi::units::volts>::Constraints constraints{
      12_V, -kV / kA, 1 / kA};
  wpi::math::ExponentialProfile<wpi::units::meter, wpi::units::volts> profile{
      constraints};
  wpi::math::SimpleMotorFeedforward<wpi::units::meter> feedforward{
      0_V, 2.5629_V / 1_mps, 0.43277_V / 1_mps_sq, kDt};
  wpi::math::ExponentialProfile<wpi::units::meter, wpi::units::volts>::State
      goal{40_m, 0_mps};
  wpi::math::ExponentialProfile<wpi::units::meter, wpi::units::volts>::State
      state;

  wpi::units::meters_per_second_t maxVelocity = 0_mps;

  for (int i = 0; i < 900; ++i) {
    state = CheckDynamics(profile, constraints, feedforward, state, goal);
    maxVelocity = wpi::units::math::max(state.velocity, maxVelocity);
  }

  CHECK_NEAR_UNITS(constraints.MaxVelocity(), maxVelocity, 1e-5_mps);
  CHECK(state == goal);
}

// Checks to make sure that it hits top velocity on long trajectories
TEST_CASE("ExponentialProfileTest TopVelocityBackward", "[wpimath]") {
  wpi::math::ExponentialProfile<wpi::units::meter,
                                wpi::units::volts>::Constraints constraints{
      12_V, -kV / kA, 1 / kA};
  wpi::math::ExponentialProfile<wpi::units::meter, wpi::units::volts> profile{
      constraints};
  wpi::math::SimpleMotorFeedforward<wpi::units::meter> feedforward{
      0_V, 2.5629_V / 1_mps, 0.43277_V / 1_mps_sq, kDt};
  wpi::math::ExponentialProfile<wpi::units::meter, wpi::units::volts>::State
      goal{-40_m, 0_mps};
  wpi::math::ExponentialProfile<wpi::units::meter, wpi::units::volts>::State
      state;

  wpi::units::meters_per_second_t maxVelocity = 0_mps;

  for (int i = 0; i < 900; ++i) {
    state = CheckDynamics(profile, constraints, feedforward, state, goal);
    maxVelocity = wpi::units::math::min(state.velocity, maxVelocity);
  }

  CHECK_NEAR_UNITS(-constraints.MaxVelocity(), maxVelocity, 1e-5_mps);
  CHECK(state == goal);
}

// Checks to make sure that it hits top velocity on long trajectories
TEST_CASE("ExponentialProfileTest HighInitialVelocity", "[wpimath]") {
  wpi::math::ExponentialProfile<wpi::units::meter,
                                wpi::units::volts>::Constraints constraints{
      12_V, -kV / kA, 1 / kA};
  wpi::math::ExponentialProfile<wpi::units::meter, wpi::units::volts> profile{
      constraints};
  wpi::math::SimpleMotorFeedforward<wpi::units::meter> feedforward{
      0_V, 2.5629_V / 1_mps, 0.43277_V / 1_mps_sq, kDt};
  wpi::math::ExponentialProfile<wpi::units::meter, wpi::units::volts>::State
      goal{40_m, 0_mps};
  wpi::math::ExponentialProfile<wpi::units::meter, wpi::units::volts>::State
      state{0_m, 8_mps};

  for (int i = 0; i < 900; ++i) {
    state = CheckDynamics(profile, constraints, feedforward, state, goal);
  }

  CHECK(state == goal);
}

// Checks to make sure that it hits top velocity on long trajectories
TEST_CASE("ExponentialProfileTest HighInitialVelocityBackward", "[wpimath]") {
  wpi::math::ExponentialProfile<wpi::units::meter,
                                wpi::units::volts>::Constraints constraints{
      12_V, -kV / kA, 1 / kA};
  wpi::math::ExponentialProfile<wpi::units::meter, wpi::units::volts> profile{
      constraints};
  wpi::math::SimpleMotorFeedforward<wpi::units::meter> feedforward{
      0_V, 2.5629_V / 1_mps, 0.43277_V / 1_mps_sq, kDt};
  wpi::math::ExponentialProfile<wpi::units::meter, wpi::units::volts>::State
      goal{-40_m, 0_mps};
  wpi::math::ExponentialProfile<wpi::units::meter, wpi::units::volts>::State
      state{0_m, -8_mps};

  for (int i = 0; i < 900; ++i) {
    state = CheckDynamics(profile, constraints, feedforward, state, goal);
  }

  CHECK(state == goal);
}

TEST_CASE("ExponentialProfileTest TestHeuristic", "[wpimath]") {
  wpi::math::ExponentialProfile<wpi::units::meter,
                                wpi::units::volts>::Constraints constraints{
      12_V, -kV / kA, 1 / kA};
  wpi::math::ExponentialProfile<wpi::units::meter, wpi::units::volts> profile{
      constraints};
  std::vector<std::tuple<
      wpi::math::ExponentialProfile<wpi::units::meter,
                                    wpi::units::volts>::State,  // initial
      wpi::math::ExponentialProfile<wpi::units::meter,
                                    wpi::units::volts>::State,  // goal
      wpi::math::ExponentialProfile<wpi::units::meter,
                                    wpi::units::volts>::State>  // inflection
                                                                // point
              >
      testCases{
          // red > green and purple => always positive => false
          {{0_m, -4_mps}, {0.75_m, -4_mps}, {1.3758_m, 4.4304_mps}},
          {{0_m, -4_mps}, {1.4103_m, 4_mps}, {1.3758_m, 4.4304_mps}},
          {{0.6603_m, 4_mps}, {0.75_m, -4_mps}, {1.3758_m, 4.4304_mps}},
          {{0.6603_m, 4_mps}, {1.4103_m, 4_mps}, {1.3758_m, 4.4304_mps}},

          // purple > red > green => positive if v0 < 0 => c && !d && a
          {{0_m, -4_mps}, {0.5_m, -2_mps}, {0.4367_m, 3.7217_mps}},
          {{0_m, -4_mps}, {0.546_m, 2_mps}, {0.4367_m, 3.7217_mps}},
          {{0.6603_m, 4_mps}, {0.5_m, -2_mps}, {0.5560_m, -2.9616_mps}},
          {{0.6603_m, 4_mps}, {0.546_m, 2_mps}, {0.5560_m, -2.9616_mps}},

          // red < green and purple => always negative => true => !c && !d
          {{0_m, -4_mps}, {-0.75_m, -4_mps}, {-0.7156_m, -4.4304_mps}},
          {{0_m, -4_mps}, {-0.0897_m, 4_mps}, {-0.7156_m, -4.4304_mps}},
          {{0.6603_m, 4_mps}, {-0.75_m, -4_mps}, {-0.7156_m, -4.4304_mps}},
          {{0.6603_m, 4_mps}, {-0.0897_m, 4_mps}, {-0.7156_m, -4.4304_mps}},

          // green > red > purple => positive if vf < 0 => !c && d && b
          {{0_m, -4_mps}, {-0.5_m, -4.5_mps}, {1.095_m, 4.314_mps}},
          {{0_m, -4_mps}, {1.0795_m, 4.5_mps}, {-0.5122_m, -4.351_mps}},
          {{0.6603_m, 4_mps}, {-0.5_m, -4.5_mps}, {1.095_m, 4.314_mps}},
          {{0.6603_m, 4_mps}, {1.0795_m, 4.5_mps}, {-0.5122_m, -4.351_mps}},

          // tests for initial velocity > V/kV
          {{0_m, -8_mps}, {0_m, 0_mps}, {-0.1384_m, 3.342_mps}},
          {{0_m, -8_mps}, {-1_m, 0_mps}, {-0.562_m, -6.792_mps}},
          {{0_m, 8_mps}, {1_m, 0_mps}, {0.562_m, 6.792_mps}},
          {{0_m, 8_mps}, {-1_m, 0_mps}, {-0.785_m, -4.346_mps}},
      };

  for (auto& testCase : testCases) {
    auto state = profile.CalculateInflectionPoint(std::get<0>(testCase),
                                                  std::get<1>(testCase));
    CHECK_NEAR_UNITS(std::get<2>(testCase).position / 1_m, state.position / 1_m,
                     1e-3);
    CHECK_NEAR_UNITS(std::get<2>(testCase).velocity / 1_mps,
                     state.velocity / 1_mps, 1e-3);
  }
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
