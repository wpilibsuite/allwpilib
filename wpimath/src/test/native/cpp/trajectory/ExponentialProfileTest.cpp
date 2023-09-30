// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/trajectory/ExponentialProfile.h"  // NOLINT(build/include_order)

#include <chrono>
#include <cmath>

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

  for (int i = 0; i < 200; ++i) {
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

// Checks to make sure that it hits top speed
TEST(ExponentialProfileTest, TopSpeed) {
  frc::ExponentialProfile<units::meter, units::volts>::Constraints constraints{12_V, -kV/kA, 1/kA};
  frc::ExponentialProfile<units::meter, units::volts>::State goal{40_m, 0_mps};
  frc::ExponentialProfile<units::meter, units::volts>::State state;

  for (int i = 0; i < 200; ++i) {
    frc::ExponentialProfile<units::meter, units::volts> profile{constraints, goal, state};
    state = profile.Calculate(kDt);
  }
  EXPECT_NEAR_UNITS(constraints.maxInput / kV, state.velocity, 10e-5_mps);

  for (int i = 0; i < 2000; ++i) {
    frc::ExponentialProfile<units::meter, units::volts> profile{constraints, goal, state};
    state = profile.Calculate(kDt);
  }
  EXPECT_EQ(state, goal);
}

// TEST(ExponentialProfileTest, TimingToCurrent) {
//   frc::ExponentialProfile<units::meter, units::volts>::Constraints constraints{12_V, -kV/kA, 1/kA};
//   frc::ExponentialProfile<units::meter, units::volts>::State goal{10_m, 0_mps};
//   frc::ExponentialProfile<units::meter, units::volts>::State state;

//   for (int i = 0; i < 400; i++) {
//     frc::ExponentialProfile<units::meter, units::volts> profile{constraints, goal, state};
//     state = profile.Calculate(kDt);
//     EXPECT_NEAR_UNITS(profile.TimeLeftUntil(state.position), 0_s, 2e-2_s);
//   }
// }

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
