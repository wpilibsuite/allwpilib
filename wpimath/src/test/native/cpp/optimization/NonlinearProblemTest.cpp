// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <array>
#include <cstdio>
#include <ranges>
#include <string>

#include <catch2/catch_approx.hpp>
#include <catch2/catch_message.hpp>
#include <catch2/catch_test_macros.hpp>
#include <sleipnir/autodiff/expression_type.hpp>
#include <sleipnir/optimization/problem.hpp>
#include <sleipnir/optimization/solver/exit_status.hpp>

#ifdef _WIN32
#include <io.h>
#else
#include <unistd.h>
#endif

template <typename T>
auto Range(T start, T end, T step) {
  return std::views::iota(0, static_cast<int>((end - start) / step)) |
         std::views::transform([=](auto&& i) { return start + T(i) * step; });
}

namespace {

class StdoutCapture {
 public:
  StdoutCapture() {
    std::fflush(stdout);
    m_file = std::tmpfile();
    REQUIRE(m_file != nullptr);
    m_originalStdout = Dup(Fileno(stdout));
    REQUIRE(m_originalStdout >= 0);
    REQUIRE(Dup2(Fileno(m_file), Fileno(stdout)) >= 0);
  }

  StdoutCapture(const StdoutCapture&) = delete;
  StdoutCapture& operator=(const StdoutCapture&) = delete;

  ~StdoutCapture() {
    if (m_originalStdout >= 0) {
      std::fflush(stdout);
      Dup2(m_originalStdout, Fileno(stdout));
      Close(m_originalStdout);
    }
    if (m_file) {
      std::fclose(m_file);
    }
  }

  std::string Stop() {
    std::fflush(stdout);
    REQUIRE(Dup2(m_originalStdout, Fileno(stdout)) >= 0);
    Close(m_originalStdout);
    m_originalStdout = -1;

    std::rewind(m_file);
    std::string output;
    std::array<char, 1024> buffer;
    while (auto count = std::fread(buffer.data(), 1, buffer.size(), m_file)) {
      output.append(buffer.data(), count);
    }
    return output;
  }

 private:
  static int Fileno(FILE* file) {
#ifdef _WIN32
    return _fileno(file);
#else
    return fileno(file);
#endif
  }

  static int Dup(int fd) {
#ifdef _WIN32
    return _dup(fd);
#else
    return dup(fd);
#endif
  }

  static int Dup2(int source, int dest) {
#ifdef _WIN32
    return _dup2(source, dest);
#else
    return dup2(source, dest);
#endif
  }

  static void Close(int fd) {
#ifdef _WIN32
    _close(fd);
#else
    close(fd);
#endif
  }

  FILE* m_file = nullptr;
  int m_originalStdout = -1;
};

slp::ExitStatus SolveWithDiagnostics(slp::Problem<double>& problem) {
  StdoutCapture capture;
  auto status = problem.solve({.diagnostics = true});
  auto output = capture.Stop();
  UNSCOPED_INFO(output);
  return status;
}

}  // namespace

TEST_CASE("ProblemTest Quartic", "[wpimath]") {
  slp::Problem<double> problem;

  auto x = problem.decision_variable();
  x.set_value(20.0);

  problem.minimize(pow(x, 4.0));

  problem.subject_to(x >= 1.0);

  CHECK(problem.cost_function_type() == slp::ExpressionType::NONLINEAR);
  CHECK(problem.equality_constraint_type() == slp::ExpressionType::NONE);
  CHECK(problem.inequality_constraint_type() == slp::ExpressionType::LINEAR);

  CHECK(SolveWithDiagnostics(problem) == slp::ExitStatus::SUCCESS);

  CHECK(x.value() == Catch::Approx(1.0).margin(1e-6));
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

      CHECK(SolveWithDiagnostics(problem) == slp::ExitStatus::SUCCESS);

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

      CHECK(SolveWithDiagnostics(problem) == slp::ExitStatus::SUCCESS);

      CHECK(x.value() == Catch::Approx(1.0).margin(1e-3));
      CHECK(y.value() == Catch::Approx(1.0).margin(1e-3));
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

  CHECK(SolveWithDiagnostics(problem) == slp::ExitStatus::SUCCESS);

  CHECK(x.value() == Catch::Approx(2.5).margin(1e-2));
  CHECK(y.value() == Catch::Approx(2.5).margin(1e-2));
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

  CHECK(SolveWithDiagnostics(problem) == slp::ExitStatus::GLOBALLY_INFEASIBLE);
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

  CHECK(SolveWithDiagnostics(problem) == slp::ExitStatus::SUCCESS);

  CHECK(x.value() == Catch::Approx(1.0).margin(1e-6));
  CHECK(s1.value() == Catch::Approx(0.0).margin(1e-6));
  CHECK(s2.value() == Catch::Approx(0.5).margin(1e-6));
}
