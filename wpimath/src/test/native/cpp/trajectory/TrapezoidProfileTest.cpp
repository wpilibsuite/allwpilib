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

TEST(TrapezoidProfileTest, ReachesGoal) {
  wpi::math::TrapezoidProfile<wpi::units::meter>::Constraints constraints{
      1.75_mps, 0.75_mps_sq};
  wpi::math::TrapezoidProfile<wpi::units::meter>::State goal{3_m, 0_mps};
  wpi::math::TrapezoidProfile<wpi::units::meter>::State state;

  wpi::math::TrapezoidProfile<wpi::units::meter> profile{constraints};
  for (int i = 0; i < 450; ++i) {
    state = profile.Calculate(kDt, state, goal);
  }
  EXPECT_EQ(state, goal);
}

// Tests that decreasing the maximum velocity in the middle when it is already
// moving faster than the new max is handled correctly
TEST(TrapezoidProfileTest, PosContinuousUnderVelChange) {
  wpi::math::TrapezoidProfile<wpi::units::meter>::Constraints constraints{
      1.75_mps, 0.75_mps_sq};
  wpi::math::TrapezoidProfile<wpi::units::meter>::State goal{12_m, 0_mps};

  wpi::math::TrapezoidProfile<wpi::units::meter> profile{constraints};
  auto state = profile.Calculate(
      kDt, wpi::math::TrapezoidProfile<wpi::units::meter>::State{}, goal);

  auto lastPos = state.position;
  for (int i = 0; i < 1600; ++i) {
    if (i == 400) {
      constraints.maxVelocity = 0.75_mps;
      profile = wpi::math::TrapezoidProfile<wpi::units::meter>{constraints};
    }

    state = profile.Calculate(kDt, state, goal);
    auto estimatedVel = (state.position - lastPos) / kDt;

    if (i >= 400) {
      // Since estimatedVel can have floating point rounding errors, we check
      // whether value is less than or within an error delta of the new
      // constraint.
      EXPECT_LT_OR_NEAR_UNITS(estimatedVel, constraints.maxVelocity, 1e-4_mps);

      EXPECT_LE(state.velocity, constraints.maxVelocity);
    }

    lastPos = state.position;
  }
  EXPECT_EQ(state, goal);
}

// There is some somewhat tricky code for dealing with going backwards
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

// Checks to make sure that it hits top velocity
TEST(TrapezoidProfileTest, TopVelocity) {
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
    EXPECT_NEAR_UNITS(profile.TimeLeftUntil(state.position), 0_s, 2e-2_s);
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

  auto predictedTimeLeft = profile.TimeLeftUntil(goal.position);
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

TEST(TrapezoidProfileTest, TimingBeforeGoal) {
  using wpi::units::unit_cast;

  wpi::math::TrapezoidProfile<wpi::units::meter>::Constraints constraints{
      0.75_mps, 0.75_mps_sq};
  wpi::math::TrapezoidProfile<wpi::units::meter>::State goal{2_m, 0_mps};

  wpi::math::TrapezoidProfile<wpi::units::meter> profile{constraints};
  auto state = profile.Calculate(
      kDt, goal, wpi::math::TrapezoidProfile<wpi::units::meter>::State{});

  auto predictedTimeLeft = profile.TimeLeftUntil(1_m);
  bool reachedGoal = false;
  for (int i = 0; i < 400; i++) {
    state = profile.Calculate(kDt, state, goal);
    if (!reachedGoal &&
        (wpi::units::math::abs(state.velocity - 1_mps) < 10e-5_mps)) {
      EXPECT_NEAR(unit_cast<double>(predictedTimeLeft), i / 100.0, 2e-2);
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

  auto predictedTimeLeft = profile.TimeLeftUntil(goal.position);
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

TEST(TrapezoidProfileTest, TimingBeforeNegativeGoal) {
  using wpi::units::unit_cast;

  wpi::math::TrapezoidProfile<wpi::units::meter>::Constraints constraints{
      0.75_mps, 0.75_mps_sq};
  wpi::math::TrapezoidProfile<wpi::units::meter>::State goal{-2_m, 0_mps};

  wpi::math::TrapezoidProfile<wpi::units::meter> profile{constraints};
  auto state = profile.Calculate(
      kDt, goal, wpi::math::TrapezoidProfile<wpi::units::meter>::State{});

  auto predictedTimeLeft = profile.TimeLeftUntil(-1_m);
  bool reachedGoal = false;
  for (int i = 0; i < 400; i++) {
    state = profile.Calculate(kDt, state, goal);
    if (!reachedGoal &&
        (wpi::units::math::abs(state.velocity + 1_mps) < 10e-5_mps)) {
      EXPECT_NEAR(unit_cast<double>(predictedTimeLeft), i / 100.0, 2e-2);
      reachedGoal = true;
    }
  }
}

TEST(TrapezoidProfileTest, InitalizationOfCurrentState) {
  wpi::math::TrapezoidProfile<wpi::units::meter>::Constraints constraints{
      1_mps, 1_mps_sq};
  wpi::math::TrapezoidProfile<wpi::units::meter> profile{constraints};
  EXPECT_NEAR_UNITS(profile.TimeLeftUntil(0_m), 0_s, 1e-10_s);
  EXPECT_NEAR_UNITS(profile.TotalTime(), 0_s, 1e-10_s);
}

TEST(TrapezoidProfileTest, InitialVelocityConstraints) {
  wpi::math::TrapezoidProfile<wpi::units::meter>::Constraints constraints{
      0.75_mps, 0.75_mps_sq};
  wpi::math::TrapezoidProfile<wpi::units::meter>::State goal{10_m, 0_mps};
  wpi::math::TrapezoidProfile<wpi::units::meter>::State state{0_m, -10_mps};

  wpi::math::TrapezoidProfile<wpi::units::meter> profile{constraints};

  for (int i = 0; i < 200; ++i) {
    state = profile.Calculate(kDt, state, goal);
    EXPECT_LE(wpi::units::math::abs(state.velocity),
              wpi::units::math::abs(constraints.maxVelocity));
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
