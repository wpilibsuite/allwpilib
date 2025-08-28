// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <numbers>

#include <gtest/gtest.h>

#include "frc/simulation/SingleJointedArmSim.h"

using namespace wpi::math;

TEST(SingleJointedArmTest, Disabled) {
  sim::SingleJointedArmSim sim(DCMotor::Vex775Pro(2), 300, 3_kg_sq_m, 30_in,
                               -180_deg, 0_deg, true, 90_deg);
  sim.SetState(Vectord<2>{0.0, 0.0});

  for (size_t i = 0; i < 12 / 0.02; ++i) {
    sim.SetInput(Vectord<1>{0.0});
    sim.Update(20_ms);
  }

  // The arm should swing down.
  EXPECT_NEAR(sim.GetAngle().value(), -std::numbers::pi / 2, 0.01);
}

TEST(SingleJointedArmTest, InitialState) {
  constexpr auto startingAngle = 45_deg;
  sim::SingleJointedArmSim sim(DCMotor::KrakenX60(2), 125, 3_kg_sq_m, 30_in,
                               0_deg, 90_deg, true, startingAngle);

  EXPECT_EQ(startingAngle, sim.GetAngle());
  EXPECT_DOUBLE_EQ(0, sim.GetVelocity().value());
}
