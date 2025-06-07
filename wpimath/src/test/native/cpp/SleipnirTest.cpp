// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <gtest/gtest.h>
#include <sleipnir/optimization/problem.hpp>

TEST(SleipnirTest, Quartic) {
  slp::Problem problem;

  auto x = problem.decision_variable();
  x.set_value(20.0);

  problem.minimize(slp::pow(x, 4));

  problem.subject_to(x >= 1);

  EXPECT_EQ(problem.cost_function_type(), slp::ExpressionType::NONLINEAR);
  EXPECT_EQ(problem.equality_constraint_type(), slp::ExpressionType::NONE);
  EXPECT_EQ(problem.inequality_constraint_type(), slp::ExpressionType::LINEAR);

  auto status = problem.solve({.diagnostics = true});

  EXPECT_EQ(status, slp::ExitStatus::SUCCESS);

  EXPECT_NEAR(x.value(), 1.0, 1e-6);
}
