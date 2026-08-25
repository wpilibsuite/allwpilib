// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/math/trajectory/TrapezoidProfile.hpp"

#include <stdexcept>
#include <type_traits>

#include <catch2/catch_test_macros.hpp>

#include "wpi/math/TestAssertions.hpp"
#include "wpi/units/acceleration.hpp"
#include "wpi/units/length.hpp"
#include "wpi/units/time.hpp"
#include "wpi/units/velocity.hpp"

static constexpr auto DT = 10_ms;

// Expressions are improperly handled without parentheses.
#define CHECK_LT_OR_NEAR_UNITS(val1, val2, eps) \
  if ((val1) <= (val2)) {                       \
    CHECK((val1) <= (val2));                    \
  } else {                                      \
    CHECK_UNITS_NEAR((val1), (val2), (eps));    \
  }

// Ensure the state only changes within the control effort available to us.
void CheckFeasible(
    const wpi::math::TrapezoidProfile<wpi::units::meters_>::State& initial,
    const wpi::math::TrapezoidProfile<wpi::units::meters_>::State& next,
    const wpi::math::TrapezoidProfile<wpi::units::meters_>::Acceleration_t
        maxAccel) {
  auto deltaV = next.velocity - initial.velocity;
  auto deltaX = next.position - initial.position;

  // We can't check for an exact state because the profile may input sign
  // between timestemps.
  CHECK_LT_OR_NEAR_UNITS(wpi::units::abs(deltaV), DT * maxAccel, 2e-8_mps);
  CHECK_LT_OR_NEAR_UNITS(
      wpi::units::abs(deltaX),
      wpi::units::abs(initial.velocity) * DT + maxAccel / 2.0 * DT * DT,
      1e-8_m);
}

TEST_CASE("TrapezoidProfileTest ConstraintsRequirePositiveValues",
          "[wpimath]") {
  using Constraints =
      wpi::math::TrapezoidProfile<wpi::units::meters_>::Constraints;

  STATIC_REQUIRE(!std::is_default_constructible_v<Constraints>);
  CHECK_THROWS_AS(Constraints(0_mps, 1_mps2), std::domain_error);
  CHECK_THROWS_AS(Constraints(1_mps, 0_mps2), std::domain_error);
  CHECK_THROWS_AS(Constraints(-1_mps, 1_mps2), std::domain_error);
  CHECK_THROWS_AS(Constraints(1_mps, -1_mps2), std::domain_error);
  CHECK_NOTHROW(Constraints(1_mps, 1_mps2));
}

TEST_CASE("TrapezoidProfileTest CheckTiming", "[wpimath]") {
  wpi::math::TrapezoidProfile<wpi::units::meters_>::Constraints constraints{
      1.75_mps, 0.75_mps2};
  // Make sure we hit the velocity cap and the profile has input shape -0-.
  wpi::math::TrapezoidProfile<wpi::units::meters_>::State goal{12_m, -1_mps};
  wpi::math::TrapezoidProfile<wpi::units::meters_>::State state{0_m, 1_mps};

  wpi::math::TrapezoidProfile<wpi::units::meters_> profile{constraints};
  profile.Calculate(DT, state, goal);
  auto profileTime = profile.Duration();

  CHECK_UNITS_NEAR(profileTime, 9.952380952380953_s, 1e-10_s);
  CHECK(profileTime == profile.TimeLeftUntil(state, goal));
  profile.TimeLeftUntil(goal, goal);
  CHECK(profileTime == profile.Duration());
}

TEST_CASE("TrapezoidProfileTest ReachesGoal", "[wpimath]") {
  wpi::math::TrapezoidProfile<wpi::units::meters_>::Constraints constraints{
      1.75_mps, 0.75_mps2};
  wpi::math::TrapezoidProfile<wpi::units::meters_>::State goal{3_m, 0_mps};
  wpi::math::TrapezoidProfile<wpi::units::meters_>::State state{};

  wpi::math::TrapezoidProfile<wpi::units::meters_> profile{constraints};
  for (int i = 0; i < 450; ++i) {
    auto newState = profile.Calculate(DT, state, goal);
    CheckFeasible(state, newState, constraints.maxAcceleration);
    state = newState;
  }
  CHECK(state == goal);
}

TEST_CASE("TrapezoidProfileTest Backwards", "[wpimath]") {
  wpi::math::TrapezoidProfile<wpi::units::meters_>::Constraints constraints{
      0.75_mps, 0.75_mps2};
  wpi::math::TrapezoidProfile<wpi::units::meters_>::State goal{-2_m, 0_mps};
  wpi::math::TrapezoidProfile<wpi::units::meters_>::State state;

  wpi::math::TrapezoidProfile<wpi::units::meters_> profile{constraints};
  for (int i = 0; i < 400; ++i) {
    auto newState = profile.Calculate(DT, state, goal);
    CheckFeasible(state, newState, constraints.maxAcceleration);
    state = newState;
  }
  CHECK(state == goal);
}

// Test the forwards case for an invalid initial velocity with the profile sign.
TEST_CASE("TrapezoidProfileTest CheckLargeVelocitySameSignAsPeak",
          "[wpimath]") {
  wpi::math::TrapezoidProfile<wpi::units::meters_>::Constraints constraints{
      1.75_mps, 0.75_mps2};
  // Make sure we hit the velocity cap and the profile has input shape -0-.
  wpi::math::TrapezoidProfile<wpi::units::meters_>::State goal{12_m, 0_mps};
  wpi::math::TrapezoidProfile<wpi::units::meters_>::State state{0_m, 3_mps};

  wpi::math::TrapezoidProfile<wpi::units::meters_> profile{constraints};

  int plateauCount = 0;
  // Profile is ~7.5s.
  for (int i = 0; i < 1000; i++) {
    auto newState = profile.Calculate(DT, state, goal);
    CheckFeasible(state, newState, constraints.maxAcceleration);
    if (newState.velocity == constraints.maxVelocity) {
      plateauCount++;
    }
    state = newState;
  }
  // Make sure it plateaued at the correct velocity, not just passed it.
  CHECK(plateauCount > 5);

  CHECK(state == goal);
}

// Test the backwards case for an invalid initial velocity with the profile
// sign.
TEST_CASE("TrapezoidProfileTest CheckLargeVelocitySameSignAsPeakBackwards",
          "[wpimath]") {
  wpi::math::TrapezoidProfile<wpi::units::meters_>::Constraints constraints{
      1.75_mps, 0.75_mps2};
  // Make sure we hit the velocity cap and the profile has input shape +0+.
  wpi::math::TrapezoidProfile<wpi::units::meters_>::State goal{-12_m, 0_mps};
  wpi::math::TrapezoidProfile<wpi::units::meters_>::State state{0_m, -3_mps};

  wpi::math::TrapezoidProfile<wpi::units::meters_> profile{constraints};

  int plateauCount = 0;
  // Profile is ~7.5s.
  for (int i = 0; i < 1000; i++) {
    auto newState = profile.Calculate(DT, state, goal);
    CheckFeasible(state, newState, constraints.maxAcceleration);
    if (newState.velocity == -constraints.maxVelocity) {
      plateauCount++;
    }
    state = newState;
  }
  // Make sure it plateaued at the correct velocity, not just passed it.
  CHECK(plateauCount > 5);

  CHECK(state == goal);
}

// Test the forwards case for an invalid initial velocity with a sign
// opposite the profile sign.
TEST_CASE("TrapezoidProfileTest CheckLargeVelocityOppositePeak", "[wpimath]") {
  wpi::math::TrapezoidProfile<wpi::units::meters_>::Constraints constraints{
      1.75_mps, 0.75_mps2};
  // Make sure we hit the velocity cap and the profile has input shape -0-.
  wpi::math::TrapezoidProfile<wpi::units::meters_>::State goal{12_m, 0_mps};
  wpi::math::TrapezoidProfile<wpi::units::meters_>::State state{0_m, -3_mps};

  wpi::math::TrapezoidProfile<wpi::units::meters_> profile{constraints};

  int plateauCount = 0;
  // ~17 second trajectory.
  for (int i = 0; i < 1700; i++) {
    auto newState = profile.Calculate(DT, state, goal);

    CheckFeasible(state, newState, constraints.maxAcceleration);
    if (newState.velocity == constraints.maxVelocity) {
      plateauCount++;
    }
    state = newState;
  }
  // Make sure it plateaued at the correct velocity, not just passed it.
  CHECK(plateauCount > 5);

  CHECK(state == goal);
}

// Test the backwards case for an invalid initial velocity with a sign
// opposite the profile sign.
TEST_CASE("TrapezoidProfileTest CheckLargeVelocityOppositePeakBackwards",
          "[wpimath]") {
  wpi::math::TrapezoidProfile<wpi::units::meters_>::Constraints constraints{
      1.75_mps, 0.75_mps2};
  // Make sure we hit the velocity cap and the profile has input shape -0-.
  wpi::math::TrapezoidProfile<wpi::units::meters_>::State goal{-12_m, 0_mps};
  wpi::math::TrapezoidProfile<wpi::units::meters_>::State state{0_m, 3_mps};

  wpi::math::TrapezoidProfile<wpi::units::meters_> profile{constraints};

  int plateauCount = 0;
  for (int i = 0; i < 1700; i++) {
    auto newState = profile.Calculate(DT, state, goal);
    CheckFeasible(state, newState, constraints.maxAcceleration);
    if (newState.velocity == -constraints.maxVelocity) {
      plateauCount++;
    }
    state = newState;
  }
  // Make sure it plateaued at the correct velocity, not just passed it.
  CHECK(plateauCount > 5);

  CHECK(state == goal);
}

// Test the forwards case for displacement equal to the threshold displacement.
TEST_CASE("TrapezoidProfileTest CheckSignAtThreshold", "[wpimath]") {
  wpi::math::TrapezoidProfile<wpi::units::meters_>::Constraints constraints{
      4_mps, 4_mps2};
  wpi::math::TrapezoidProfile<wpi::units::meters_>::State goal{1_m, 1_mps};
  wpi::math::TrapezoidProfile<wpi::units::meters_>::State state{0_m, 3_mps};

  wpi::math::TrapezoidProfile<wpi::units::meters_> profile{constraints};

  // Normal profile is 0.5s, and an incorrect implementation might repeat.
  for (int i = 0; i < 52; i++) {
    auto newState = profile.Calculate(DT, state, goal);
    CheckFeasible(state, newState, constraints.maxAcceleration);
    state = newState;
  }

  // The "chattering" failure mode won't reach the goal.
  CHECK(state == goal);
}

// Test the backwards case for displacement equal to the threshold displacement.
TEST_CASE("TrapezoidProfileTest CheckSignAtThresholdBackwards", "[wpimath]") {
  wpi::math::TrapezoidProfile<wpi::units::meters_>::Constraints constraints{
      4_mps, 4_mps2};
  wpi::math::TrapezoidProfile<wpi::units::meters_>::State goal{-1_m, -1_mps};
  wpi::math::TrapezoidProfile<wpi::units::meters_>::State state{0_m, -3_mps};

  wpi::math::TrapezoidProfile<wpi::units::meters_> profile{constraints};

  // Normal profile is 0.5s, and an incorrect implementation might repeat.
  for (int i = 0; i < 52; i++) {
    auto newState = profile.Calculate(DT, state, goal);
    CheckFeasible(state, newState, constraints.maxAcceleration);
    state = newState;
  }

  // The "chattering" failure mode won't reach the goal.
  CHECK(state == goal);
}

// This is the case that generated a broken profile in the old impl.
TEST_CASE("TrapezoidProfileTest LargeVelocityAndSmallPositionDelta",
          "[wpimath]") {
  wpi::math::TrapezoidProfile<wpi::units::meters_>::Constraints constraints{
      1.75_mps, 0.75_mps2};
  wpi::math::TrapezoidProfile<wpi::units::meters_>::State goal{0.01_m, 0_mps};
  wpi::math::TrapezoidProfile<wpi::units::meters_>::State state{0_m, 1_mps};

  wpi::math::TrapezoidProfile<wpi::units::meters_> profile{constraints};
  for (int i = 0; i < 450; ++i) {
    auto newState = profile.Calculate(DT, state, goal);
    CheckFeasible(state, newState, constraints.maxAcceleration);
    state = newState;
  }
  CHECK(state == goal);
}

TEST_CASE("TrapezoidProfileTest LargeVelocityAndSmallPositionDeltaBackwards",
          "[wpimath]") {
  wpi::math::TrapezoidProfile<wpi::units::meters_>::Constraints constraints{
      1.75_mps, 0.75_mps2};
  wpi::math::TrapezoidProfile<wpi::units::meters_>::State goal{-0.01_m, 0_mps};
  wpi::math::TrapezoidProfile<wpi::units::meters_>::State state{0_m, -2_mps};

  wpi::math::TrapezoidProfile<wpi::units::meters_> profile{constraints};
  for (int i = 0; i < 700; ++i) {
    auto newState = profile.Calculate(DT, state, goal);
    CheckFeasible(state, newState, constraints.maxAcceleration);
    state = newState;
  }
  CHECK(state == goal);
}

TEST_CASE("TrapezoidProfileTest SwitchGoalInMiddle", "[wpimath]") {
  wpi::math::TrapezoidProfile<wpi::units::meters_>::Constraints constraints{
      0.75_mps, 0.75_mps2};
  wpi::math::TrapezoidProfile<wpi::units::meters_>::State goal{-2_m, 0_mps};
  wpi::math::TrapezoidProfile<wpi::units::meters_>::State state;

  wpi::math::TrapezoidProfile<wpi::units::meters_> profile{constraints};
  for (int i = 0; i < 200; ++i) {
    auto newState = profile.Calculate(DT, state, goal);
    CheckFeasible(state, newState, constraints.maxAcceleration);
    state = newState;
  }
  CHECK(state != goal);

  goal = {0.0_m, 0.0_mps};
  profile = wpi::math::TrapezoidProfile<wpi::units::meters_>{constraints};
  for (int i = 0; i < 550; ++i) {
    auto newState = profile.Calculate(DT, state, goal);
    CheckFeasible(state, newState, constraints.maxAcceleration);
    state = newState;
  }
  CHECK(state == goal);
}

// Checks to make sure that it hits top velocity
TEST_CASE("TrapezoidProfileTest TopVelocity", "[wpimath]") {
  wpi::math::TrapezoidProfile<wpi::units::meters_>::Constraints constraints{
      0.75_mps, 0.75_mps2};
  wpi::math::TrapezoidProfile<wpi::units::meters_>::State goal{4_m, 0_mps};
  wpi::math::TrapezoidProfile<wpi::units::meters_>::State state;

  wpi::math::TrapezoidProfile<wpi::units::meters_> profile{constraints};
  for (int i = 0; i < 200; ++i) {
    auto newState = profile.Calculate(DT, state, goal);
    CheckFeasible(state, newState, constraints.maxAcceleration);
    state = newState;
  }
  CHECK_UNITS_NEAR(constraints.maxVelocity, state.velocity, 10e-5_mps);

  profile = wpi::math::TrapezoidProfile<wpi::units::meters_>{constraints};
  for (int i = 0; i < 2000; ++i) {
    auto newState = profile.Calculate(DT, state, goal);
    CheckFeasible(state, newState, constraints.maxAcceleration);
    state = newState;
  }
  CHECK(state == goal);
}

TEST_CASE("TrapezoidProfileTest TimingToCurrent", "[wpimath]") {
  wpi::math::TrapezoidProfile<wpi::units::meters_>::Constraints constraints{
      0.75_mps, 0.75_mps2};
  wpi::math::TrapezoidProfile<wpi::units::meters_>::State goal{2_m, 0_mps};
  wpi::math::TrapezoidProfile<wpi::units::meters_>::State state;

  wpi::math::TrapezoidProfile<wpi::units::meters_> profile{constraints};
  for (int i = 0; i < 400; i++) {
    auto newState = profile.Calculate(DT, state, goal);
    CheckFeasible(state, newState, constraints.maxAcceleration);
    state = newState;
    CHECK_UNITS_NEAR(profile.TimeLeftUntil(state, state), 0_s, 2e-2_s);
  }
}

TEST_CASE("TrapezoidProfileTest TimingToGoal", "[wpimath]") {
  using wpi::units::unit_cast;

  wpi::math::TrapezoidProfile<wpi::units::meters_>::Constraints constraints{
      0.75_mps, 0.75_mps2};
  wpi::math::TrapezoidProfile<wpi::units::meters_>::State goal{2_m, 0_mps};
  wpi::math::TrapezoidProfile<wpi::units::meters_>::State state{0_m, 0_mps};

  wpi::math::TrapezoidProfile<wpi::units::meters_> profile{constraints};

  auto predictedTimeLeft = profile.TimeLeftUntil(state, goal);
  bool reachedGoal = false;
  for (int i = 0; i < 400; i++) {
    auto newState = profile.Calculate(DT, state, goal);
    CheckFeasible(state, newState, constraints.maxAcceleration);
    state = newState;
    if (!reachedGoal && state == goal) {
      // Expected value using for loop index is just an approximation since the
      // time left in the profile doesn't increase linearly at the endpoints
      CHECK_NEAR(unit_cast<double>(predictedTimeLeft), i / 100.0, 0.25);
      reachedGoal = true;
    }
  }
}

TEST_CASE("TrapezoidProfileTest TimingToNegativeGoal", "[wpimath]") {
  using wpi::units::unit_cast;

  wpi::math::TrapezoidProfile<wpi::units::meters_>::Constraints constraints{
      0.75_mps, 0.75_mps2};
  wpi::math::TrapezoidProfile<wpi::units::meters_>::State goal{-2_m, 0_mps};
  wpi::math::TrapezoidProfile<wpi::units::meters_>::State state{0_m, 0_mps};

  wpi::math::TrapezoidProfile<wpi::units::meters_> profile{constraints};

  auto predictedTimeLeft = profile.TimeLeftUntil(state, goal);
  bool reachedGoal = false;
  for (int i = 0; i < 400; i++) {
    auto newState = profile.Calculate(DT, state, goal);
    CheckFeasible(state, newState, constraints.maxAcceleration);
    state = newState;
    if (!reachedGoal && state == goal) {
      // Expected value using for loop index is just an approximation since the
      // time left in the profile doesn't increase linearly at the endpoints
      CHECK_NEAR(unit_cast<double>(predictedTimeLeft), i / 100.0, 0.25);
      reachedGoal = true;
    }
  }
}

TEST_CASE("TrapezoidProfileTest GoalVelocityConstraints", "[wpimath]") {
  wpi::math::TrapezoidProfile<wpi::units::meters_>::Constraints constraints{
      0.75_mps, 0.75_mps2};
  wpi::math::TrapezoidProfile<wpi::units::meters_>::State goal{10_m, 5_mps};
  wpi::math::TrapezoidProfile<wpi::units::meters_>::State state{0_m, 0.75_mps};

  wpi::math::TrapezoidProfile<wpi::units::meters_> profile{constraints};

  for (int i = 0; i < 1400; ++i) {
    auto newState = profile.Calculate(DT, state, goal);
    CheckFeasible(state, newState, constraints.maxAcceleration);
    state = newState;
    CHECK(wpi::units::abs(state.velocity) <= constraints.maxVelocity);
  }
}

TEST_CASE("TrapezoidProfileTest NegativeGoalVelocityConstraints", "[wpimath]") {
  wpi::math::TrapezoidProfile<wpi::units::meters_>::Constraints constraints{
      0.75_mps, 0.75_mps2};
  wpi::math::TrapezoidProfile<wpi::units::meters_>::State goal{10_m, -5_mps};
  wpi::math::TrapezoidProfile<wpi::units::meters_>::State state{0_m, 0.75_mps};

  wpi::math::TrapezoidProfile<wpi::units::meters_> profile{constraints};

  for (int i = 0; i < 1600; ++i) {
    auto newState = profile.Calculate(DT, state, goal);
    CheckFeasible(state, newState, constraints.maxAcceleration);
    state = newState;
    CHECK(wpi::units::abs(state.velocity) <= constraints.maxVelocity);
  }
}
