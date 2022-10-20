// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <numbers>

#include "frc/simulation/SingleJointedArmSim.h"
#include "gtest/gtest.h"

TEST(SingleJointedArmTest, Disabled) {
  frc::sim::SingleJointedArmSim sim(frc::DCMotor::Vex775Pro(2), 100, 3_kg_sq_m,
                                    9.5_in, -180_deg, 0_deg, 10_lb, true);
  sim.SetState(frc::Vectord<2>{0.0, 0.0});

  for (size_t i = 0; i < 12 / 0.02; ++i) {
    sim.SetInput(frc::Vectord<1>{0.0});
    sim.Update(20_ms);
  }

  // The arm should swing down.
  EXPECT_NEAR(sim.GetAngle().value(), -std::numbers::pi / 2, 0.01);
}
