// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <gtest/gtest.h>

#include <cmath>

#include "Eigen/Core"
#include "frc/controller/LinearPlantInversionFeedforward.h"
#include "units/time.h"

namespace frc {

TEST(LinearPlantInversionFeedforwardTest, Calculate) {
  Eigen::Matrix<double, 2, 2> A;
  A << 1, 0, 0, 1;

  Eigen::Matrix<double, 2, 1> B;
  B << 0, 1;

  frc::LinearPlantInversionFeedforward<2, 1> feedforward{A, B,
                                                         units::second_t(0.02)};

  Eigen::Matrix<double, 2, 1> r;
  r << 2, 2;
  Eigen::Matrix<double, 2, 1> nextR;
  nextR << 3, 3;

  EXPECT_NEAR(47.502599, feedforward.Calculate(r, nextR)(0, 0), 0.002);
}

}  // namespace frc
