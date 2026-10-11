// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <Eigen/Core>
#include <benchmark/benchmark.h>
#include <unsupported/Eigen/MatrixFunctions>

inline void BM_MatrixExp(benchmark::State& state) {
  // NOLINTNEXTLINE(clang-analyzer-deadcode.DeadStores)
  for (auto _ : state) {
    // Pascal matrix
    //
    //    ([0  0  0  0  0  0  0])   [1  0   0   0   0  0  0]
    //    ([1  0  0  0  0  0  0])   [1  1   0   0   0  0  0]
    //    ([0  2  0  0  0  0  0])   [1  2   1   0   0  0  0]
    // exp([0  0  3  0  0  0  0]) = [1  3   3   1   0  0  0]
    //    ([0  0  0  4  0  0  0])   [1  4   6   4   1  0  0]
    //    ([0  0  0  0  5  0  0])   [1  5  10  10   5  1  0]
    //    ([0  0  0  0  0  6  0])   [1  6  15  20  15  6  1]
    Eigen::Matrix<double, 7, 7> pascal = Eigen::Matrix<double, 7, 7>::Zero();
    for (int col = 0; col < 6; ++col) {
      pascal(col + 1, col) = col + 1;
    }
    Eigen::Matrix<double, 7, 7> result = pascal.exp();
    benchmark::DoNotOptimize(result);
  }
}
