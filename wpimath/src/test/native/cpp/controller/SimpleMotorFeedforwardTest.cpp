// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <gtest/gtest.h>

#include <cmath>

#include "frc/EigenCore.h"
#include "frc/controller/LinearPlantInversionFeedforward.h"
#include "frc/controller/SimpleMotorFeedforward.h"
#include "units/acceleration.h"
#include "units/length.h"
#include "units/time.h"

namespace frc {

TEST(SimpleMotorFeedforwardTest, Calculate) {
  double Ks = 0.5;
  double Kv = 3.0;
  double Ka = 0.6;
  auto dt = 0.02_s;

  Matrixd<1, 1> A{-Kv / Ka};
  Matrixd<1, 1> B{1.0 / Ka};

  frc::LinearPlantInversionFeedforward<1, 1> plantInversion{A, B, dt};
  frc::SimpleMotorFeedforward<units::meter> simpleMotor{
      units::volt_t{Ks}, units::volt_t{Kv} / 1_mps,
      units::volt_t{Ka} / 1_mps_sq};

  Vectord<1> r{2.0};
  Vectord<1> nextR{3.0};

  EXPECT_NEAR(37.524995834325161 + Ks,
              simpleMotor.Calculate(2_mps, 3_mps, dt).value(), 0.002);
  EXPECT_NEAR(plantInversion.Calculate(r, nextR)(0) + Ks,
              simpleMotor.Calculate(2_mps, 3_mps, dt).value(), 0.002);

  // These won't match exactly. It's just an approximation to make sure they're
  // in the same ballpark.
  EXPECT_NEAR(plantInversion.Calculate(r, nextR)(0) + Ks,
              simpleMotor.Calculate(2_mps, 1_mps / dt).value(), 2.0);
}

}  // namespace frc
