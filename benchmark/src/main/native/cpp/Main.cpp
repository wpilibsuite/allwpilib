// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <benchmark/benchmark.h>
#include <frc/geometry/Pose2d.h>
#include <frc/path/TravelingSalesman.h>

#include <units/angle.h>
#include <units/length.h>
#include <wpi/array.h>

static constexpr wpi::array<frc::Pose2d, 6> poses{
    frc::Pose2d{-1_m, 1_m, -90_deg}, frc::Pose2d{-1_m, 2_m, 90_deg},
    frc::Pose2d{0_m, 0_m, 0_deg},    frc::Pose2d{0_m, 3_m, -90_deg},
    frc::Pose2d{1_m, 1_m, 90_deg},   frc::Pose2d{1_m, 2_m, 90_deg},
};
static constexpr int iterations = 100;

void BM_Transform(benchmark::State& state) {
  frc::TravelingSalesman traveler{[](auto pose1, auto pose2) {
    auto transform = pose2 - pose1;
    return units::math::hypot(transform.X(), transform.Y()).value();
  }};
  // NOLINTNEXTLINE(clang-analyzer-deadcode.DeadStores)
  for (auto _ : state) {
    traveler.Solve(poses, iterations);
  }
}
BENCHMARK(BM_Transform);

void BM_Twist(benchmark::State& state) {
  frc::TravelingSalesman traveler{[](auto pose1, auto pose2) {
    auto twist = pose1.Log(pose2);
    return units::math::hypot(twist.dx, twist.dy).value();
  }};
  // NOLINTNEXTLINE(clang-analyzer-deadcode.DeadStores)
  for (auto _ : state) {
    traveler.Solve(poses, iterations);
  }
}
BENCHMARK(BM_Twist);

BENCHMARK_MAIN();
