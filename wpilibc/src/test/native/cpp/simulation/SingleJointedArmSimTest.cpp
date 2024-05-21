// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <numbers>

#include <gtest/gtest.h>

#include "frc/simulation/SingleJointedArmSim.h"
#include "frc/system/plant/LinearSystemId.h"

TEST(SingleJointedArmTest, Disabled) {
  auto system = frc::LinearSystemId::SingleJointedArmSystem(
      frc::DCMotor::Vex775Pro(2), 15.5_kg, 30_in, 0_m, 300.0);
  frc::sim::SingleJointedArmSim sim(system, frc::DCMotor::Vex775Pro(2), 30_in,
                                    0_m, -180_deg, 0_deg, true, 90_deg);
  sim.SetState(0_rad, 0_rad_per_s);

  for (size_t i = 0; i < 12 / 0.02; ++i) {
    sim.SetInput(frc::Vectord<1>{0.0});
    sim.Update(20_ms);
  }

  // The arm should swing down.
  EXPECT_NEAR(sim.GetAngularPosition().value(), -std::numbers::pi / 2, 0.01);
}
