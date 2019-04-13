/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "gtest/gtest.h"
#include "lowfisim/motormodel/SimpleMotorModel.h"

TEST(SimpleMotorModelSimulationTest, TestSimpleModel) {
  frc::sim::lowfi::SimpleMotorModel motorModelSim(200);

  // Test forward voltage
  motorModelSim.SetVoltage(6);
  motorModelSim.Update(.5);

  EXPECT_DOUBLE_EQ(50, motorModelSim.GetPosition());
  EXPECT_DOUBLE_EQ(100, motorModelSim.GetVelocity());

  // Test Reset
  motorModelSim.Reset();
  EXPECT_DOUBLE_EQ(0, motorModelSim.GetPosition());
  EXPECT_DOUBLE_EQ(0, motorModelSim.GetVelocity());

  // Test negative voltage
  motorModelSim.Reset();
  motorModelSim.SetVoltage(-3);
  motorModelSim.Update(.06);

  EXPECT_DOUBLE_EQ(-3, motorModelSim.GetPosition());
  EXPECT_DOUBLE_EQ(-50, motorModelSim.GetVelocity());
}
