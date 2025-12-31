// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/math/trajectory/TrapezoidProfile.hpp"

#include <chrono>
#include <cmath>

#include <gtest/gtest.h>

#include "wpi/units/acceleration.hpp"
#include "wpi/units/length.hpp"
#include "wpi/units/math.hpp"
#include "wpi/units/velocity.hpp"

static constexpr auto kDt = 10_ms;

#define EXPECT_NEAR_UNITS(val1, val2, eps) \
  EXPECT_LE(wpi::units::math::abs(val1 - val2), eps)

#define EXPECT_LT_OR_NEAR_UNITS(val1, val2, eps) \
  if (val1 <= val2) {                            \
    EXPECT_LE(val1, val2);                       \
  } else {                                       \
    EXPECT_NEAR_UNITS(val1, val2, eps);          \
  }

// Ensure the state only changes within the control effort available to us.
void CheckFeasible(
		const wpi::math::TrapezoidProfile<wpi::units::meter>::State& initial,
		const wpi::math::TrapezoidProfile<wpi::units::meter>::State& next,
		const wpi::math::TrapezoidProfile<wpi::units::meter>::Acceleration_t maxAccel) {

	auto deltaV = next.velocity - initial.velocity;
	auto deltaX = next.position - initial.position;

	// We can't check for an exact state because the profile may input sign between timestemps.
	EXPECT_LT_OR_NEAR_UNITS(wpi::units::math::abs(deltaV), kDt * maxAccel, 2e-8_mps);
	EXPECT_LT_OR_NEAR_UNITS(wpi::units::math::abs(deltaX), wpi::units::math::abs(initial.velocity) * kDt + maxAccel / 2.0 * kDt * kDt, wpi::units::meter_t{1e-8}); // Bad
	EXPECT_LT_OR_NEAR_UNITS(wpi::units::math::abs(deltaX), wpi::units::meter_t{wpi::units::math::abs(initial.velocity) * kDt + maxAccel / 2.0 * kDt * kDt}, wpi::units::meter_t{1e-8}); // Works now
	// EXPECT_LE(wpi::units::meter_t(wpi::units::math::abs(deltaX) - (wpi::units::math::abs(initial.velocity) * kDt + maxAccel / 2.0 * kDt * kDt)).value(), 1e-8); // Good
	// std::cout << std::format("{}\n", (2e-8_m - wpi::units::math::abs(wpi::units::math::abs(initial.velocity) * kDt + maxAccel / 2.0 * kDt * kDt - wpi::units::math::abs(deltaX))).value());
}

TEST(TrapezoidProfileTest, CheckTiming) {
  wpi::math::TrapezoidProfile<wpi::units::meter>::Constraints constraints{
      1.75_mps, 0.75_mps_sq};
  // Make sure we hit the velocity cap and the profile has input shape -0-.
  wpi::math::TrapezoidProfile<wpi::units::meter>::State goal{12_m, -1_mps};
  wpi::math::TrapezoidProfile<wpi::units::meter>::State state{0_m, 1_mps};

  wpi::math::TrapezoidProfile<wpi::units::meter> profile{constraints};

  profile.Calculate(kDt, state, goal);

  auto profileTime = profile.TotalTime();

  EXPECT_NEAR_UNITS(profileTime, 9.952380952380953_s, 1e-10_s);
  EXPECT_EQ(profileTime, profile.TimeLeftUntil(state, goal));
  profile.TimeLeftUntil(goal, {goal.position, goal.velocity});
  EXPECT_EQ(profileTime, profile.TotalTime());
}

TEST(TrapezoidProfileTest, ReachesGoal) {
  wpi::math::TrapezoidProfile<wpi::units::meter>::Constraints constraints{
      1.75_mps, 0.75_mps_sq};
  wpi::math::TrapezoidProfile<wpi::units::meter>::State goal{3_m, 0_mps};
  wpi::math::TrapezoidProfile<wpi::units::meter>::State state{};

  wpi::math::TrapezoidProfile<wpi::units::meter> profile{constraints};
  for (int i = 0; i < 450; ++i) {
    state = profile.Calculate(kDt, state, goal);
  }
  EXPECT_EQ(state, goal);
}

TEST(TrapezoidProfileTest, Backwards) {
  wpi::math::TrapezoidProfile<wpi::units::meter>::Constraints constraints{
      0.75_mps, 0.75_mps_sq};
  wpi::math::TrapezoidProfile<wpi::units::meter>::State goal{-2_m, 0_mps};
  wpi::math::TrapezoidProfile<wpi::units::meter>::State state;

  wpi::math::TrapezoidProfile<wpi::units::meter> profile{constraints};
  for (int i = 0; i < 400; ++i) {
    state = profile.Calculate(kDt, state, goal);
  }
  EXPECT_EQ(state, goal);
}

// Test the forwards case for a positive, invalid initial velocity.
TEST(TrapezoidProfileTest, CheckLargeVelocityAbovePeak) {
  wpi::math::TrapezoidProfile<wpi::units::meter>::Constraints constraints{
      1.75_mps, 0.75_mps_sq};
  // Make sure we hit the velocity cap and the profile has input shape -0-.
  wpi::math::TrapezoidProfile<wpi::units::meter>::State goal{12_m, 0_mps};
  wpi::math::TrapezoidProfile<wpi::units::meter>::State state{0_m, 3_mps};

  wpi::math::TrapezoidProfile<wpi::units::meter> profile{constraints};

  int plateauCount = 0;
  // Profile is ~7.5s.
  for (int i = 0; i < 1000; i++) {
	auto newState = profile.Calculate(kDt, state, goal);
	CheckFeasible(state, newState, constraints.maxAcceleration);
	if (newState.velocity == constraints.maxVelocity) {
	  plateauCount++;
	}
	state = newState;
  }
  // Make sure it plateaued at the correct velocity, not just passed it.
  EXPECT_GT(plateauCount, 5);

  EXPECT_EQ(state, goal);
}

// Test the backwards case for a negative invalid initial velocity.
TEST(TrapezoidProfileTest, CheckLargeVelocityAbovePeakBackwards) {
  wpi::math::TrapezoidProfile<wpi::units::meter>::Constraints constraints{
      1.75_mps, 0.75_mps_sq};
  // Make sure we hit the velocity cap and the profile has input shape +0+.
  wpi::math::TrapezoidProfile<wpi::units::meter>::State goal{-12_m, 0_mps};
  wpi::math::TrapezoidProfile<wpi::units::meter>::State state{0_m, -3_mps};

  wpi::math::TrapezoidProfile<wpi::units::meter> profile{constraints};

  int plateauCount = 0;
  // Profile is ~7.5s.
  for (int i = 0; i < 1000; i++) {
	auto newState = profile.Calculate(kDt, state, goal);
	CheckFeasible(state, newState, constraints.maxAcceleration);
	if (newState.velocity == -constraints.maxVelocity) {
	  plateauCount++;
	}
	state = newState;
  }
  // Make sure it plateaued at the correct velocity, not just passed it.
  EXPECT_GT(plateauCount, 5);

  EXPECT_EQ(state, goal);
}

// Test forwards case for a negative, invalid initial velocity.
TEST(TrapezoidProfileTest, CheckLargeVelocityBelowPeak) {
  wpi::math::TrapezoidProfile<wpi::units::meter>::Constraints constraints{
      1.75_mps, 0.75_mps_sq};
  // Make sure we hit the velocity cap and the profile has input shape -0-.
  wpi::math::TrapezoidProfile<wpi::units::meter>::State goal{12_m, 0_mps};
  wpi::math::TrapezoidProfile<wpi::units::meter>::State state{0_m, -3_mps};

  wpi::math::TrapezoidProfile<wpi::units::meter> profile{constraints};

  int plateauCount = 0;
  for (int i = 0; i < 500; i++) {
	std::cout << std::format("i: {}\n", i);
	auto newState = profile.Calculate(kDt, state, goal, true);
	auto posDelta = wpi::units::math::abs(newState.position - state.position);
	auto posBound = wpi::units::math::abs(state.velocity) * kDt + constraints.maxAcceleration / 2.0 * kDt * kDt;
	EXPECT_LT_OR_NEAR_UNITS(posDelta, posBound, wpi::units::meter_t{1e-9}); // Good
	EXPECT_LT_OR_NEAR_UNITS(posDelta, wpi::units::meter_t{posBound}, wpi::units::meter_t{1e-9}); // Also good
	EXPECT_LT_OR_NEAR_UNITS(wpi::units::math::abs(newState.position - state.position), wpi::units::math::abs(state.velocity) * kDt + constraints.maxAcceleration / 2.0 * kDt * kDt, wpi::units::meter_t{1e-8}); // Bad
	EXPECT_LT_OR_NEAR_UNITS(wpi::units::math::abs(newState.position - state.position), wpi::units::meter_t{wpi::units::math::abs(state.velocity) * kDt + constraints.maxAcceleration / 2.0 * kDt * kDt}, wpi::units::meter_t{1e-9}); // Good
	
	
	std::cout << std::format("Position change: {}\nPosition change bound: {}\nVelocity change: {}\nVelocity change bound: {}\n",
			posDelta.value(),
			wpi::units::meter_t{posBound}.value(),
			wpi::units::math::abs(newState.velocity - state.velocity).value(),
			wpi::units::meters_per_second_t(constraints.maxAcceleration * kDt).value());
			
	// CheckFeasible(state, newState, constraints.maxAcceleration);
	if (newState.velocity == constraints.maxVelocity) {
	  plateauCount++;
	}
	state = newState;
  }
  // Make sure it plateaued at the correct velocity, not just passed it.
  EXPECT_GT(plateauCount, 5);

  EXPECT_EQ(state, goal);
}

// Test forwards case for a negative, invalid initial velocity.
TEST(TrapezoidProfileTest, CheckLargeVelocityBelowPeakBackwards) {
  wpi::math::TrapezoidProfile<wpi::units::meter>::Constraints constraints{
      1.75_mps, 0.75_mps_sq};
  // Make sure we hit the velocity cap and the profile has input shape -0-.
  wpi::math::TrapezoidProfile<wpi::units::meter>::State goal{-12_m, 0_mps};
  wpi::math::TrapezoidProfile<wpi::units::meter>::State state{0_m, 3_mps};

  wpi::math::TrapezoidProfile<wpi::units::meter> profile{constraints};

  int plateauCount = 0;
  for (int i = 0; i < 1000; i++) {
	auto newState = profile.Calculate(kDt, state, goal);
	CheckFeasible(state, newState, constraints.maxAcceleration);
	if (newState.velocity == -constraints.maxVelocity) {
	  plateauCount++;
	}
	state = newState;
  }
  // Make sure it plateaued at the correct velocity, not just passed it.
  EXPECT_GT(plateauCount, 5);

  EXPECT_EQ(state, goal);
}


TEST(TrapezoidProfileTest, SwitchGoalInMiddle) {
  wpi::math::TrapezoidProfile<wpi::units::meter>::Constraints constraints{
      0.75_mps, 0.75_mps_sq};
  wpi::math::TrapezoidProfile<wpi::units::meter>::State goal{-2_m, 0_mps};
  wpi::math::TrapezoidProfile<wpi::units::meter>::State state;

  wpi::math::TrapezoidProfile<wpi::units::meter> profile{constraints};
  for (int i = 0; i < 200; ++i) {
    state = profile.Calculate(kDt, state, goal);
  }
  EXPECT_NE(state, goal);

  goal = {0.0_m, 0.0_mps};
  profile = wpi::math::TrapezoidProfile<wpi::units::meter>{constraints};
  for (int i = 0; i < 550; ++i) {
    state = profile.Calculate(kDt, state, goal);
  }
  EXPECT_EQ(state, goal);
}

// Checks to make sure that it hits top speed
TEST(TrapezoidProfileTest, TopSpeed) {
  wpi::math::TrapezoidProfile<wpi::units::meter>::Constraints constraints{
      0.75_mps, 0.75_mps_sq};
  wpi::math::TrapezoidProfile<wpi::units::meter>::State goal{4_m, 0_mps};
  wpi::math::TrapezoidProfile<wpi::units::meter>::State state;

  wpi::math::TrapezoidProfile<wpi::units::meter> profile{constraints};
  for (int i = 0; i < 200; ++i) {
    state = profile.Calculate(kDt, state, goal);
  }
  EXPECT_NEAR_UNITS(constraints.maxVelocity, state.velocity, 10e-5_mps);

  profile = wpi::math::TrapezoidProfile<wpi::units::meter>{constraints};
  for (int i = 0; i < 2000; ++i) {
    state = profile.Calculate(kDt, state, goal);
  }
  EXPECT_EQ(state, goal);
}

TEST(TrapezoidProfileTest, TimingToCurrent) {
  wpi::math::TrapezoidProfile<wpi::units::meter>::Constraints constraints{
      0.75_mps, 0.75_mps_sq};
  wpi::math::TrapezoidProfile<wpi::units::meter>::State goal{2_m, 0_mps};
  wpi::math::TrapezoidProfile<wpi::units::meter>::State state;

  wpi::math::TrapezoidProfile<wpi::units::meter> profile{constraints};
  for (int i = 0; i < 400; i++) {
    state = profile.Calculate(kDt, state, goal);
    EXPECT_NEAR_UNITS(profile.TimeLeftUntil(state, state), 0_s, 2e-2_s);
  }
}

TEST(TrapezoidProfileTest, TimingToGoal) {
  using wpi::units::unit_cast;

  wpi::math::TrapezoidProfile<wpi::units::meter>::Constraints constraints{
      0.75_mps, 0.75_mps_sq};
  wpi::math::TrapezoidProfile<wpi::units::meter>::State goal{2_m, 0_mps};

  wpi::math::TrapezoidProfile<wpi::units::meter> profile{constraints};
  auto state = profile.Calculate(
      kDt, goal, wpi::math::TrapezoidProfile<wpi::units::meter>::State{});

  auto predictedTimeLeft = profile.TotalTime();
  bool reachedGoal = false;
  for (int i = 0; i < 400; i++) {
    state = profile.Calculate(kDt, state, goal);
    if (!reachedGoal && state == goal) {
      // Expected value using for loop index is just an approximation since the
      // time left in the profile doesn't increase linearly at the endpoints
      EXPECT_NEAR(unit_cast<double>(predictedTimeLeft), i / 100.0, 0.25);
      reachedGoal = true;
    }
  }
}

TEST(TrapezoidProfileTest, TimingToNegativeGoal) {
  using wpi::units::unit_cast;

  wpi::math::TrapezoidProfile<wpi::units::meter>::Constraints constraints{
      0.75_mps, 0.75_mps_sq};
  wpi::math::TrapezoidProfile<wpi::units::meter>::State goal{-2_m, 0_mps};

  wpi::math::TrapezoidProfile<wpi::units::meter> profile{constraints};
  auto state = profile.Calculate(
      kDt, goal, wpi::math::TrapezoidProfile<wpi::units::meter>::State{});

  auto predictedTimeLeft = profile.TotalTime();
  bool reachedGoal = false;
  for (int i = 0; i < 400; i++) {
    state = profile.Calculate(kDt, state, goal);
    if (!reachedGoal && state == goal) {
      // Expected value using for loop index is just an approximation since the
      // time left in the profile doesn't increase linearly at the endpoints
      EXPECT_NEAR(unit_cast<double>(predictedTimeLeft), i / 100.0, 0.25);
      reachedGoal = true;
    }
  }
}

TEST(TrapezoidProfileTest, GoalVelocityConstraints) {
  wpi::math::TrapezoidProfile<wpi::units::meter>::Constraints constraints{
      0.75_mps, 0.75_mps_sq};
  wpi::math::TrapezoidProfile<wpi::units::meter>::State goal{10_m, 5_mps};
  wpi::math::TrapezoidProfile<wpi::units::meter>::State state{0_m, 0.75_mps};

  wpi::math::TrapezoidProfile<wpi::units::meter> profile{constraints};

  for (int i = 0; i < 200; ++i) {
    state = profile.Calculate(kDt, state, goal);
    EXPECT_LE(wpi::units::math::abs(state.velocity),
              wpi::units::math::abs(constraints.maxVelocity));
  }
}

TEST(TrapezoidProfileTest, NegativeGoalVelocityConstraints) {
  wpi::math::TrapezoidProfile<wpi::units::meter>::Constraints constraints{
      0.75_mps, 0.75_mps_sq};
  wpi::math::TrapezoidProfile<wpi::units::meter>::State goal{10_m, -5_mps};
  wpi::math::TrapezoidProfile<wpi::units::meter>::State state{0_m, 0.75_mps};

  wpi::math::TrapezoidProfile<wpi::units::meter> profile{constraints};

  for (int i = 0; i < 200; ++i) {
    state = profile.Calculate(kDt, state, goal);
    EXPECT_LE(wpi::units::math::abs(state.velocity),
              wpi::units::math::abs(constraints.maxVelocity));
  }
}
