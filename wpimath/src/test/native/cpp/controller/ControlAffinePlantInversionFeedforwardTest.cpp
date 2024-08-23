// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <cmath>

#include <gtest/gtest.h>

#include "frc/EigenCore.h"
#include "frc/controller/ControlAffinePlantInversionFeedforward.h"
#include "units/time.h"

namespace frc {

Vectord<2> Dynamics(const Vectord<2>& x, const Vectord<1>& u) {
  return Matrixd<2, 2>{{1.0, 0.0}, {0.0, 1.0}} * x +
         Matrixd<2, 1>{0.0, 1.0} * u;
}

Vectord<2> StateDynamics(const Vectord<2>& x) {
  return Matrixd<2, 2>{{1.0, 0.0}, {0.0, 1.0}} * x;
}

TEST(ControlAffinePlantInversionFeedforwardTest, Calculate) {
  std::function<Vectord<2>(const Vectord<2>&, const Vectord<1>&)>
      modelDynamics = [](auto& x, auto& u) { return Dynamics(x, u); };

  frc::ControlAffinePlantInversionFeedforward<2, 1> feedforward{modelDynamics,
                                                                20_ms};

  Vectord<2> r{2, 2};
  Vectord<2> nextR{3, 3};

  EXPECT_NEAR(48, feedforward.Calculate(r, nextR)(0, 0), 1e-6);
}

TEST(ControlAffinePlantInversionFeedforwardTest, CalculateState) {
  std::function<Vectord<2>(const Vectord<2>&)> modelDynamics = [](auto& x) {
    return StateDynamics(x);
  };

  Matrixd<2, 1> B{0, 1};

  frc::ControlAffinePlantInversionFeedforward<2, 1> feedforward{modelDynamics,
                                                                B, 20_ms};

  Vectord<2> r{2, 2};
  Vectord<2> nextR{3, 3};

  EXPECT_NEAR(48, feedforward.Calculate(r, nextR)(0, 0), 1e-6);
}

}  // namespace frc
