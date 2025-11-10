// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <benchmark/benchmark.h>

#include <chrono>

#include <sleipnir/optimization/problem.hpp>

#include "wpi/math/system/NumericalIntegration.hpp"

inline slp::VariableMatrix CartPoleDynamics(const slp::VariableMatrix& x,
                                            const slp::VariableMatrix& u) {
  constexpr double m_c = 5.0;  // Cart mass (kg)
  constexpr double m_p = 0.5;  // Pole mass (kg)
  constexpr double l = 0.5;    // Pole length (m)
  constexpr double g = 9.806;  // Acceleration due to gravity (m/s²)

  auto q = x.segment(0, 2);
  auto qdot = x.segment(2, 2);
  auto theta = q[1];
  auto thetadot = qdot[1];

  //        [ m_c + m_p  m_p l cosθ]
  // M(q) = [m_p l cosθ    m_p l²  ]
  slp::VariableMatrix M{{m_c + m_p, m_p * l * cos(theta)},
                        {m_p * l * cos(theta), m_p * std::pow(l, 2)}};

  //           [0  −m_p lθ̇ sinθ]
  // C(q, q̇) = [0       0      ]
  slp::VariableMatrix C{{0, -m_p * l * thetadot * sin(theta)}, {0, 0}};

  //          [     0      ]
  // τ_g(q) = [-m_p gl sinθ]
  slp::VariableMatrix tau_g{{0}, {-m_p * g * l * sin(theta)}};

  //     [1]
  // B = [0]
  constexpr Eigen::Matrix<double, 2, 1> B{{1}, {0}};

  // q̈ = M⁻¹(q)(τ_g(q) − C(q, q̇)q̇ + Bu)
  slp::VariableMatrix qddot{4};
  qddot.segment(0, 2) = qdot;
  qddot.segment(2, 2) = solve(M, tau_g - C * qdot + B * u);
  return qddot;
}

inline void BM_CartPole(benchmark::State& state) {
  using namespace std::chrono_literals;

  // NOLINTNEXTLINE(clang-analyzer-deadcode.DeadStores)
  for (auto _ : state) {
    constexpr std::chrono::duration<double> T = 5s;
    constexpr std::chrono::duration<double> dt = 50ms;
    constexpr int N = T / dt;

    constexpr double u_max = 20.0;  // N
    constexpr double d_max = 2.0;   // m

    constexpr Eigen::Vector<double, 4> x_initial{{0.0, 0.0, 0.0, 0.0}};
    constexpr Eigen::Vector<double, 4> x_final{
        {1.0, std::numbers::pi, 0.0, 0.0}};

    slp::Problem problem;

    // x = [q, q̇]ᵀ = [x, θ, ẋ, θ̇]ᵀ
    auto X = problem.decision_variable(4, N + 1);

    // Initial guess
    for (int k = 0; k < N + 1; ++k) {
      X(0, k).set_value(
          std::lerp(x_initial[0], x_final[0], static_cast<double>(k) / N));
      X(1, k).set_value(
          std::lerp(x_initial[1], x_final[1], static_cast<double>(k) / N));
    }

    // u = f_x
    auto U = problem.decision_variable(1, N);

    // Initial conditions
    problem.subject_to(X.col(0) == x_initial);

    // Final conditions
    problem.subject_to(X.col(N) == x_final);

    // Cart position constraints
    problem.subject_to(X.row(0) >= 0.0);
    problem.subject_to(X.row(0) <= d_max);

    // Input constraints
    problem.subject_to(U >= -u_max);
    problem.subject_to(U <= u_max);

    // Dynamics constraints - RK4 integration
    for (int k = 0; k < N; ++k) {
      problem.subject_to(
          X.col(k + 1) ==
          wpi::math::RK4<decltype(CartPoleDynamics), slp::VariableMatrix,
                         slp::VariableMatrix>(CartPoleDynamics, X.col(k),
                                              U.col(k), dt));
    }

    // Minimize sum squared inputs
    slp::Variable J = 0.0;
    for (int k = 0; k < N; ++k) {
      J += U.col(k).T() * U.col(k);
    }
    problem.minimize(J);

    problem.solve({.diagnostics = true});
  }
}
