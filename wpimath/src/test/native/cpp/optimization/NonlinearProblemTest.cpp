// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <gtest/gtest.h>
#include <sleipnir/autodiff/expression_type.hpp>
#include <sleipnir/optimization/problem.hpp>
#include <sleipnir/optimization/solver/exit_status.hpp>

TEST(ProblemTest, Quartic) {
  slp::Problem problem;

  auto x = problem.decision_variable();
  x.set_value(20.0);

  problem.minimize(slp::pow(x, 4));

  problem.subject_to(x >= 1);

  EXPECT_EQ(problem.cost_function_type(), slp::ExpressionType::NONLINEAR);
  EXPECT_EQ(problem.equality_constraint_type(), slp::ExpressionType::NONE);
  EXPECT_EQ(problem.inequality_constraint_type(), slp::ExpressionType::LINEAR);

  EXPECT_EQ(problem.solve({.diagnostics = true}), slp::ExitStatus::SUCCESS);

  EXPECT_NEAR(x.value(), 1.0, 1e-6);
}

TEST(ProblemTest, Minimum2DDistanceWithLinearConstraint) {
  slp::Problem problem;

  auto x = problem.decision_variable();
  x.set_value(20.0);

  auto y = problem.decision_variable();
  y.set_value(50.0);

  problem.minimize(slp::sqrt(x * x + y * y));

  problem.subject_to(y == -x + 5.0);

  EXPECT_EQ(problem.cost_function_type(), slp::ExpressionType::NONLINEAR);
  EXPECT_EQ(problem.equality_constraint_type(), slp::ExpressionType::LINEAR);
  EXPECT_EQ(problem.inequality_constraint_type(), slp::ExpressionType::NONE);

#if defined(__linux__) && defined(__aarch64__)
  // FIXME: Fails on Linux aarch64 with "line search failed"
  EXPECT_EQ(problem.solve({.diagnostics = true}),
            slp::ExitStatus::LINE_SEARCH_FAILED);
  return;
#else
  EXPECT_EQ(problem.solve({.diagnostics = true}), slp::ExitStatus::SUCCESS);
#endif

  EXPECT_NEAR(x.value(), 2.5, 1e-2);
  EXPECT_NEAR(y.value(), 2.5, 1e-2);
}

TEST(ProblemTest, ConflictingBounds) {
  slp::Problem problem;

  auto x = problem.decision_variable();
  auto y = problem.decision_variable();

  problem.minimize(slp::hypot(x, y));

  problem.subject_to(slp::hypot(x, y) <= 1);
  problem.subject_to(x >= 0.5);
  problem.subject_to(x <= -0.5);

  EXPECT_EQ(problem.cost_function_type(), slp::ExpressionType::NONLINEAR);
  EXPECT_EQ(problem.equality_constraint_type(), slp::ExpressionType::NONE);
  EXPECT_EQ(problem.inequality_constraint_type(),
            slp::ExpressionType::NONLINEAR);

  EXPECT_EQ(problem.solve({.diagnostics = true}),
            slp::ExitStatus::GLOBALLY_INFEASIBLE);
}
