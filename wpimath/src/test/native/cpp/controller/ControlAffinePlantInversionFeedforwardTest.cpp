// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <gtest/gtest.h>
#include <wpi/math/EigenCore.h>
#include <wpi/math/controller/ControlAffinePlantInversionFeedforward.h>

#include "units/time.h"

namespace wpi::math {

Vectord<2> Dynamics(const Vectord<2>& x, const Vectord<1>& u) {
  return Matrixd<2, 2>{{1.0, 0.0}, {0.0, 1.0}} * x +
         Matrixd<2, 1>{0.0, 1.0} * u;
}

Vectord<2> StateDynamics(const Vectord<2>& x) {
  return Matrixd<2, 2>{{1.0, 0.0}, {0.0, 1.0}} * x;
}

TEST(ControlAffinePlantInversionFeedforwardTest, Calculate) {
  wpi::math::ControlAffinePlantInversionFeedforward<2, 1> feedforward{&Dynamics,
                                                                      20_ms};

  Vectord<2> r{2, 2};
  Vectord<2> nextR{3, 3};

  EXPECT_NEAR(48, feedforward.Calculate(r, nextR)(0, 0), 1e-6);
}

TEST(ControlAffinePlantInversionFeedforwardTest, CalculateState) {
  wpi::math::ControlAffinePlantInversionFeedforward<2, 1> feedforward{
      &StateDynamics, Matrixd<2, 1>{{0.0}, {1.0}}, 20_ms};

  Vectord<2> r{2, 2};
  Vectord<2> nextR{3, 3};

  EXPECT_NEAR(48, feedforward.Calculate(r, nextR)(0, 0), 1e-6);
}

}  // namespace wpi::math
