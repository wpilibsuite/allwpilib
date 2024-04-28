// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <gtest/gtest.h>
#include <sleipnir/optimization/OptimizationProblem.hpp>

TEST(SleipnirTest, Quartic) {
  sleipnir::OptimizationProblem problem;

  auto x = problem.DecisionVariable();
  x.SetValue(20.0);

  problem.Minimize(sleipnir::pow(x, 4));

  problem.SubjectTo(x >= 1);

  auto status = problem.Solve({.diagnostics = true});

  EXPECT_EQ(status.costFunctionType, sleipnir::ExpressionType::kNonlinear);
  EXPECT_EQ(status.equalityConstraintType, sleipnir::ExpressionType::kNone);
  EXPECT_EQ(status.inequalityConstraintType, sleipnir::ExpressionType::kLinear);
  EXPECT_EQ(status.exitCondition, sleipnir::SolverExitCondition::kSuccess);

  EXPECT_NEAR(x.Value(), 1.0, 1e-6);
}
