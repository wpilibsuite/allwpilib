// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/trajectory/ExponentialProfile.h"  // NOLINT(build/include_order)

#include <chrono>
#include <cmath>
#include <vector>
#include <tuple>

#include "gtest/gtest.h"
#include "units/acceleration.h"
#include "units/length.h"
#include "units/math.h"
#include "units/velocity.h"
#include "units/voltage.h"
#include "units/frequency.h"

static constexpr auto kDt = 10_ms;
static constexpr auto kV = 2.5629_V / 1_mps;
static constexpr auto kA = 0.43277_V / 1_mps_sq;

#define EXPECT_NEAR_UNITS(val1, val2, eps) \
  EXPECT_LE(units::math::abs(val1 - val2), eps)

#define EXPECT_LT_OR_NEAR_UNITS(val1, val2, eps) \
  if (val1 <= val2) {                            \
    EXPECT_LE(val1, val2);                       \
  } else {                                       \
    EXPECT_NEAR_UNITS(val1, val2, eps);          \
  }

TEST(ExponentialProfileTest, ReachesGoal) {
  frc::ExponentialProfile<units::meter, units::volts>::Constraints constraints{12_V, -kV/kA, 1/kA};
  frc::ExponentialProfile<units::meter, units::volts>::State goal{10_m, 0_mps};
  frc::ExponentialProfile<units::meter, units::volts>::State state{0_m, 0_mps};

  for (int i = 0; i < 450; ++i) {
    frc::ExponentialProfile<units::meter, units::volts> profile{constraints, goal, state};
    state = profile.Calculate(kDt);
  }
  EXPECT_EQ(state, goal);
}

// Tests that decreasing the maximum velocity in the middle when it is already
// moving faster than the new max is handled correctly
TEST(ExponentialProfileTest, PosContinousUnderVelChange) {
  frc::ExponentialProfile<units::meter, units::volts>::Constraints constraints{12_V, -kV/kA, 1/kA};
  frc::ExponentialProfile<units::meter, units::volts>::State goal{10_m, 0_mps};
  frc::ExponentialProfile<units::meter, units::volts>::State state{0_m, 0_mps};

  for (int i = 0; i < 300; ++i) {
    if (i == 150) {
      constraints.maxInput = 9_V;
    }

    frc::ExponentialProfile<units::meter, units::volts> profile{constraints, goal, state};
    state = profile.Calculate(kDt);
  }
  EXPECT_EQ(state, goal);
}

// Tests that decreasing the maximum velocity in the middle when it is already
// moving faster than the new max is handled correctly
TEST(ExponentialProfileTest, PosContinousUnderVelChangeBackward) {
  frc::ExponentialProfile<units::meter, units::volts>::Constraints constraints{12_V, -kV/kA, 1/kA};
  frc::ExponentialProfile<units::meter, units::volts>::State goal{-10_m, 0_mps};
  frc::ExponentialProfile<units::meter, units::volts>::State state{0_m, 0_mps};

  for (int i = 0; i < 300; ++i) {
    if (i == 150) {
      constraints.maxInput = 9_V;
    }

    frc::ExponentialProfile<units::meter, units::volts> profile{constraints, goal, state};
    state = profile.Calculate(kDt);
  }
  EXPECT_EQ(state, goal);
}

// There is some somewhat tricky code for dealing with going backwards
TEST(ExponentialProfileTest, Backwards) {
  frc::ExponentialProfile<units::meter, units::volts>::Constraints constraints{12_V, -kV/kA, 1/kA};
  frc::ExponentialProfile<units::meter, units::volts>::State goal{-10_m, 0_mps};
  frc::ExponentialProfile<units::meter, units::volts>::State state;

  for (int i = 0; i < 400; ++i) {
    frc::ExponentialProfile<units::meter, units::volts> profile{constraints, goal, state};
    state = profile.Calculate(kDt);
  }
  EXPECT_EQ(state, goal);
}

TEST(ExponentialProfileTest, SwitchGoalInMiddle) {
  frc::ExponentialProfile<units::meter, units::volts>::Constraints constraints{12_V, -kV/kA, 1/kA};
  frc::ExponentialProfile<units::meter, units::volts>::State goal{-10_m, 0_mps};
  frc::ExponentialProfile<units::meter, units::volts>::State state{0_m, 0_mps};

  for (int i = 0; i < 50; ++i) {
    frc::ExponentialProfile<units::meter, units::volts> profile{constraints, goal, state};
    state = profile.Calculate(kDt);
  }
  EXPECT_NE(state, goal);

  goal = {0.0_m, 0.0_mps};
  for (int i = 0; i < 100; ++i) {
    frc::ExponentialProfile<units::meter, units::volts> profile{constraints, goal, state};
    state = profile.Calculate(kDt);
  }
  EXPECT_EQ(state, goal);
}

// Checks to make sure that it hits top speed on long trajectories
TEST(ExponentialProfileTest, TopSpeed) {
  frc::ExponentialProfile<units::meter, units::volts>::Constraints constraints{12_V, -kV/kA, 1/kA};
  frc::ExponentialProfile<units::meter, units::volts>::State goal{40_m, 0_mps};
  frc::ExponentialProfile<units::meter, units::volts>::State state;

  units::meters_per_second_t maxSpeed = 0_mps;

  for (int i = 0; i < 900; ++i) {
    frc::ExponentialProfile<units::meter, units::volts> profile{constraints, goal, state};
    state = profile.Calculate(kDt);
    maxSpeed = units::math::max(state.velocity, maxSpeed);
  }

  EXPECT_NEAR_UNITS(constraints.MaxVelocity(), maxSpeed, 1e-5_mps);
  EXPECT_EQ(state, goal);
}

// Checks to make sure that it hits top speed on long trajectories
TEST(ExponentialProfileTest, TopSpeedBackward) {
  frc::ExponentialProfile<units::meter, units::volts>::Constraints constraints{12_V, -kV/kA, 1/kA};
  frc::ExponentialProfile<units::meter, units::volts>::State goal{-40_m, 0_mps};
  frc::ExponentialProfile<units::meter, units::volts>::State state;

  units::meters_per_second_t maxSpeed = 0_mps;

  for (int i = 0; i < 900; ++i) {
    frc::ExponentialProfile<units::meter, units::volts> profile{constraints, goal, state};
    state = profile.Calculate(kDt);
    maxSpeed = units::math::min(state.velocity, maxSpeed);
  }

  EXPECT_NEAR_UNITS(-constraints.MaxVelocity(), maxSpeed, 1e-5_mps);
  EXPECT_EQ(state, goal);
}

// Checks to make sure that it hits top speed on long trajectories
TEST(ExponentialProfileTest, HighInitialSpeed) {
  frc::ExponentialProfile<units::meter, units::volts>::Constraints constraints{12_V, -kV/kA, 1/kA};
  frc::ExponentialProfile<units::meter, units::volts>::State goal{40_m, 0_mps};
  frc::ExponentialProfile<units::meter, units::volts>::State state{0_m, 8_mps};

  for (int i = 0; i < 900; ++i) {
    frc::ExponentialProfile<units::meter, units::volts> profile{constraints, goal, state};
    state = profile.Calculate(kDt);
  }

  EXPECT_EQ(state, goal);
}

// Checks to make sure that it hits top speed on long trajectories
TEST(ExponentialProfileTest, HighInitialSpeedBackward) {
  frc::ExponentialProfile<units::meter, units::volts>::Constraints constraints{12_V, -kV/kA, 1/kA};
  frc::ExponentialProfile<units::meter, units::volts>::State goal{-40_m, 0_mps};
  frc::ExponentialProfile<units::meter, units::volts>::State state{0_m, -8_mps};

  for (int i = 0; i < 900; ++i) {
    frc::ExponentialProfile<units::meter, units::volts> profile{constraints, goal, state};
    state = profile.Calculate(kDt);
  }

  EXPECT_EQ(state, goal);
}

TEST(ExponentialProfileTest, TestHeuristic) {
  frc::ExponentialProfile<units::meter, units::volts>::Constraints constraints{12_V, -kV/kA, 1/kA};
  std::vector<
    std::tuple<
      frc::ExponentialProfile<units::meter, units::volts>::State, // initial
      frc::ExponentialProfile<units::meter, units::volts>::State, // goal
      frc::ExponentialProfile<units::meter, units::volts>::State> // inflection point
    > testCases {
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

  for (auto &testCase : testCases) {
    frc::ExponentialProfile<units::meter, units::volts> profile{constraints, std::get<1>(testCase), std::get<0>(testCase)};
    auto state = profile.InflectionState();
    EXPECT_NEAR_UNITS(std::get<2>(testCase).position / 1_m, state.position / 1_m, 1e-3);
    EXPECT_NEAR_UNITS(std::get<2>(testCase).velocity / 1_mps, state.velocity / 1_mps, 1e-3);
  }
}
