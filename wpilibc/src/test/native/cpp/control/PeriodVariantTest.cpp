/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include <control/PeriodVariant/ElevatorCoeffs.h>
#include <frc/controller/PeriodVariantLoop.h>

#include <cmath>
#include <random>

#include <Eigen/Core>

#include "gtest/gtest.h"

using namespace frc;

constexpr double kNominalDt = 0.005;
constexpr double kPositionStddev = 0.0001;

void update(PeriodVariantLoop<2, 1, 1>& loop, double dt, double yNoise) {
  Eigen::Matrix<double, 1, 1> y =
      loop.GetPlant().CalculateY(loop.Xhat(), loop.U()) +
      Eigen::Matrix<double, 1, 1>(yNoise);

  loop.Correct(y);
  loop.Predict(units::second_t{dt});
}

TEST(PeriodVariant, TestEnabled) {
  PeriodVariantLoop<2, 1, 1> m_loop{MakeElevatorPVLoop()};
  std::default_random_engine generator;
  std::normal_distribution<double> dist;

  Eigen::Matrix<double, 2, 1> references;
  references << 2.0, 0.0;
  m_loop.SetNextR(references);

  m_loop.Enable();

  for (int i = 0; i < 350; i++) {
    double dtNoise = dist(generator);
    double dt = std::abs(kNominalDt + (dtNoise * 0.02));
    double yNoise = dist(generator) * (kPositionStddev / kNominalDt) * dt;
    update(m_loop, dt, yNoise);
    EXPECT_GE(m_loop.U(0), -12.0);
    EXPECT_LE(m_loop.U(0), 12.0);
  }

  EXPECT_LT(std::abs(m_loop.Xhat(0) - 2.0), 0.05);
  EXPECT_LT(std::abs(m_loop.Xhat(1) - 0.0), 0.5);
}

TEST(PeriodVariant, TestDisabled) {
  PeriodVariantLoop<2, 1, 1> m_loop{MakeElevatorPVLoop()};

  Eigen::Matrix<double, 2, 1> references;
  references << 2.0, 0.0;

  m_loop.SetNextR(references);

  for (int i = 0; i < 100; i++) {
    update(m_loop, 0.0, 0.0);
  }

  auto position = m_loop.Xhat(0);
  auto velocity = m_loop.Xhat(1);

  EXPECT_LT(std::abs(position), 1E-9);
  EXPECT_LT(std::abs(velocity), 1E-9);
}
