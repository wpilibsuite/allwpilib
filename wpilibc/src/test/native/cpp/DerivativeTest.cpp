/*----------------------------------------------------------------------------*/
/* Copyright (c) 2020 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include <units/units.h>

#include "frc/Derivative.h"
#include "gtest/gtest.h"

using namespace frc;

TEST(DerivativeTest, DerivativeTest) {
  Derivative<units::meter> twoSampleDerivative{2, 0.02_s};
  Derivative<units::meter> fiveSampleDerivative{5, 0.02_s};

  // Input stream; increases by 1 every 20ms, derivative should be 50.
  units::meter_t input[] = {0_m, 1_m, 2_m, 3_m, 4_m, 5_m, 6_m, 7_m, 8_m, 9_m};

  units::meters_per_second_t twoSampleOutput = 0_mps;
  units::meters_per_second_t fiveSampleOutput = 0_mps;

  for (auto value : input) {
    twoSampleOutput = twoSampleDerivative.Calculate(value);
    fiveSampleOutput = fiveSampleDerivative.Calculate(value);
  }

  EXPECT_EQ(twoSampleOutput, 50_mps);
  EXPECT_EQ(fiveSampleOutput, 50_mps);
}
