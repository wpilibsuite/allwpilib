// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <cmath>

#include <gtest/gtest.h>

#include "frc/EigenCore.h"
#include "frc/controller/LinearPlantInversionFeedforward.h"
#include "frc/controller/SimpleMotorFeedforward.h"
#include "units/acceleration.h"
#include "units/length.h"
#include "units/time.h"
#include "units/velocity.h"

namespace frc {

TEST(SimpleMotorFeedforwardTest, Calculate) {
  constexpr auto kS = 0.5_V;
  constexpr auto kV = 3_V / 1_mps;
  constexpr auto kA = 0.6_V / 1_mps_sq;
  constexpr units::second_t dt = 20_ms;

  constexpr Matrixd<1, 1> A{{-kV.value() / kA.value()}};
  constexpr Matrixd<1, 1> B{{1.0 / kA.value()}};

  frc::LinearPlantInversionFeedforward<1, 1> plantInversion{A, B, dt};
  frc::SimpleMotorFeedforward<units::meter> simpleMotor{kS, kV, kA};

  constexpr Vectord<1> r{{2.0}};
  constexpr Vectord<1> nextR{{3.0}};

  EXPECT_NEAR((37.524995834325161_V + kS).value(),
              simpleMotor.Calculate(2_mps, 3_mps).value(), 0.002);
  EXPECT_NEAR(plantInversion.Calculate(r, nextR)(0) + kS.value(),
              simpleMotor.Calculate(2_mps, 3_mps).value(), 0.002);

  // These won't match exactly. It's just an approximation to make sure they're
  // in the same ballpark.
  EXPECT_NEAR(plantInversion.Calculate(r, nextR)(0) + kS.value(),
              simpleMotor.Calculate(2_mps, 3_mps).value(), 2.0);
}

TEST(SimpleMotorFeedforwardTest, NegativeGains) {
  constexpr auto kS = 0.5_V;
  constexpr auto kV = -3_V / 1_mps;
  constexpr auto kA = -0.6_V / 1_mps_sq;
  constexpr units::second_t dt = 0_ms;
  frc::SimpleMotorFeedforward<units::meter> simpleMotor{kS, kV, kA, dt};
  EXPECT_EQ(simpleMotor.GetKv().value(), 0);
  EXPECT_EQ(simpleMotor.GetKa().value(), 0);
  EXPECT_EQ(simpleMotor.GetDt().value(), 0.02);
}

}  // namespace frc
