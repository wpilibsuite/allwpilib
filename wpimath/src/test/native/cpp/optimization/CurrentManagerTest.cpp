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

  // Expected values are from the following CasADi program:
  //
  // #!/usr/bin/env python3
  //
  // import casadi as ca
  // import numpy as np
  //
  // opti = ca.Opti()
  // allocated_currents = opti.variable(4, 1)
  //
  // current_tolerances = np.array([[1.0], [5.0], [10.0], [5.0]])
  // desired_currents = np.array([[30.0], [10.0], [5.0], [0.0]])
  //
  // J = 0.0
  // current_sum = 0.0
  // for i in range(4):
  //     error = desired_currents[i, 0] - allocated_currents[i, 0]
  //     J += error**2 / current_tolerances[i] ** 2
  //
  //     current_sum += allocated_currents[i, 0]
  //
  //     # Currents must be nonnegative
  //     opti.subject_to(allocated_currents[i, 0] >= 0.0)
  // opti.minimize(J)
  //
  // # Keep total current below maximum
  // opti.subject_to(current_sum <= 40.0)
  //
  // opti.solver("ipopt")
  // print(opti.solve().value(allocated_currents))
  EXPECT_NEAR(currents[0], 29.960, 1e-3);
  EXPECT_NEAR(currents[1], 9.007, 1e-3);
  EXPECT_NEAR(currents[2], 1.032, 1e-3);
  EXPECT_NEAR(currents[3], 0.0, 1e-3);
}
