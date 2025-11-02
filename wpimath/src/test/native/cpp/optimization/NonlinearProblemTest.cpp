// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <concepts>
#include <ranges>

#include <gtest/gtest.h>
#include <sleipnir/autodiff/expression_type.hpp>
#include <sleipnir/optimization/problem.hpp>
#include <sleipnir/optimization/solver/exit_status.hpp>

template <typename T>
auto Range(T start, T end, T step) {
  return std::views::iota(0, static_cast<int>((end - start) / step)) |
         std::views::transform([=](auto&& i) { return start + T(i) * step; });
}

TEST(ProblemTest, Quartic) {
  slp::Problem<double> problem;

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

TEST(ProblemTest, RosenbrockWithCubicAndLineConstraint) {
  // https://en.wikipedia.org/wiki/Test_functions_for_optimization#Test_functions_for_constrained_optimization
  for (auto x0 : Range(-1.5, 1.5, 0.1)) {
    for (auto y0 : Range(-0.5, 2.5, 0.1)) {
      slp::Problem<double> problem;

      auto x = problem.decision_variable();
      x.set_value(x0);
      auto y = problem.decision_variable();
      y.set_value(y0);

      problem.minimize(100 * slp::pow(y - slp::pow(x, 2), 2) +
                       slp::pow(1 - x, 2));

      problem.subject_to(y >= slp::pow(x - 1, 3) + 1);
      problem.subject_to(y <= -x + 2);

      EXPECT_EQ(problem.cost_function_type(), slp::ExpressionType::NONLINEAR);
      EXPECT_EQ(problem.equality_constraint_type(), slp::ExpressionType::NONE);
      EXPECT_EQ(problem.inequality_constraint_type(),
                slp::ExpressionType::NONLINEAR);

      EXPECT_EQ(problem.solve({.diagnostics = true}), slp::ExitStatus::SUCCESS);

      auto near = [](double expected, double actual, double tolerance) {
        return std::abs(expected - actual) < tolerance;
      };

      // Local minimum at (0.0, 0.0)
      // Global minimum at (1.0, 1.0)
      EXPECT_TRUE((near(0.0, x.value(), 1e-2) || near(1.0, x.value(), 1e-2)));
      EXPECT_TRUE((near(0.0, y.value(), 1e-2) || near(1.0, y.value(), 1e-2)));
    }
  }
}

TEST(ProblemTest, RosenbrockWithDiskConstraint) {
  // https://en.wikipedia.org/wiki/Test_functions_for_optimization#Test_functions_for_constrained_optimization
  for (auto x0 : Range(-1.5, 1.5, 0.1)) {
    for (auto y0 : Range(-1.5, 1.5, 0.1)) {
      slp::Problem<double> problem;

      auto x = problem.decision_variable();
      x.set_value(x0);
      auto y = problem.decision_variable();
      y.set_value(y0);

      problem.minimize(slp::pow(1 - x, 2) +
                       100 * slp::pow(y - slp::pow(x, 2), 2));

      problem.subject_to(slp::pow(x, 2) + slp::pow(y, 2) <= 2);

      EXPECT_EQ(problem.cost_function_type(), slp::ExpressionType::NONLINEAR);
      EXPECT_EQ(problem.equality_constraint_type(), slp::ExpressionType::NONE);
      EXPECT_EQ(problem.inequality_constraint_type(),
                slp::ExpressionType::QUADRATIC);

      EXPECT_EQ(problem.solve({.diagnostics = true}), slp::ExitStatus::SUCCESS);

      EXPECT_NEAR(x.value(), 1.0, 1e-3);
      EXPECT_NEAR(y.value(), 1.0, 1e-3);
    }
  }
}

TEST(ProblemTest, Minimum2DDistanceWithLinearConstraint) {
  slp::Problem<double> problem;

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
  slp::Problem<double> problem;

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

TEST(ProblemTest, WachterAndBieglerLineSearchFailure) {
  // See example 19.2 of [1]
  //
  // [1] Nocedal, J. and Wright, S. "Numerical Optimization", 2nd. ed., Ch. 19.
  //     Springer, 2006.

  slp::Problem<double> problem;

  auto x = problem.decision_variable();
  auto s1 = problem.decision_variable();
  auto s2 = problem.decision_variable();

  x.set_value(-2);
  s1.set_value(3);
  s2.set_value(1);

  problem.minimize(x);

  problem.subject_to(slp::pow(x, 2) - s1 - 1 == 0);
  problem.subject_to(x - s2 - 0.5 == 0);
  problem.subject_to(s1 >= 0);
  problem.subject_to(s2 >= 0);

  EXPECT_EQ(problem.cost_function_type(), slp::ExpressionType::LINEAR);
  EXPECT_EQ(problem.equality_constraint_type(), slp::ExpressionType::QUADRATIC);
  EXPECT_EQ(problem.inequality_constraint_type(), slp::ExpressionType::LINEAR);

  // FIXME: Fails with "line search failed"
  EXPECT_EQ(problem.solve({.diagnostics = true}),
            slp::ExitStatus::LINE_SEARCH_FAILED);

  // EXPECT_EQ(x.value(), 1.0);
  // EXPECT_EQ(s1.value(), 0.0);
  // EXPECT_EQ(s2.value(), 0.5);
}
