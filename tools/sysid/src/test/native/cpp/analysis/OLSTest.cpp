// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/sysid/analysis/OLS.hpp"

#ifndef NDEBUG
#ifndef _WIN32
#include <sys/wait.h>
#include <unistd.h>

#include <csignal>
#include <cstdio>
#endif
#endif

#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>

#ifndef NDEBUG
#ifndef _WIN32
namespace {

template <typename F>
bool Dies(F&& f) {
  pid_t pid = fork();
  REQUIRE(pid >= 0);

  if (pid == 0) {
    std::signal(SIGABRT, SIG_DFL);
    std::freopen("/dev/null", "w", stderr);
    f();
    _exit(0);
  }

  int status = 0;
  REQUIRE(waitpid(pid, &status, 0) == pid);
  return WIFSIGNALED(status) || (WIFEXITED(status) && WEXITSTATUS(status) != 0);
}

}  // namespace
#endif
#endif

TEST_CASE("OLSTest TwoVariablesTwoPoints", "[sysid]") {
  // (1, 3) and (2, 5). Should produce y = 2x + 1.
  Eigen::MatrixXd X{{1.0, 1.0}, {1.0, 2.0}};
  Eigen::VectorXd y{{3.0}, {5.0}};

  auto [coeffs, rSquared, rmse] = sysid::OLS(X, y);
  CHECK(coeffs.size() == 2u);

  CHECK(coeffs[0] == Catch::Approx(1.0).margin(1e-12));
  CHECK(coeffs[1] == Catch::Approx(2.0).margin(1e-12));
  CHECK(rSquared == Catch::Approx(1.0).margin(1e-12));
}

TEST_CASE("OLSTest TwoVariablesFivePoints", "[sysid]") {
  // (2, 4), (3, 5), (5, 7), (7, 10), (9, 15)
  // Should produce 1.518x + 0.305.
  Eigen::MatrixXd X{{1, 2}, {1, 3}, {1, 5}, {1, 7}, {1, 9}};
  Eigen::VectorXd y{{4}, {5}, {7}, {10}, {15}};

  auto [coeffs, rSquared, rmse] = sysid::OLS(X, y);
  CHECK(coeffs.size() == 2u);

  CHECK(coeffs[0] == Catch::Approx(0.30487804878048774).margin(1e-12));
  CHECK(coeffs[1] == Catch::Approx(1.5182926829268293).margin(1e-12));
  CHECK(rSquared == Catch::Approx(0.91906029466386019).margin(1e-12));
}

#if !defined(NDEBUG) && !defined(_WIN32)
TEST_CASE("OLSTest MalformedData", "[sysid]") {
  Eigen::MatrixXd X{{1, 2}, {1, 3}, {1, 4}};
  Eigen::VectorXd y{{4}, {5}};
  CHECK(Dies([&] { sysid::OLS(X, y); }));
}
#endif
