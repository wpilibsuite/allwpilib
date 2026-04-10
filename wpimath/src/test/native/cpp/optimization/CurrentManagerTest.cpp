// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/math/optimization/CurrentManager.hpp"

#include <array>

#include <gtest/gtest.h>

TEST(CurrentManagerTest, EnoughCurrent) {
  wpi::math::CurrentManager manager{std::array{1.0, 5.0, 10.0, 5.0}, 40.0};

  auto currents = manager.calculate(std::array{25.0, 10.0, 5.0, 0.0});

  EXPECT_NEAR(currents[0], 25.0, 1e-3);
  EXPECT_NEAR(currents[1], 10.0, 1e-3);
  EXPECT_NEAR(currents[2], 5.0, 1e-3);
  EXPECT_NEAR(currents[3], 0.0, 1e-3);
}

TEST(CurrentManagerTest, NotEnoughCurrent) {
  wpi::math::CurrentManager manager{std::array{1.0, 5.0, 10.0, 5.0}, 40.0};

  auto currents = manager.calculate(std::array{30.0, 10.0, 5.0, 0.0});

  // Expected values are from the following program:
  //
  // #!/usr/bin/env python3
  //
  // from scipy.optimize import minimize
  //
  // r = [30.0, 10.0, 5.0, 0.0]
  // q = [1.0, 5.0, 10.0, 5.0]
  //
  // result = minimize(
  //     lambda x: sum((r[i] - x[i]) ** 2 / q[i] ** 2 for i in range(4)),
  //     [0.0, 0.0, 0.0, 0.0],
  //     constraints=[
  //         {"type": "ineq", "fun": lambda x: x},
  //         {"type": "ineq", "fun": lambda x: 40.0 - sum(x)},
  //     ],
  // )
  // print(result.x)
  EXPECT_NEAR(currents[0], 29.960, 1e-3);
  EXPECT_NEAR(currents[1], 9.008, 1e-3);
  EXPECT_NEAR(currents[2], 1.032, 1e-3);
  EXPECT_NEAR(currents[3], 0.0, 1e-3);
}
