// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <Eigen/Cholesky>
#include <Eigen/Core>
#include <benchmark/benchmark.h>

inline void CholeskyRankUpdate(benchmark::State& state, double sigma) {
  // Symmetric positive definite matrix
  const Eigen::Matrix<double, 5, 5> A{{4.0, 1.0, 0.5, 0.0, 0.2},
                                      {1.0, 5.0, 1.0, 0.3, 0.0},
                                      {0.5, 1.0, 6.0, 1.0, 0.4},
                                      {0.0, 0.3, 1.0, 7.0, 1.0},
                                      {0.2, 0.0, 0.4, 1.0, 8.0}};
  const Eigen::Matrix<double, 5, 5> L = A.llt().matrixL();
  const Eigen::Vector<double, 5> v{{0.1, 0.2, 0.3, 0.4, 0.5}};

  // NOLINTNEXTLINE(clang-analyzer-deadcode.DeadStores)
  for (auto _ : state) {
    // The rank update is in-place, so start from the original factor each
    // iteration
    Eigen::Matrix<double, 5, 5> S = L;
    Eigen::internal::llt_inplace<double, Eigen::Lower>::rankUpdate(S, v, sigma);
    benchmark::DoNotOptimize(S);
  }
}

inline void BM_CholeskyRankUpdate(benchmark::State& state) {
  CholeskyRankUpdate(state, 1.0);
}

inline void BM_CholeskyRankDowndate(benchmark::State& state) {
  CholeskyRankUpdate(state, -1.0);
}
