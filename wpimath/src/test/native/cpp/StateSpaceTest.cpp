/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include <gtest/gtest.h>

#include <cmath>
#include <random>

#include "Eigen/Core"
#include "frc/controller/LinearPlantInversionFeedforward.h"
#include "frc/controller/LinearQuadraticRegulator.h"
#include "frc/estimator/KalmanFilter.h"
#include "frc/system/LinearSystem.h"
#include "frc/system/LinearSystemLoop.h"
#include "frc/system/plant/DCMotor.h"
#include "frc/system/plant/LinearSystemId.h"
#include "units/time.h"

namespace frc {

constexpr double kPositionStddev = 0.0001;
constexpr auto kDt = 0.00505_s;

class StateSpace : public testing::Test {
 public:
  LinearSystem<2, 1, 1> plant = [] {
    auto motors = DCMotor::Vex775Pro(2);

    // Carriage mass
    constexpr auto m = 5_kg;

    // Radius of pulley
    constexpr auto r = 0.0181864_m;

    // Gear ratio
    constexpr double G = 40.0 / 40.0;

    return frc::LinearSystemId::ElevatorSystem(motors, m, r, G);
  }();
  LinearQuadraticRegulator<2, 1> controller{plant, {0.02, 0.4}, {12.0}, kDt};
  KalmanFilter<2, 1, 1> observer{plant, {0.05, 1.0}, {0.0001}, kDt};
  LinearSystemLoop<2, 1, 1> loop{plant, controller, observer, 12_V, kDt};
};

void Update(LinearSystemLoop<2, 1, 1>& loop, double noise) {
  Eigen::Matrix<double, 1, 1> y =
      loop.Plant().CalculateY(loop.Xhat(), loop.U()) +
      Eigen::Matrix<double, 1, 1>(noise);
  loop.Correct(y);
  loop.Predict(kDt);
}

TEST_F(StateSpace, CorrectPredictLoop) {
  std::default_random_engine generator;
  std::normal_distribution<double> dist{0.0, kPositionStddev};

  Eigen::Matrix<double, 2, 1> references;
  references << 2.0, 0.0;
  loop.SetNextR(references);

  for (int i = 0; i < 1000; i++) {
    Update(loop, dist(generator));
    EXPECT_PRED_FORMAT2(testing::DoubleLE, -12.0, loop.U(0));
    EXPECT_PRED_FORMAT2(testing::DoubleLE, loop.U(0), 12.0);
  }

  EXPECT_NEAR(loop.Xhat(0), 2.0, 0.05);
  EXPECT_NEAR(loop.Xhat(1), 0.0, 0.5);
}

}  // namespace frc
