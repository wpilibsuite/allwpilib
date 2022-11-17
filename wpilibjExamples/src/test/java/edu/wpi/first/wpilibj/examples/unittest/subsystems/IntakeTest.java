// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.examples.unittest.subsystems;

import static org.junit.jupiter.api.Assertions.assertEquals;

import edu.wpi.first.hal.HAL;
import edu.wpi.first.wpilibj.DoubleSolenoid;
import edu.wpi.first.wpilibj.PneumaticsModuleType;
import edu.wpi.first.wpilibj.examples.unittest.Constants.IntakeConstants;
import edu.wpi.first.wpilibj.simulation.DoubleSolenoidSim;
import edu.wpi.first.wpilibj.simulation.PWMSim;
import org.junit.jupiter.api.AfterEach;
import org.junit.jupiter.api.BeforeEach;
import org.junit.jupiter.api.Test;

public class IntakeTest {
  public static final double DELTA = 1e-2; // acceptable deviation range
  Intake intake;
  PWMSim simMotor;
  DoubleSolenoidSim simPiston;

  @BeforeEach // this method will run before each test
  public void setup() {
    assert HAL.initialize(500, 0); // initialize the HAL, crash if failed
    intake = new Intake(); // create our intake
    simMotor = new PWMSim(IntakeConstants.kMotorPort); // create our simulation PWM motor controller
    simPiston =
        new DoubleSolenoidSim(
            PneumaticsModuleType.CTREPCM,
            IntakeConstants.kPistonFwdChannel,
            IntakeConstants.kPistonRevChannel); // create our simulation solenoid
  }

  @AfterEach // this method will run after each test
  public void shutdown() throws Exception {
    intake.close(); // destroy our intake object
  }

  @Test // marks this method as a test
  public void doesntWorkWhenClosed() {
    intake.retract(); // close the intake
    intake.activate(0.5); // try to activate the motor
    assertEquals(0.0, simMotor.getSpeed(), DELTA); // make sure that the value set to the motor is 0
  }

  @Test
  public void worksWhenOpen() {
    intake.deploy();
    intake.activate(0.5);
    assertEquals(0.5, simMotor.getSpeed(), DELTA);
  }

  @Test
  public void retractTest() {
    intake.retract();
    assertEquals(DoubleSolenoid.Value.kReverse, simPiston.get());
  }

  @Test
  public void deployTest() {
    intake.deploy();
    assertEquals(DoubleSolenoid.Value.kForward, simPiston.get());
  }
}
