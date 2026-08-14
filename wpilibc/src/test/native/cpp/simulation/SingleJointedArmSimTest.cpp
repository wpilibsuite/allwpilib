// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/simulation/SingleJointedArmSim.hpp"

#include <numbers>

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

TEST_CASE("SingleJointedArmTest Disabled", "[wpilibc][simulation]") {
  wpi::sim::SingleJointedArmSim sim(wpi::math::DCMotor::Vex775Pro(2), 300,
                                    3_kg_sq_m, 30_in, -180_deg, 0_deg, true,
                                    90_deg);
  sim.SetState(wpi::math::Vectord<2>{0.0, 0.0});

  for (size_t i = 0; i < 12 / 0.02; ++i) {
    sim.SetInput(wpi::math::Vectord<1>{0.0});
    sim.Update(20_ms);
  }

  // The arm should swing down.
  CHECK_THAT(sim.GetAngle().value(),
             Catch::Matchers::WithinAbs(-std::numbers::pi / 2, 0.01));
}

TEST_CASE("SingleJointedArmTest InitialState", "[wpilibc][simulation]") {
  constexpr auto startingAngle = 45_deg;
  wpi::sim::SingleJointedArmSim sim(wpi::math::DCMotor::KrakenX60(2), 125,
                                    3_kg_sq_m, 30_in, 0_deg, 90_deg, true,
                                    startingAngle);

  CHECK(startingAngle == sim.GetAngle());
  CHECK_THAT(0, Catch::Matchers::WithinULP(sim.GetVelocity().value(), 4));
}
