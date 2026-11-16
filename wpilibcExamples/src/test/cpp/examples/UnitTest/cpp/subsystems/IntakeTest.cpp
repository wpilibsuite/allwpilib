// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "subsystems/Intake.hpp"

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include "Constants.hpp"
#include "wpi/hardware/pneumatic/DoubleSolenoid.hpp"
#include "wpi/simulation/DoubleSolenoidSim.hpp"
#include "wpi/simulation/PWMMotorControllerSim.hpp"

class IntakeTest {
 public:
  Intake intake;  // create our intake
  wpi::sim::PWMMotorControllerSim simMotor{
      IntakeConstants::MOTOR_PORT};  // create our simulation PWM
  wpi::sim::DoubleSolenoidSim simPiston{
      wpi::PneumaticsModuleType::CTRE_PCM, IntakeConstants::PISTON_FWD_CHANNEL,
      IntakeConstants::PISTON_REV_CHANNEL};  // create our simulation solenoid
};

TEST_CASE_METHOD(IntakeTest, "IntakeTest doesnt work when closed",
                 "[wpilibcExamples][examples][unitTest]") {
  intake.Retract();      // close the intake
  intake.Activate(0.5);  // try to activate the motor
  CHECK_THAT(simMotor.GetThrottle(),
             Catch::Matchers::WithinULP(0.0, 4));  // make sure that the value
                                                   // set to the motor is 0
}

TEST_CASE_METHOD(IntakeTest, "IntakeTest works when open",
                 "[wpilibcExamples][examples][unitTest]") {
  intake.Deploy();
  intake.Activate(0.5);
  CHECK_THAT(simMotor.GetThrottle(), Catch::Matchers::WithinULP(0.5, 4));
}

TEST_CASE_METHOD(IntakeTest, "IntakeTest retract",
                 "[wpilibcExamples][examples][unitTest]") {
  intake.Retract();
  CHECK(wpi::DoubleSolenoid::Value::REVERSE == simPiston.Get());
}

TEST_CASE_METHOD(IntakeTest, "IntakeTest deploy",
                 "[wpilibcExamples][examples][unitTest]") {
  intake.Deploy();
  CHECK(wpi::DoubleSolenoid::Value::FORWARD == simPiston.Get());
}
