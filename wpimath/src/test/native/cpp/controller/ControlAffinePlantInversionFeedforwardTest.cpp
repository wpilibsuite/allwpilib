// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <gtest/gtest.h>

#include <cmath>

#include "Eigen/Core"
#include "frc/controller/ControlAffinePlantInversionFeedforward.h"
#include "units/time.h"

namespace frc {

Eigen::Vector<double, 2> Dynamics(const Eigen::Vector<double, 2>& x,
                                  const Eigen::Vector<double, 1>& u) {
  return Eigen::Matrix<double, 2, 2>{{1.0, 0.0}, {0.0, 1.0}} * x +
         Eigen::Matrix<double, 2, 1>{0.0, 1.0} * u;
}

Eigen::Vector<double, 2> StateDynamics(const Eigen::Vector<double, 2>& x) {
  return Eigen::Matrix<double, 2, 2>{{1.0, 0.0}, {0.0, 1.0}} * x;
}

TEST(ControlAffinePlantInversionFeedforwardTest, Calculate) {
  std::function<Eigen::Vector<double, 2>(const Eigen::Vector<double, 2>&,
                                         const Eigen::Vector<double, 1>&)>
      modelDynamics = [](auto& x, auto& u) { return Dynamics(x, u); };

  frc::ControlAffinePlantInversionFeedforward<2, 1> feedforward{
      modelDynamics, units::second_t{0.02}};

  Eigen::Vector<double, 2> r{2, 2};
  Eigen::Vector<double, 2> nextR{3, 3};

  EXPECT_NEAR(48, feedforward.Calculate(r, nextR)(0, 0), 1e-6);
}

TEST(ControlAffinePlantInversionFeedforwardTest, CalculateState) {
  std::function<Eigen::Vector<double, 2>(const Eigen::Vector<double, 2>&)>
      modelDynamics = [](auto& x) { return StateDynamics(x); };

  Eigen::Matrix<double, 2, 1> B{0, 1};

  frc::ControlAffinePlantInversionFeedforward<2, 1> feedforward{
      modelDynamics, B, units::second_t(0.02)};

  Eigen::Vector<double, 2> r{2, 2};
  Eigen::Vector<double, 2> nextR{3, 3};

  EXPECT_NEAR(48, feedforward.Calculate(r, nextR)(0, 0), 1e-6);
}

}  // namespace frc
