// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/math/optimization/CurrentManager.hpp"

#include <array>

#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>

#include "wpi/math/TestAssertions.hpp"

TEST_CASE("CurrentManagerTest EnoughCurrent", "[wpimath]") {
  wpi::math::CurrentManager manager{std::array{1.0, 5.0, 10.0, 5.0}, 40.0};

  auto currents = manager.calculate(std::array{25.0, 10.0, 5.0, 0.0});

  CHECK_NEAR(currents[0], 25.0, 1e-3);
  CHECK_NEAR(currents[1], 10.0, 1e-3);
  CHECK_NEAR(currents[2], 5.0, 1e-3);
  CHECK_NEAR(currents[3], 0.0, 1e-3);
}

TEST_CASE("CurrentManagerTest NotEnoughCurrent", "[wpimath]") {
  wpi::math::CurrentManager manager{std::array{1.0, 5.0, 10.0, 5.0}, 40.0};

  auto currents = manager.calculate(std::array{30.0, 10.0, 5.0, 0.0});

  // Expected values are from the following program:
  //
  // #!/usr/bin/env python
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
  CHECK_NEAR(currents[0], 29.960, 1e-3);
  CHECK_NEAR(currents[1], 9.008, 1e-3);
  CHECK_NEAR(currents[2], 1.032, 1e-3);
  CHECK_NEAR(currents[3], 0.0, 1e-3);
}
