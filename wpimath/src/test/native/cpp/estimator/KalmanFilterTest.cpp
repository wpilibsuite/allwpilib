// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/math/estimator/KalmanFilter.hpp"

#include <Eigen/Core>
#include <gtest/gtest.h>

#include "wpi/math/random/Normal.hpp"
#include "wpi/math/system/LinearSystem.hpp"
#include "wpi/math/trajectory/TrajectoryConfig.hpp"
#include "wpi/math/trajectory/TrajectoryGenerator.hpp"
#include "wpi/units/time.hpp"

TEST(KalmanFilterTest, SwerveStationary) {
  constexpr wpi::units::second_t dt = 20_ms;

  // Swerve drive with x = [x, y, θ, v_x, v_y, ω]ᵀ, u = [a_x, a_y, α]ᵀ,
  // y = [x, y, θ]ᵀ
  wpi::math::LinearSystem<6, 3, 3> plant{
      Eigen::Matrix<double, 6, 6>{{0, 0, 0, 1, 0, 0},
                                  {0, 0, 0, 0, 1, 0},
                                  {0, 0, 0, 0, 0, 1},
                                  {0, 0, 0, 0, 0, 0},
                                  {0, 0, 0, 0, 0, 0},
                                  {0, 0, 0, 0, 0, 0}},
      Eigen::Matrix<double, 6, 3>{
          {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {1, 0, 0}, {0, 1, 0}, {0, 0, 1}},
      Eigen::Matrix<double, 3, 6>{
          {1, 0, 0, 0, 0, 0}, {0, 1, 0, 0, 0, 0}, {0, 0, 1, 0, 0, 0}},
      Eigen::Matrix<double, 3, 3>::Zero()};

  wpi::math::KalmanFilter<6, 3, 3> filter{
      plant, {0.1, 0.1, 0.1, 0.1, 0.1, 0.1}, {1.0, 1.0, 1.0}, dt};

  constexpr Eigen::Vector<double, 3> u{{0.0}, {0.0}, {0.0}};

  for (int i = 0; i < 100; ++i) {
    auto y = wpi::math::Normal(1.0, 1.0, 1.0);

    filter.Correct(u, y);
    filter.Predict(u, dt);
  }

  EXPECT_NEAR(0.0, filter.Xhat(0), 0.3);
  EXPECT_NEAR(0.0, filter.Xhat(1), 0.3);
}

TEST(KalmanFilterTest, SwerveBadInitialPose) {
  constexpr wpi::units::second_t dt = 20_ms;

  // Swerve drive with x = [x, y, θ, v_x, v_y, ω]ᵀ, u = [a_x, a_y, α]ᵀ,
  // y = [x, y, θ]ᵀ
  wpi::math::LinearSystem<6, 3, 3> plant{
      Eigen::Matrix<double, 6, 6>{{0, 0, 0, 1, 0, 0},
                                  {0, 0, 0, 0, 1, 0},
                                  {0, 0, 0, 0, 0, 1},
                                  {0, 0, 0, 0, 0, 0},
                                  {0, 0, 0, 0, 0, 0},
                                  {0, 0, 0, 0, 0, 0}},
      Eigen::Matrix<double, 6, 3>{
          {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {1, 0, 0}, {0, 1, 0}, {0, 0, 1}},
      Eigen::Matrix<double, 3, 6>{
          {1, 0, 0, 0, 0, 0}, {0, 1, 0, 0, 0, 0}, {0, 0, 1, 0, 0, 0}},
      Eigen::Matrix<double, 3, 3>::Zero()};

  wpi::math::KalmanFilter<6, 3, 3> filter{
      plant, {0.1, 0.1, 0.1, 0.1, 0.1, 0.1}, {0.1, 0.1, 0.25}, dt};

  // Set nonzero position
  filter.SetXhat(0, 0.5);
  filter.SetXhat(1, 0.5);

  constexpr Eigen::Vector<double, 3> u{{0.0}, {0.0}, {0.0}};

  // Let filter converge to zero position
  for (int i = 0; i < 300; ++i) {
    auto y = wpi::math::Normal(0.1, 0.1, 0.25);

    filter.Correct(u, y);
    filter.Predict(u, dt);
  }

  EXPECT_NEAR(0.0, filter.Xhat(0), 0.2);
  EXPECT_NEAR(0.0, filter.Xhat(1), 0.2);
}

TEST(KalmanFilterTest, SwerveMovingOverTrajectory) {
  constexpr wpi::units::second_t dt = 20_ms;

  // Swerve drive with x = [x, y, θ, v_x, v_y, ω]ᵀ, u = [a_x, a_y, α]ᵀ,
  // y = [x, y, θ]ᵀ
  wpi::math::LinearSystem<6, 3, 3> plant{
      Eigen::Matrix<double, 6, 6>{{0, 0, 0, 1, 0, 0},
                                  {0, 0, 0, 0, 1, 0},
                                  {0, 0, 0, 0, 0, 1},
                                  {0, 0, 0, 0, 0, 0},
                                  {0, 0, 0, 0, 0, 0},
                                  {0, 0, 0, 0, 0, 0}},
      Eigen::Matrix<double, 6, 3>{
          {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {1, 0, 0}, {0, 1, 0}, {0, 0, 1}},
      Eigen::Matrix<double, 3, 6>{
          {1, 0, 0, 0, 0, 0}, {0, 1, 0, 0, 0, 0}, {0, 0, 1, 0, 0, 0}},
      Eigen::Matrix<double, 3, 3>::Zero()};

  wpi::math::KalmanFilter<6, 3, 3> filter{
      plant, {0.1, 0.1, 0.1, 0.1, 0.1, 0.1}, {0.2, 0.2, 1.0 / 3.0}, dt};

  auto trajectory = wpi::math::TrajectoryGenerator::GenerateTrajectory(
      {wpi::math::Pose2d{0_m, 0_m, 0_rad}, wpi::math::Pose2d{5_m, 5_m, 0_rad}},
      wpi::math::TrajectoryConfig{2_mps, 2_mps_sq});

  Eigen::Vector3d lastVelocity{{0.0}, {0.0}, {0.0}};

  for (wpi::units::second_t t = 0_s; t < trajectory.TotalTime(); t += dt) {
    auto sample = trajectory.Sample(t);

    Eigen::Vector3d y{sample.pose.Translation().X().value(),
                      sample.pose.Translation().Y().value(),
                      sample.pose.Rotation().Radians().value()};
    y += wpi::math::Normal(0.2, 0.2, 1.0 / 3.0);

    Eigen::Vector3d velocity{
        (sample.velocity * sample.pose.Rotation().Cos()).value(),
        (sample.velocity * sample.pose.Rotation().Sin()).value(),
        (sample.curvature * sample.velocity).value()};
    Eigen::Vector3d u = (velocity - lastVelocity) / dt.value();

    filter.Correct(u, y);
    filter.Predict(u, dt);

    lastVelocity = velocity;
  }

  EXPECT_NEAR(
      trajectory.Sample(trajectory.TotalTime()).pose.Translation().X().value(),
      filter.Xhat(0), 0.2);
  EXPECT_NEAR(
      trajectory.Sample(trajectory.TotalTime()).pose.Translation().Y().value(),
      filter.Xhat(1), 0.2);
}
