// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <benchmark/benchmark.h>

#include "wpi/math/geometry/Transform3d.hpp"
#include "wpi/math/geometry/Twist3d.hpp"
#include "wpi/units/angle.hpp"
#include "wpi/units/length.hpp"

inline constexpr wpi::math::Twist3d BENCHMARK_TWIST{1_m,     2_m,      0.5_m,
                                                    0.3_rad, -0.2_rad, 0.7_rad};

inline void BM_Geometry3d_Twist3d_Exp(benchmark::State& state) {
  wpi::math::Twist3d twist = BENCHMARK_TWIST;

  // NOLINTNEXTLINE(clang-analyzer-deadcode.DeadStores)
  for (auto _ : state) {
    // Prevent the compiler from constant-folding the constexpr function
    benchmark::DoNotOptimize(twist);
    auto transform = twist.Exp();
    benchmark::DoNotOptimize(transform);
  }
}

inline void BM_Geometry3d_Transform3d_Log(benchmark::State& state) {
  wpi::math::Transform3d transform = BENCHMARK_TWIST.Exp();

  // NOLINTNEXTLINE(clang-analyzer-deadcode.DeadStores)
  for (auto _ : state) {
    // Prevent the compiler from constant-folding the constexpr function
    benchmark::DoNotOptimize(transform);
    auto twist = transform.Log();
    benchmark::DoNotOptimize(twist);
  }
}
