// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/math/system/LinearSystemLoop.hpp"

#include <numbers>
#include <random>

#include <gtest/gtest.h>

#include "wpi/math/controller/LinearQuadraticRegulator.hpp"
#include "wpi/math/linalg/EigenCore.hpp"
#include "wpi/math/system/DCMotor.hpp"
#include "wpi/math/system/LinearSystem.hpp"
#include "wpi/math/system/Models.hpp"
#include "wpi/math/trajectory/TrapezoidProfile.hpp"
#include "wpi/units/length.hpp"
#include "wpi/units/mass.hpp"

namespace wpi::math {

constexpr units::second_t kDt = 5_ms;
constexpr double kPositionStddev = 0.0001;
std::default_random_engine generator{};
std::normal_distribution<double> distribution(0.0, 1.0);

TEST(LinearSystemLoopTest, StateSpaceEnabled) {
  LinearSystem<2, 1, 2> plant{Models::ElevatorFromPhysicalConstants(
      DCMotor::Vex775Pro(2), 5_kg, 0.0181864_m, 1.0)};

  LinearSystem<2, 1, 1> slicedPlant{plant.Slice(0)};

  KalmanFilter<2, 1, 1> observer{
      slicedPlant, wpi::util::array<double, 2>{0.05, 1.0},
      wpi::util::array<double, 1>{kPositionStddev}, kDt};

  LinearQuadraticRegulator<2, 1> controller{
      slicedPlant, {0.02, 0.4}, {12.0}, kDt};

  LinearSystemLoop<2, 1, 1> loop{slicedPlant, controller, observer, 12_V, kDt};

  loop.Reset({0, 0});
  Vectord<2> references{2.0, 0.0};

  TrapezoidProfile<units::meters>::Constraints constraints{4_mps, 3_mps_sq};

  loop.SetNextR(references);

  for (int i{0}; i < 1000; i++) {
    TrapezoidProfile<units::meters> profile{constraints};

    TrapezoidProfile<units::meters>::State current{
        units::meter_t{loop.Xhat(0)}, units::meters_per_second_t{loop.Xhat(1)}};

    TrapezoidProfile<units::meters>::State goal{
        units::meter_t{references(0)},
        units::meters_per_second_t{references(1)}};

    TrapezoidProfile<units::meters>::State state{
        profile.Calculate(kDt, current, goal)};

    loop.SetNextR({state.position.value(), state.velocity.value()});

    Matrixd<1, 1> y{slicedPlant.CalculateY(loop.Xhat(), loop.U()) +
                    Vectord<1>{distribution(generator) * kPositionStddev}};

    loop.Correct(y);
    loop.Predict(kDt);

    double u = loop.U(0);

    EXPECT_GT(u, -12.1);
    EXPECT_LE(u, 12.1);
  }

  EXPECT_NEAR(2.0, loop.Xhat(0), 0.05);
  EXPECT_NEAR(0.0, loop.Xhat(1), 0.5);
}

TEST(LinearSystemLoopTest, FlywheelEnabled) {
  LinearSystem<1, 1, 1> plant{Models::FlywheelFromPhysicalConstants(
      DCMotor::NEO(2), 0.00289_kg_sq_m, 1.0)};

  KalmanFilter<1, 1, 1> observer{plant, {1.0}, {kPositionStddev}, kDt};

  LinearQuadraticRegulator<1, 1> controller{
      plant, wpi::util::array<double, 1>{9.0},
      wpi::util::array<double, 1>{12.0}, kDt};

  LinearPlantInversionFeedforward feedforward{plant, kDt};

  LinearSystemLoop<1, 1, 1> loop{controller, feedforward, observer, 12_V};

  loop.Reset(Vectord<1>{0.0});
  Vectord<1> references{3000.0 / 60.0 * 2.0 * std::numbers::pi};

  loop.SetNextR(references);

  double time = 0.0;
  while (time < 10) {
    loop.SetNextR(references);
    Matrixd<1, 1> y{plant.CalculateY(loop.Xhat(), loop.U()) +
                    Vectord<1>{distribution(generator) * kPositionStddev}};

    loop.Correct(y);
    loop.Predict(kDt);

    double u = loop.U(0);

    EXPECT_GT(u, -12.1);
    EXPECT_LE(u, 12.1);

    time += kDt.value();
  }

  EXPECT_NEAR(0.0, loop.Error().value(), 0.1);
}

}  // namespace wpi::math
