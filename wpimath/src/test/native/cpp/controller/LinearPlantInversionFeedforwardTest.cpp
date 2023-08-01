// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <gtest/gtest.h>

#include <cmath>

#include "frc/EigenCore.h"
#include "frc/controller/LinearPlantInversionFeedforward.h"
#include "units/time.h"

namespace frc {

TEST(LinearPlantInversionFeedforwardTest, Calculate) {
  Matrixd<2, 2> A{{1, 0}, {0, 1}};
  Matrixd<2, 1> B{0, 1};

  frc::LinearPlantInversionFeedforward<2, 1> feedforward{A, B, 20_ms};

  Vectord<2> r{2, 2};
  Vectord<2> nextR{3, 3};

  EXPECT_NEAR(47.502599, feedforward.Calculate(r, nextR)(0, 0), 0.002);
}

}  // namespace frc
