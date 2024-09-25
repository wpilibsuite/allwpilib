// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <chrono>
#include <cstddef>

#include <gtest/gtest.h>
#include <wpi/array.h>
#include <wpi/print.h>

#include "frc/estimator/DifferentialDrivePoseEstimator.h"
#include "frc/estimator/DifferentialDrivePoseEstimator3d.h"
#include "frc/geometry/Pose2d.h"
#include "frc/geometry/Pose3d.h"
#include "frc/geometry/Rotation2d.h"
#include "frc/geometry/Rotation3d.h"
#include "frc/kinematics/DifferentialDriveOdometry.h"
#include "frc/kinematics/DifferentialDriveOdometry3d.h"
#include "units/length.h"
#include "units/math.h"
#include "units/time.h"

template <typename T, size_t N, typename CharT>
struct fmt::formatter<wpi::array<T, N>, CharT> {
  template <typename ParseContext>
  constexpr auto parse(ParseContext& ctx) {
    return m_underlying.parse(ctx);
  }

  template <typename FmtContext>
  auto format(const wpi::array<T, N>& arr, FmtContext& ctx) const {
    auto out = ctx.out();

    out = fmt::format_to(out, "[");

    for (size_t i = 0; i < N; ++i) {
      out = m_underlying.format(arr[i], ctx);
      if (i < N - 1) {
        out = fmt::format_to(out, ", ");
      }
    }

    out = fmt::format_to(out, "]");

    return out;
  }

 private:
  fmt::formatter<T, CharT> m_underlying;
};

template <size_t N>
void ProcessDurations(const wpi::array<units::nanosecond_t, N>& durations,
                      std::string_view prefix = "") {
  units::nanosecond_t total_duration = 0_ns;
  for (auto duration : durations) {
    total_duration += duration;
  }

  units::nanosecond_t mean = total_duration / N;

  auto sum_squares = 0_ns * 0_ns;
  for (auto duration : durations) {
    sum_squares += (duration - mean) * (duration - mean);
  }

  units::nanosecond_t std_dev = units::math::sqrt(sum_squares / N);

  wpi::print("{}Mean: {}, Std dev: {}\n", prefix, mean, std_dev);

  wpi::array<units::nanosecond_t, 10> buffer{wpi::empty_array};

  for (size_t i = 0; i < 10; ++i) {
    buffer[i] = durations[i];
  }

  wpi::print("{}First 10: {}\n", prefix, buffer);

  for (size_t i = 0; i < 10; ++i) {
    buffer[i] = durations[N - 10 + i];
  }

  wpi::print("{}Last 10: {}\n", prefix, buffer);

  wpi::array<units::nanosecond_t, N> sorted{durations};
  std::sort(sorted.begin(), sorted.end());

  for (size_t i = 0; i < 10; ++i) {
    buffer[i] = sorted[i];
  }

  wpi::print("{}Fastest 10: {}\n", prefix, buffer);

  for (size_t i = 0; i < 10; ++i) {
    buffer[i] = sorted[N - 10 + i];
  }

  wpi::print("{}Slowest 10: {}\n", prefix, buffer);
}

template <size_t N>
void Time(
    std::function<void()> action, std::function<void()> setup = [] {},
    std::string_view prefix = "") {
  wpi::array<units::nanosecond_t, N> durations(wpi::empty_array);

  for (size_t i = 0; i < N; ++i) {
    setup();
    auto start = std::chrono::steady_clock::now();
    action();
    auto end = std::chrono::steady_clock::now();
    durations[i] = end - start;
  }

  ProcessDurations(durations, prefix);
}

void TimeSuite(
    std::string_view name, std::function<void()> action,
    std::function<void()> setup = [] {}) {
  fmt::print("{}:\n", name);
  fmt::print("  Warmup: (100,000 iterations):\n");
  Time<100'000>(action, setup, "    ");
  for (size_t i = 0; i < 5; ++i) {
    fmt::print("  Run {}:\n", i);
    Time<1'000>(action, setup, "    ");
  }
}

TEST(TimeTest, Time) {
  {
    frc::DifferentialDriveOdometry odometry{frc::Rotation2d{}, 0_m, 0_m,
                                            frc::Pose2d{}};
    frc::Rotation2d gyroAngle{};
    auto leftDistance = 0_m;
    auto rightDistance = 0_m;
    TimeSuite(
        "Odometry update (2d)",
        [&] { odometry.Update(gyroAngle, leftDistance, rightDistance); },
        [&] {
          odometry.ResetPosition(frc::Rotation2d{}, 0_m, 0_m, frc::Pose2d{});
        });
  }

  {
    frc::DifferentialDriveOdometry3d odometry{frc::Rotation3d{}, 0_m, 0_m,
                                              frc::Pose3d{}};
    frc::Rotation3d gyroAngle{};
    auto leftDistance = 0_m;
    auto rightDistance = 0_m;
    TimeSuite(
        "Odometry update (3d)",
        [&] { odometry.Update(gyroAngle, leftDistance, rightDistance); },
        [&] {
          odometry.ResetPosition(frc::Rotation3d{}, 0_m, 0_m, frc::Pose3d{});
        });
  }

  {
    frc::DifferentialDriveKinematics kinematics{1_m};
    frc::DifferentialDrivePoseEstimator poseEstimator{
        kinematics, frc::Rotation2d{}, 0_m, 0_m, frc::Pose2d{}};
    frc::Rotation2d gyroAngle{};
    auto leftDistance = 0_m;
    auto rightDistance = 0_m;
    TimeSuite(
        "Pose estimator update (2d)",
        [&] { poseEstimator.Update(gyroAngle, leftDistance, rightDistance); },
        [&] {
          poseEstimator.ResetPosition(frc::Rotation2d{}, 0_m, 0_m,
                                      frc::Pose2d{});
        });
  }

  {
    frc::DifferentialDriveKinematics kinematics{1_m};
    frc::DifferentialDrivePoseEstimator3d poseEstimator{
        kinematics, frc::Rotation3d{}, 0_m, 0_m, frc::Pose3d{}};
    frc::Rotation3d gyroAngle{};
    auto leftDistance = 0_m;
    auto rightDistance = 0_m;
    TimeSuite(
        "Pose estimator update (3d)",
        [&] { poseEstimator.Update(gyroAngle, leftDistance, rightDistance); },
        [&] {
          poseEstimator.ResetPosition(frc::Rotation3d{}, 0_m, 0_m,
                                      frc::Pose3d{});
        });
  }
}
