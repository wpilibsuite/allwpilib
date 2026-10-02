// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <cmath>
#include <ranges>

#include <catch2/catch_test_macros.hpp>
#include <sleipnir/autodiff/expression_type.hpp>
#include <sleipnir/autodiff/variable.hpp>
#include <sleipnir/optimization/problem.hpp>
#include <sleipnir/optimization/solver/exit_status.hpp>

#include "wpi/math/TestAssertions.hpp"

template <typename T>
auto Range(T start, T end, T step) {
  return std::views::iota(0, static_cast<int>((end - start) / step)) |
         std::views::transform([=](auto&& i) { return start + T(i) * step; });
}

TEST_CASE("ProblemTest Quartic", "[wpimath]") {
  slp::Problem<double> problem;

  auto x = problem.decision_variable();
  x.set_value(20.0);

  problem.minimize(pow(x, 4.0));

  problem.subject_to(x >= 1.0);

  CHECK(problem.cost_function_type() == slp::ExpressionType::NONLINEAR);
  CHECK(problem.equality_constraint_type() == slp::ExpressionType::NONE);
  CHECK(problem.inequality_constraint_type() == slp::ExpressionType::LINEAR);

  CHECK(problem.solve() == slp::ExitStatus::SUCCESS);

  CHECK_NEAR(x.value(), 1.0, 1e-6);
}

TEST_CASE("ProblemTest RosenbrockWithCubicAndLineConstraint", "[wpimath]") {
  // https://en.wikipedia.org/wiki/Test_functions_for_optimization#Test_functions_for_constrained_optimization
  for (auto x0 : Range(-1.5, 1.5, 0.1)) {
    for (auto y0 : Range(-0.5, 2.5, 0.1)) {
      slp::Problem<double> problem;

      auto x = problem.decision_variable();
      x.set_value(x0);
      auto y = problem.decision_variable();
      y.set_value(y0);

      problem.minimize(100.0 * pow(y - pow(x, 2.0), 2.0) + pow(1.0 - x, 2.0));

      problem.subject_to(y >= pow(x - 1.0, 3.0) + 1.0);
      problem.subject_to(y <= -x + 2.0);

      CHECK(problem.cost_function_type() == slp::ExpressionType::NONLINEAR);
      CHECK(problem.equality_constraint_type() == slp::ExpressionType::NONE);
      CHECK(problem.inequality_constraint_type() ==
            slp::ExpressionType::NONLINEAR);

      CHECK(problem.solve() == slp::ExitStatus::SUCCESS);

      auto near = [](double expected, double actual, double tolerance) {
        return std::abs(expected - actual) < tolerance;
      };

      // Local minimum at (0.0, 0.0)
      // Global minimum at (1.0, 1.0)
      CHECK((near(0.0, x.value(), 1e-2) || near(1.0, x.value(), 1e-2)));
      CHECK((near(0.0, y.value(), 1e-2) || near(1.0, y.value(), 1e-2)));
    }
  }
}

TEST_CASE("ProblemTest RosenbrockWithDiskConstraint", "[wpimath]") {
  // https://en.wikipedia.org/wiki/Test_functions_for_optimization#Test_functions_for_constrained_optimization
  for (auto x0 : Range(-1.5, 1.5, 0.1)) {
    for (auto y0 : Range(-1.5, 1.5, 0.1)) {
      slp::Problem<double> problem;

      auto x = problem.decision_variable();
      x.set_value(x0);
      auto y = problem.decision_variable();
      y.set_value(y0);

      problem.minimize(pow(1.0 - x, 2.0) + 100.0 * pow(y - pow(x, 2.0), 2.0));

      problem.subject_to(pow(x, 2.0) + pow(y, 2.0) <= 2.0);

      CHECK(problem.cost_function_type() == slp::ExpressionType::NONLINEAR);
      CHECK(problem.equality_constraint_type() == slp::ExpressionType::NONE);
      CHECK(problem.inequality_constraint_type() ==
            slp::ExpressionType::QUADRATIC);

      CHECK(problem.solve() == slp::ExitStatus::SUCCESS);

      CHECK_NEAR(x.value(), 1.0, 1e-3);
      CHECK_NEAR(y.value(), 1.0, 1e-3);
    }
  }
}

TEST_CASE("ProblemTest Minimum2DDistanceWithLinearConstraint", "[wpimath]") {
  slp::Problem<double> problem;

  auto x = problem.decision_variable();
  x.set_value(20.0);

  auto y = problem.decision_variable();
  y.set_value(50.0);

  problem.minimize(sqrt(x * x + y * y));

  problem.subject_to(y == -x + 5.0);

  CHECK(problem.cost_function_type() == slp::ExpressionType::NONLINEAR);
  CHECK(problem.equality_constraint_type() == slp::ExpressionType::LINEAR);
  CHECK(problem.inequality_constraint_type() == slp::ExpressionType::NONE);

  CHECK(problem.solve() == slp::ExitStatus::SUCCESS);

  CHECK_NEAR(x.value(), 2.5, 1e-2);
  CHECK_NEAR(y.value(), 2.5, 1e-2);
}

TEST_CASE("ProblemTest ConflictingBounds", "[wpimath]") {
  slp::Problem<double> problem;

  auto x = problem.decision_variable();
  auto y = problem.decision_variable();

  problem.minimize(hypot(x, y));

  problem.subject_to(hypot(x, y) <= 1.0);
  problem.subject_to(x >= 0.5);
  problem.subject_to(x <= -0.5);

  CHECK(problem.cost_function_type() == slp::ExpressionType::NONLINEAR);
  CHECK(problem.equality_constraint_type() == slp::ExpressionType::NONE);
  CHECK(problem.inequality_constraint_type() == slp::ExpressionType::NONLINEAR);

  CHECK(problem.solve() == slp::ExitStatus::GLOBALLY_INFEASIBLE);
}

TEST_CASE("ProblemTest WachterAndBieglerLineSearchFailure", "[wpimath]") {
  // See example 19.2 of [1]
  //
  // [1] Nocedal, J. and Wright, S. "Numerical Optimization", 2nd. ed., Ch. 19.
  //     Springer, 2006.

  slp::Problem<double> problem;

  auto x = problem.decision_variable();
  auto s1 = problem.decision_variable();
  auto s2 = problem.decision_variable();

  x.set_value(-2.0);
  s1.set_value(3.0);
  s2.set_value(1.0);

  problem.minimize(x);

  problem.subject_to(pow(x, 2.0) - s1 - 1.0 == 0.0);
  problem.subject_to(x - s2 - 0.5 == 0.0);
  problem.subject_to(s1 >= 0.0);
  problem.subject_to(s2 >= 0.0);

  CHECK(problem.cost_function_type() == slp::ExpressionType::LINEAR);
  CHECK(problem.equality_constraint_type() == slp::ExpressionType::QUADRATIC);
  CHECK(problem.inequality_constraint_type() == slp::ExpressionType::LINEAR);

  CHECK(problem.solve() == slp::ExitStatus::SUCCESS);

  CHECK_NEAR(x.value(), 1.0, 1e-6);
  CHECK_NEAR(s1.value(), 0.0, 1e-6);
  CHECK_NEAR(s2.value(), 0.5, 1e-6);
}
