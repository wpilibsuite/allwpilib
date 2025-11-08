// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <benchmark/benchmark.h>

#include "wpi/math/geometry/Pose2d.hpp"
#include "wpi/math/path/TravelingSalesman.hpp"
#include "wpi/units/angle.hpp"
#include "wpi/units/length.hpp"
#include "wpi/util/array.hpp"

static constexpr wpi::util::array<wpi::math::Pose2d, 6> poses{
    wpi::math::Pose2d{-1_m, 1_m, -90_deg}, wpi::math::Pose2d{-1_m, 2_m, 90_deg},
    wpi::math::Pose2d{0_m, 0_m, 0_deg},    wpi::math::Pose2d{0_m, 3_m, -90_deg},
    wpi::math::Pose2d{1_m, 1_m, 90_deg},   wpi::math::Pose2d{1_m, 2_m, 90_deg},
};
static constexpr int iterations = 100;

void BM_Transform(benchmark::State& state) {
  wpi::math::TravelingSalesman traveler{[](auto pose1, auto pose2) {
    auto transform = pose2 - pose1;
    return wpi::units::math::hypot(transform.X(), transform.Y()).value();
  }};
  // NOLINTNEXTLINE(clang-analyzer-deadcode.DeadStores)
  for (auto _ : state) {
    traveler.Solve(poses, iterations);
  }
}
BENCHMARK(BM_Transform);

void BM_Twist(benchmark::State& state) {
  wpi::math::TravelingSalesman traveler{[](auto pose1, auto pose2) {
    auto twist = (pose2 - pose1).Log();
    return wpi::units::math::hypot(twist.dx, twist.dy).value();
  }};
  // NOLINTNEXTLINE(clang-analyzer-deadcode.DeadStores)
  for (auto _ : state) {
    traveler.Solve(poses, iterations);
  }
}
BENCHMARK(BM_Twist);

BENCHMARK_MAIN();
