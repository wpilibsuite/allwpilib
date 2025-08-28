// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <algorithm>
#include <cmath>
#include <random>

#include <gtest/gtest.h>

#include "wpimath/optimization/SimulatedAnnealing.h"

TEST(SimulatedAnnealingTest, DoubleFunctionOptimizationHeartBeat) {
  auto function = [](double x) {
    return -(x + std::sin(x)) * std::exp(-x * x) + 1;
  };

  constexpr double stepSize = 10.0;

  std::random_device rd;
  std::mt19937 gen{rd()};
  std::uniform_real_distribution<> distr{0.0, 1.0};

  wpimath::SimulatedAnnealing<double> simulatedAnnealing{
      2.0,
      [&](const double& x) {
        return std::clamp(x + (distr(gen) - 0.5) * stepSize, -3.0, 3.0);
      },
      [&](const double& x) { return function(x); }};

  double solution = simulatedAnnealing.Solve(-1.0, 5000);

  EXPECT_NEAR(0.68, solution, 1e-1);
}

TEST(SimulatedAnnealingTest, DoubleFunctionOptimizationMultimodal) {
  auto function = [](double x) {
    return std::sin(x) + std::sin((10.0 / 3.0) * x);
  };

  constexpr double stepSize = 10.0;

  std::random_device rd;
  std::mt19937 gen{rd()};
  std::uniform_real_distribution<> distr{0.0, 1.0};

  wpimath::SimulatedAnnealing<double> simulatedAnnealing{
      2.0,
      [&](const double& x) {
        return std::clamp(x + (distr(gen) - 0.5) * stepSize, 0.0, 7.0);
      },
      [&](const double& x) { return function(x); }};

  double solution = simulatedAnnealing.Solve(-1.0, 5000);

  EXPECT_NEAR(5.146, solution, 1e-1);
}
