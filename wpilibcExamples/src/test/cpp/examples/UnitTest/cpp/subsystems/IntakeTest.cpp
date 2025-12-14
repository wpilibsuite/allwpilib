// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "subsystems/Intake.hpp"

#include <gtest/gtest.h>

#include "Constants.hpp"
#include "wpi/hardware/pneumatic/DoubleSolenoid.hpp"
#include "wpi/simulation/DoubleSolenoidSim.hpp"
#include "wpi/simulation/PWMMotorControllerSim.hpp"

class IntakeTest : public testing::Test {
 protected:
  Intake intake;  // create our intake
  wpi::sim::PWMMotorControllerSim simMotor{
      IntakeConstants::kMotorPort};  // create our simulation PWM
  wpi::sim::DoubleSolenoidSim simPiston{
      wpi::PneumaticsModuleType::CTREPCM, IntakeConstants::kPistonFwdChannel,
      IntakeConstants::kPistonRevChannel};  // create our simulation solenoid
};

TEST_F(IntakeTest, DoesntWorkWhenClosed) {
  intake.Retract();      // close the intake
  intake.Activate(0.5);  // try to activate the motor
  EXPECT_DOUBLE_EQ(
      0.0,
      simMotor
          .GetDutyCycle());  // make sure that the value set to the motor is 0
}

TEST_F(IntakeTest, WorksWhenOpen) {
  intake.Deploy();
  intake.Activate(0.5);
  EXPECT_DOUBLE_EQ(0.5, simMotor.GetDutyCycle());
}

TEST_F(IntakeTest, Retract) {
  intake.Retract();
  EXPECT_EQ(wpi::DoubleSolenoid::Value::kReverse, simPiston.Get());
}

TEST_F(IntakeTest, Deploy) {
  intake.Deploy();
  EXPECT_EQ(wpi::DoubleSolenoid::Value::kForward, simPiston.Get());
}
