// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/math/trajectory/TrapezoidProfile.hpp"

#include <cmath>

#include <gtest/gtest.h>

#include "wpi/units/acceleration.hpp"
#include "wpi/units/length.hpp"
#include "wpi/units/math.hpp"
#include "wpi/units/velocity.hpp"

static constexpr auto kDt = 10_ms;

// This one might be fine without it, but no need to deal with the bug again.
#define EXPECT_NEAR_UNITS(val1, val2, eps) \
  EXPECT_LE(wpi::units::math::abs((val1) - (val2)), (eps))

// Expressions are improperly handled without parenthases.
#define EXPECT_LT_OR_NEAR_UNITS(val1, val2, eps) \
  if ((val1) <= (val2)) {                        \
    EXPECT_LE((val1), (val2));                   \
  } else {                                       \
    EXPECT_NEAR_UNITS((val1), (val2), (eps));    \
  }

// Ensure the state only changes within the control effort available to us.
void CheckFeasible(
    const wpi::math::TrapezoidProfile<wpi::units::meter>::State& initial,
    const wpi::math::TrapezoidProfile<wpi::units::meter>::State& next,
    const wpi::math::TrapezoidProfile<wpi::units::meter>::Acceleration_t
        maxAccel) {
  auto deltaV = next.velocity - initial.velocity;
  auto deltaX = next.position - initial.position;

  // We can't check for an exact state because the profile may input sign
  // between timestemps.
  EXPECT_LT_OR_NEAR_UNITS(wpi::units::math::abs(deltaV), kDt * maxAccel,
                          2e-8_mps);
  EXPECT_LT_OR_NEAR_UNITS(wpi::units::math::abs(deltaX),
                          wpi::units::math::abs(initial.velocity) * kDt +
                              maxAccel / 2.0 * kDt * kDt,
                          1e-8_m);
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

// Test the forwards case for an invalid initial velocity with the profile sign.
TEST(TrapezoidProfileTest, CheckLargeVelocitySameSignAsPeak) {
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

// Test the backwards case for an invalid initial velocity with the profile
// sign.
TEST(TrapezoidProfileTest, CheckLargeVelocitySameSignAsPeakBackwards) {
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

// Test the forwards case for an invalid initial velocity with a sign
// opposite the profile sign.
TEST(TrapezoidProfileTest, CheckLargeVelocityOppositePeak) {
  wpi::math::TrapezoidProfile<wpi::units::meter>::Constraints constraints{
      1.75_mps, 0.75_mps_sq};
  // Make sure we hit the velocity cap and the profile has input shape -0-.
  wpi::math::TrapezoidProfile<wpi::units::meter>::State goal{12_m, 0_mps};
  wpi::math::TrapezoidProfile<wpi::units::meter>::State state{0_m, -3_mps};

  wpi::math::TrapezoidProfile<wpi::units::meter> profile{constraints};

  int plateauCount = 0;
  // ~17 second trajectory.
  for (int i = 0; i < 1700; i++) {
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

// Test the backwards case for an invalid initial velocity with a sign
// opposite the profile sign.
TEST(TrapezoidProfileTest, CheckLargeVelocityBelowPeakBackwards) {
  wpi::math::TrapezoidProfile<wpi::units::meter>::Constraints constraints{
      1.75_mps, 0.75_mps_sq};
  // Make sure we hit the velocity cap and the profile has input shape -0-.
  wpi::math::TrapezoidProfile<wpi::units::meter>::State goal{-12_m, 0_mps};
  wpi::math::TrapezoidProfile<wpi::units::meter>::State state{0_m, 3_mps};

  wpi::math::TrapezoidProfile<wpi::units::meter> profile{constraints};

  int plateauCount = 0;
  for (int i = 0; i < 1700; i++) {
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

// Test the forwards case for displacement equal to the threshold displacement.
TEST(TrapezoidProfileTest, CheckSignAtThreshold) {
  wpi::math::TrapezoidProfile<wpi::units::meter>::Constraints constraints{
      4_mps, 4_mps_sq};
  wpi::math::TrapezoidProfile<wpi::units::meter>::State goal{0.375_m, 1_mps};
  wpi::math::TrapezoidProfile<wpi::units::meter>::State state{0_m, 2_mps};

  wpi::math::TrapezoidProfile<wpi::units::meter> profile{constraints};

  // Normal profile is 0.25s, and some failure modes are multiples of this.
  for (int i = 0; i < 90; i++) {
    auto newState = profile.Calculate(kDt, state, goal);
    CheckFeasible(state, newState, constraints.maxAcceleration);
    state = newState;
  }

  // The "chattering" failure mode won't reach the goal.
  EXPECT_EQ(state, goal);
}

// Test the backwards case for displacement equal to the threshold displacement.
TEST(TrapezoidProfileTest, CheckSignAtThresholdBackwards) {
  wpi::math::TrapezoidProfile<wpi::units::meter>::Constraints constraints{
      4_mps, 4_mps_sq};
  wpi::math::TrapezoidProfile<wpi::units::meter>::State goal{-0.375_m, -1_mps};
  wpi::math::TrapezoidProfile<wpi::units::meter>::State state{0_m, -2_mps};

  wpi::math::TrapezoidProfile<wpi::units::meter> profile{constraints};

  // Normal profile is 0.25s, and some failure modes are multiples of this.
  for (int i = 0; i < 90; i++) {
    auto newState = profile.Calculate(kDt, state, goal);
    CheckFeasible(state, newState, constraints.maxAcceleration);
    state = newState;
  }

  // The "chattering" failure mode won't reach the goal.
  EXPECT_EQ(state, goal);
}

// This is the case that generated a broken profile in the old impl.
TEST(TrapezoidProfileTest, LargeVelocityAndSmallPositionDelta) {
  wpi::math::TrapezoidProfile<wpi::units::meter>::Constraints constraints{
      1.75_mps, 0.75_mps_sq};
  wpi::math::TrapezoidProfile<wpi::units::meter>::State goal{0.01_m, 0_mps};
  wpi::math::TrapezoidProfile<wpi::units::meter>::State state{0_m, 1_mps};

  wpi::math::TrapezoidProfile<wpi::units::meter> profile{constraints};
  for (int i = 0; i < 450; ++i) {
    auto newState = profile.Calculate(kDt, state, goal);
    CheckFeasible(state, newState, constraints.maxAcceleration);
    state = newState;
  }
  EXPECT_EQ(state, goal);
}

TEST(TrapezoidProfileTest, LargeVelocityAndSmallPositionDeltaBackwards) {
  wpi::math::TrapezoidProfile<wpi::units::meter>::Constraints constraints{
      1.75_mps, 0.75_mps_sq};
  wpi::math::TrapezoidProfile<wpi::units::meter>::State goal{-0.01_m, 0_mps};
  wpi::math::TrapezoidProfile<wpi::units::meter>::State state{0_m, -2_mps};

  wpi::math::TrapezoidProfile<wpi::units::meter> profile{constraints};
  for (int i = 0; i < 700; ++i) {
    auto newState = profile.Calculate(kDt, state, goal);
    CheckFeasible(state, newState, constraints.maxAcceleration);
    state = newState;
  }
  EXPECT_EQ(state, goal);
}

TEST(TrapezoidProfileTest, SwitchGoalInMiddle) {
  wpi::math::TrapezoidProfile<wpi::units::meter>::Constraints constraints{
      0.75_mps, 0.75_mps_sq};
  wpi::math::TrapezoidProfile<wpi::units::meter>::State goal{-2_m, 0_mps};
  wpi::math::TrapezoidProfile<wpi::units::meter>::State state;

  wpi::math::TrapezoidProfile<wpi::units::meter> profile{constraints};
  for (int i = 0; i < 200; ++i) {
    auto newState = profile.Calculate(kDt, state, goal);
    CheckFeasible(state, newState, constraints.maxAcceleration);
    state = newState;
  }
  EXPECT_NE(state, goal);

  goal = {0.0_m, 0.0_mps};
  profile = wpi::math::TrapezoidProfile<wpi::units::meter>{constraints};
  for (int i = 0; i < 550; ++i) {
    auto newState = profile.Calculate(kDt, state, goal);
    CheckFeasible(state, newState, constraints.maxAcceleration);
    state = newState;
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
    auto newState = profile.Calculate(kDt, state, goal);
    CheckFeasible(state, newState, constraints.maxAcceleration);
    state = newState;
  }
  EXPECT_NEAR_UNITS(constraints.maxVelocity, state.velocity, 10e-5_mps);

  profile = wpi::math::TrapezoidProfile<wpi::units::meter>{constraints};
  for (int i = 0; i < 2000; ++i) {
    auto newState = profile.Calculate(kDt, state, goal);
    CheckFeasible(state, newState, constraints.maxAcceleration);
    state = newState;
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
    auto newState = profile.Calculate(kDt, state, goal);
    CheckFeasible(state, newState, constraints.maxAcceleration);
    state = newState;
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
      kDt, wpi::math::TrapezoidProfile<wpi::units::meter>::State{}, goal);

  auto predictedTimeLeft = profile.TotalTime();
  bool reachedGoal = false;
  for (int i = 0; i < 400; i++) {
    auto newState = profile.Calculate(kDt, state, goal);
    CheckFeasible(state, newState, constraints.maxAcceleration);
    state = newState;
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
      kDt, wpi::math::TrapezoidProfile<wpi::units::meter>::State{}, goal);

  auto predictedTimeLeft = profile.TotalTime();
  bool reachedGoal = false;
  for (int i = 0; i < 400; i++) {
    auto newState = profile.Calculate(kDt, state, goal);
    CheckFeasible(state, newState, constraints.maxAcceleration);
    state = newState;
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
    auto newState = profile.Calculate(kDt, state, goal);
    CheckFeasible(state, newState, constraints.maxAcceleration);
    state = newState;
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
    auto newState = profile.Calculate(kDt, state, goal);
    CheckFeasible(state, newState, constraints.maxAcceleration);
    state = newState;
    EXPECT_LE(wpi::units::math::abs(state.velocity),
              wpi::units::math::abs(constraints.maxVelocity));
  }
}
