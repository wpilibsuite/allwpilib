// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.examples.unittest.subsystems;

import static org.junit.jupiter.api.Assertions.assertEquals;

import org.junit.jupiter.api.AfterEach;
import org.junit.jupiter.api.BeforeEach;
import org.junit.jupiter.api.Test;
import org.wpilib.examples.unittest.Constants.IntakeConstants;
import org.wpilib.hardware.hal.HAL;
import org.wpilib.hardware.pneumatic.DoubleSolenoid;
import org.wpilib.hardware.pneumatic.PneumaticsModuleType;
import org.wpilib.simulation.DoubleSolenoidSim;
import org.wpilib.simulation.PWMMotorControllerSim;

class IntakeTest {
  static final double DELTA = 1e-2; // acceptable deviation range
  Intake intake;
  PWMMotorControllerSim simMotor;
  DoubleSolenoidSim simPiston;

  @BeforeEach // this method will run before each test
  void setup() {
    assert HAL.initialize(500, 0); // initialize the HAL, crash if failed
    intake = new Intake(); // create our intake
    simMotor =
        new PWMMotorControllerSim(
            IntakeConstants.kMotorPort); // create our simulation PWM motor controller
    simPiston =
        new DoubleSolenoidSim(
            PneumaticsModuleType.CTRE_PCM,
            IntakeConstants.kPistonFwdChannel,
            IntakeConstants.kPistonRevChannel); // create our simulation solenoid
  }

  @SuppressWarnings("PMD.SignatureDeclareThrowsException")
  @AfterEach // this method will run after each test
  void shutdown() throws Exception {
    intake.close(); // destroy our intake object
  }

  @Test // marks this method as a test
  void doesntWorkWhenClosed() {
    intake.retract(); // close the intake
    intake.activate(0.5); // try to activate the motor
    assertEquals(
        0.0, simMotor.getThrottle(), DELTA); // make sure that the value set to the motor is 0
  }

  @Test
  void worksWhenOpen() {
    intake.deploy();
    intake.activate(0.5);
    assertEquals(0.5, simMotor.getThrottle(), DELTA);
  }

  @Test
  void retractTest() {
    intake.retract();
    assertEquals(DoubleSolenoid.Value.REVERSE, simPiston.get());
  }

  @Test
  void deployTest() {
    intake.deploy();
    assertEquals(DoubleSolenoid.Value.FORWARD, simPiston.get());
  }
}
