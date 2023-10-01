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
static constexpr auto kV = 2.5673_V / 1_mps;
static constexpr auto kA = 2.5673_V / 1_mps_sq;

#define EXPECT_NEAR_UNITS(val1, val2, eps) \
  EXPECT_LE(units::math::abs(val1 - val2) / eps, 1)

#define EXPECT_LT_OR_NEAR_UNITS(val1, val2, eps) \
  if (val1 <= val2) {                            \
    EXPECT_LE(val1, val2);                       \
  } else {                                       \
    EXPECT_NEAR_UNITS(val1, val2, eps);          \
  }

TEST(ExponentialProfileTest, ReachesGoal) {
  frc::ExponentialProfile<units::meter, units::volts>::Constraints constraints{12_V, -kV/kA, 1/kA};
  frc::ExponentialProfile<units::meter, units::volts>::State goal{10_m, 0_mps};
  frc::ExponentialProfile<units::meter, units::volts>::State state;

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

  frc::ExponentialProfile<units::meter, units::volts> profile{constraints, goal};
  auto state = profile.Calculate(kDt);

  auto lastPos = state.position;
  for (int i = 0; i < 1600; ++i) {
    if (i == 400) {
      constraints.maxInput = 9_V;
    }

    profile = frc::ExponentialProfile<units::meter, units::volts>{constraints, goal, state};
    state = profile.Calculate(kDt);
    auto estimatedVel = (state.position - lastPos) / kDt;

    if (i >= 400) {
      // Since estimatedVel can have floating point rounding errors, we check
      // whether value is less than or within an error delta of the new
      // constraint.
      EXPECT_LT_OR_NEAR_UNITS(estimatedVel, constraints.maxInput / kV, 1e-4_mps);

      EXPECT_LE(state.velocity, constraints.maxInput / kV);
    }

    lastPos = state.position;
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
  frc::ExponentialProfile<units::meter, units::volts>::State state;

  for (int i = 0; i < 20; ++i) {
    frc::ExponentialProfile<units::meter, units::volts> profile{constraints, goal, state};
    state = profile.Calculate(kDt);
  }
  EXPECT_NE(state, goal);

  goal = {0.0_m, 0.0_mps};
  for (int i = 0; i < 550; ++i) {
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

  for (int i = 0; i < 2000; ++i) {
    frc::ExponentialProfile<units::meter, units::volts> profile{constraints, goal, state};
    state = profile.Calculate(kDt);
    maxSpeed = units::math::max(state.velocity, maxSpeed);
  }
  EXPECT_NEAR_UNITS(constraints.maxInput / kV, maxSpeed, 1e-5_mps);
  EXPECT_EQ(state, goal);
}

TEST(ExponentialProfileTest, TestHeuristic) {
  frc::ExponentialProfile<units::meter, units::volts>::Constraints constraints{12_V, -kV/kA, 1/kA};
  std::vector<
    std::tuple<
      frc::ExponentialProfile<units::meter, units::volts>::State,
      frc::ExponentialProfile<units::meter, units::volts>::State,
      frc::ExponentialProfile<units::meter, units::volts>::State>> testCases {
    {{0_m, -4_mps}, {0.75_m, -4_mps}, {1.3758_m, 4.4304_mps}},
    {{0_m, -4_mps}, {1.4103_m, 4_mps}, {1.3758_m, 4.4304_mps}},
    {{0.6603_m, 4_mps}, {0.75_m, -4_mps}, {1.3758_m, 4.4304_mps}},
    {{0.6603_m, 4_mps}, {1.4103_m, 4_mps}, {1.3758_m, 4.4304_mps}},

    {{0_m, -4_mps}, {0.5_m, -2_mps}, {0.4367_m, 3.7217_mps}},
    {{0_m, -4_mps}, {0.546_m, 2_mps}, {0.4367_m, 3.7217_mps}},
    {{0.6603_m, 4_mps}, {0.5_m, -2_mps}, {0.5560_m, -2.9616_mps}},
    {{0.6603_m, 4_mps}, {0.546_m, 2_mps}, {0.5560_m, -2.9616_mps}},

    {{0_m, -4_mps}, {-0.75_m, -4_mps}, {-0.7156_m, -4.4304_mps}},
    {{0_m, -4_mps}, {-0.0897_m, 4_mps}, {-0.7156_m, -4.4304_mps}},
    {{0.6603_m, 4_mps}, {-0.75_m, -4_mps}, {-0.7156_m, -4.4304_mps}},
    {{0.6603_m, 4_mps}, {-0.0897_m, 4_mps}, {-0.7156_m, -4.4304_mps}},

    {{0_m, -4_mps}, {-0.5_m, -4.5_mps}, {1.095_m, 4.314_mps}},
    {{0_m, -4_mps}, {1.0795_m, 4.5_mps}, {-0.5122_m, -4.351_mps}},
    {{0.6603_m, 4_mps}, {-0.5_m, -4.5_mps}, {1.095_m, 4.314_mps}},
    {{0.6603_m, 4_mps}, {1.0795_m, 4.5_mps}, {-0.5122_m, -4.351_mps}},
  };

  for (auto &testCase : testCases) {
    frc::ExponentialProfile<units::meter, units::volts> profile{constraints, std::get<1>(testCase), std::get<0>(testCase)};
    auto state = profile.InflectionState();
    EXPECT_NEAR_UNITS(std::get<2>(testCase).position, state.position, 1e-3_m);
    EXPECT_NEAR_UNITS(std::get<2>(testCase).velocity, state.velocity, 1e-3_mps);
  }
}

// TEST(ExponentialProfileTest, TimingToGoal) {
//   using units::unit_cast;

//   frc::ExponentialProfile<units::meter, units::volts>::Constraints constraints{12_V, -kV/kA, 1/kA};
//   frc::ExponentialProfile<units::meter, units::volts>::State goal{10_m, 0_mps};

//   frc::ExponentialProfile<units::meter, units::volts> profile{constraints, goal};
//   auto state = profile.Calculate(kDt);

//   auto predictedTimeLeft = profile.TimeLeftUntil(goal.position);
//   bool reachedGoal = false;
//   for (int i = 0; i < 400; i++) {
//     profile = frc::ExponentialProfile<units::meter, units::volts>(constraints, goal, state);
//     state = profile.Calculate(kDt);
//     if (!reachedGoal && state == goal) {
//       // Expected value using for loop index is just an approximation since the
//       // time left in the profile doesn't increase linearly at the endpoints
//       EXPECT_NEAR(unit_cast<double>(predictedTimeLeft), i / 100.0, 0.25);
//       reachedGoal = true;
//     }
//   }
// }

// TEST(ExponentialProfileTest, TimingBeforeGoal) {
//   using units::unit_cast;

//   frc::ExponentialProfile<units::meter, units::volts>::Constraints constraints{12_V, -kV/kA, 1/kA};
//   frc::ExponentialProfile<units::meter, units::volts>::State goal{10_m, 0_mps};

//   frc::ExponentialProfile<units::meter, units::volts> profile{constraints, goal};
//   auto state = profile.Calculate(kDt);

//   auto predictedTimeLeft = profile.TimeLeftUntil(1_m);
//   bool reachedGoal = false;
//   for (int i = 0; i < 400; i++) {
//     profile = frc::ExponentialProfile<units::meter, units::volts>(constraints, goal, state);
//     state = profile.Calculate(kDt);
//     if (!reachedGoal &&
//         (units::math::abs(state.velocity - 1_mps) < 10e-5_mps)) {
//       EXPECT_NEAR(unit_cast<double>(predictedTimeLeft), i / 100.0, 2e-2);
//       reachedGoal = true;
//     }
//   }
// }

// TEST(ExponentialProfileTest, TimingToNegativeGoal) {
//   using units::unit_cast;

//   frc::ExponentialProfile<units::meter, units::volts>::Constraints constraints{12_V, -kV/kA, 1/kA};
//   frc::ExponentialProfile<units::meter, units::volts>::State goal{10_m, 0_mps};

//   frc::ExponentialProfile<units::meter, units::volts> profile{constraints, goal};
//   auto state = profile.Calculate(kDt);

//   auto predictedTimeLeft = profile.TimeLeftUntil(goal.position);
//   bool reachedGoal = false;
//   for (int i = 0; i < 400; i++) {
//     profile = frc::ExponentialProfile<units::meter, units::volts>(constraints, goal, state);
//     state = profile.Calculate(kDt);
//     if (!reachedGoal && state == goal) {
//       // Expected value using for loop index is just an approximation since the
//       // time left in the profile doesn't increase linearly at the endpoints
//       EXPECT_NEAR(unit_cast<double>(predictedTimeLeft), i / 100.0, 0.25);
//       reachedGoal = true;
//     }
//   }
// }

// TEST(ExponentialProfileTest, TimingBeforeNegativeGoal) {
//   using units::unit_cast;

//   frc::ExponentialProfile<units::meter, units::volts>::Constraints constraints{12_V, -kV/kA, 1/kA};
//   frc::ExponentialProfile<units::meter, units::volts>::State goal{10_m, 0_mps};

//   frc::ExponentialProfile<units::meter, units::volts> profile{constraints, goal};
//   auto state = profile.Calculate(kDt);

//   auto predictedTimeLeft = profile.TimeLeftUntil(-1_m);
//   bool reachedGoal = false;
//   for (int i = 0; i < 400; i++) {
//     profile = frc::ExponentialProfile<units::meter, units::volts>(constraints, goal, state);
//     state = profile.Calculate(kDt);
//     if (!reachedGoal &&
//         (units::math::abs(state.velocity + 1_mps) < 10e-5_mps)) {
//       EXPECT_NEAR(unit_cast<double>(predictedTimeLeft), i / 100.0, 2e-2);
//       reachedGoal = true;
//     }
//   }
// }
