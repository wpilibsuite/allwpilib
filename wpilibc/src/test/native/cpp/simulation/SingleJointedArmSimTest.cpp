/*----------------------------------------------------------------------------*/
/* Copyright (c) 2020 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include <wpi/math>

#include "frc/simulation/SingleJointedArmSim.h"
#include "gtest/gtest.h"

TEST(SingleJointedArmTest, Disabled) {
  frc::sim::SingleJointedArmSim sim(frc::DCMotor::Vex775Pro(2), 100, 3_kg_sq_m,
                                    9.5_in, -180_deg, 0_deg, 10_lb, true);
  sim.SetState(frc::MakeMatrix<2, 1>(0.0, 0.0));

  for (size_t i = 0; i < 12 / 0.02; ++i) {
    sim.SetInput(frc::MakeMatrix<1, 1>(0.0));
    sim.Update(20_ms);
  }

  // The arm should swing down.
  EXPECT_NEAR(sim.GetAngle().to<double>(), -wpi::math::pi / 2, 0.01);
}
