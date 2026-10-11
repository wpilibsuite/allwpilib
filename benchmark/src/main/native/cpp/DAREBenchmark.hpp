// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <Eigen/Core>
#include <benchmark/benchmark.h>

#include "wpi/math/linalg/DARE.hpp"
#include "wpi/math/system/Discretization.hpp"
#include "wpi/math/system/Models.hpp"
#include "wpi/math/util/StateSpaceUtil.hpp"
#include "wpi/units/acceleration.hpp"
#include "wpi/units/time.hpp"
#include "wpi/units/velocity.hpp"
#include "wpi/units/voltage.hpp"

inline void BM_DARE(benchmark::State& state) {
  //     [x ]
  //     [y ]       [Vₗ]
  // x = [θ ]   u = [Vᵣ]
  //     [vₗ]
  //     [vᵣ]
  constexpr auto plant = wpi::math::Models::DifferentialDriveFromSysId(
      3.02_V / 1_mps, 0.642_V / 1_mps_sq, 1.382_V / 1_mps,
      0.08495_V / 1_mps_sq);
  constexpr double trackwidth = 0.9;  // m
  constexpr double velocity = 1.0;    // m/s
  constexpr wpi::units::second_t dt = 20_ms;

  constexpr Eigen::Matrix<double, 5, 5> A{
      {0.0, 0.0, 0.0, 0.5, 0.5},
      {0.0, 0.0, velocity, 0.0, 0.0},
      {0.0, 0.0, 0.0, -1.0 / trackwidth, 1.0 / trackwidth},
      {0.0, 0.0, 0.0, plant.A()(0, 0), plant.A()(0, 1)},
      {0.0, 0.0, 0.0, plant.A()(1, 0), plant.A()(1, 1)}};
  constexpr Eigen::Matrix<double, 5, 2> B{{0.0, 0.0},
                                          {0.0, 0.0},
                                          {0.0, 0.0},
                                          {plant.B()(0, 0), plant.B()(0, 1)},
                                          {plant.B()(1, 0), plant.B()(1, 1)}};
  constexpr auto Q = wpi::math::CostMatrix(0.0625, 0.125, 2.5, 0.95, 0.95);
  constexpr auto R = wpi::math::CostMatrix(12.0, 12.0);

  Eigen::Matrix<double, 5, 5> discA;
  Eigen::Matrix<double, 5, 2> discB;
  wpi::math::DiscretizeAB(A, B, dt, &discA, &discB);

  // NOLINTNEXTLINE(clang-analyzer-deadcode.DeadStores)
  for (auto _ : state) {
    auto S = wpi::math::DARE<5, 2>(discA, discB, Q, R, false).value();
    benchmark::DoNotOptimize(S);
  }
}
