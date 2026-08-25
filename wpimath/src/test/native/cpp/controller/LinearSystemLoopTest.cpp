// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/math/system/LinearSystemLoop.hpp"

#include <numbers>
#include <random>

#include <catch2/catch_test_macros.hpp>

#include "wpi/math/TestAssertions.hpp"
#include "wpi/math/controller/LinearPlantInversionFeedforward.hpp"
#include "wpi/math/controller/LinearQuadraticRegulator.hpp"
#include "wpi/math/estimator/KalmanFilter.hpp"
#include "wpi/math/linalg/EigenCore.hpp"
#include "wpi/math/system/DCMotor.hpp"
#include "wpi/math/system/LinearSystem.hpp"
#include "wpi/math/system/Models.hpp"
#include "wpi/math/trajectory/TrapezoidProfile.hpp"
#include "wpi/units/acceleration.hpp"
#include "wpi/units/length.hpp"
#include "wpi/units/mass.hpp"
#include "wpi/units/moment_of_inertia.hpp"
#include "wpi/units/time.hpp"
#include "wpi/units/velocity.hpp"
#include "wpi/units/voltage.hpp"

inline constexpr wpi::units::seconds<> DT = 5_ms;
inline constexpr double POSITION_STDDEV = 0.0001;

namespace {
std::default_random_engine generator;
std::normal_distribution<double> distribution{0.0, 1.0};
}  // namespace

TEST_CASE("LinearSystemLoopTest StateSpaceEnabled", "[wpimath]") {
  wpi::math::LinearSystem<2, 1, 2> plant{
      wpi::math::Models::ElevatorFromPhysicalConstants(
          wpi::math::DCMotor::Vex775Pro(2), 5_kg, 0.0181864_m, 1.0)};

  wpi::math::LinearSystem<2, 1, 1> slicedPlant{plant.Slice(0)};

  wpi::math::KalmanFilter<2, 1, 1> observer{
      slicedPlant, {0.05, 1.0}, {POSITION_STDDEV}, DT};

  wpi::math::LinearQuadraticRegulator<2, 1> controller{
      slicedPlant, {0.02, 0.4}, {12.0}, DT};

  wpi::math::LinearSystemLoop<2, 1, 1> loop{slicedPlant, controller, observer,
                                            12_V, DT};
  loop.Reset({0, 0});

  wpi::math::Vectord<2> references{2.0, 0.0};
  loop.SetNextR(references);

  wpi::math::TrapezoidProfile<wpi::units::meters_>::Constraints constraints{
      4_mps, 3_mps2};
  wpi::math::TrapezoidProfile<wpi::units::meters_> profile{constraints};

  wpi::math::TrapezoidProfile<wpi::units::meters_>::State state{
      wpi::units::meters<>{loop.Xhat(0)},
      wpi::units::meters_per_second<>{loop.Xhat(1)}};
  for (int i = 0; i < 1000; ++i) {
    state = profile.Calculate(DT, state,
                              {wpi::units::meters<>{references(0)},
                               wpi::units::meters_per_second<>{references(1)}});
    loop.SetNextR({state.position.value(), state.velocity.value()});

    wpi::math::Matrixd<1, 1> y{
        slicedPlant.CalculateY(loop.Xhat(), loop.U()) +
        wpi::math::Vectord<1>{distribution(generator) * POSITION_STDDEV}};

    loop.Correct(y);
    loop.Predict(DT);

    double u = loop.U(0);

    CHECK(u > -12.1);
    CHECK(u <= 12.1);
  }

  CHECK_NEAR(2.0, loop.Xhat(0), 0.05);
  CHECK_NEAR(0.0, loop.Xhat(1), 0.5);
}

TEST_CASE("LinearSystemLoopTest FlywheelEnabled", "[wpimath]") {
  wpi::math::LinearSystem<1, 1, 1> plant{
      wpi::math::Models::FlywheelFromPhysicalConstants(
          wpi::math::DCMotor::NEO(2), 0.00289_kg_sq_m, 1.0)};

  wpi::math::KalmanFilter<1, 1, 1> observer{
      plant, {1.0}, {POSITION_STDDEV}, DT};

  wpi::math::LinearQuadraticRegulator<1, 1> controller{
      plant, {9.0}, {12.0}, DT};

  wpi::math::LinearPlantInversionFeedforward feedforward{plant, DT};

  wpi::math::LinearSystemLoop<1, 1, 1> loop{controller, feedforward, observer,
                                            12_V};
  loop.Reset(wpi::math::Vectord<1>{0.0});

  wpi::math::Vectord<1> references{3000.0 / 60.0 * 2.0 * std::numbers::pi};
  loop.SetNextR(references);

  wpi::units::seconds<> time = 0_s;
  while (time < 10_s) {
    loop.SetNextR(references);
    wpi::math::Matrixd<1, 1> y{
        plant.CalculateY(loop.Xhat(), loop.U()) +
        wpi::math::Vectord<1>{distribution(generator) * POSITION_STDDEV}};

    loop.Correct(y);
    loop.Predict(DT);

    double u = loop.U(0);

    CHECK(u > -12.1);
    CHECK(u <= 12.1);

    time += DT;
  }

  CHECK_NEAR(0.0, loop.Error().value(), 0.1);
}

TEST_CASE("LinearSystemLoopTest AtReference", "[wpimath]") {
  wpi::math::LinearSystem<2, 1, 2> plant{
      wpi::math::Models::ElevatorFromPhysicalConstants(
          wpi::math::DCMotor::Vex775Pro(2), 5_kg, 0.0181864_m, 1.0)};

  wpi::math::LinearSystem<2, 1, 1> slicedPlant{plant.Slice(0)};

  wpi::math::KalmanFilter<2, 1, 1> observer{
      slicedPlant, {0.05, 1.0}, {POSITION_STDDEV}, DT};

  wpi::math::LinearQuadraticRegulator<2, 1> controller{
      slicedPlant, {0.02, 0.4}, {12.0}, DT};

  wpi::math::LinearSystemLoop<2, 1, 1> loop{slicedPlant, controller, observer,
                                            12_V, DT};
  loop.Reset({0, 0});

  // Default tolerance is zero and the error is zero, so the loop is at
  // reference.
  CHECK(loop.AtReference());

  loop.SetTolerance({0.1, 0.2});
  loop.SetNextR({0, 0});

  // AtReference() delegates to the controller, whose error is snapshotted
  // during Predict() as nextR - xHat.
  loop.SetXhat({0.05, 0.1});
  loop.Predict(DT);
  CHECK(loop.AtReference());

  loop.SetXhat({0.2, 0.1});
  loop.Predict(DT);
  CHECK_FALSE(loop.AtReference());

  // Error exactly at the tolerance boundary is considered at reference.
  loop.SetXhat({0.1, 0.2});
  loop.Predict(DT);
  CHECK(loop.AtReference());
}
