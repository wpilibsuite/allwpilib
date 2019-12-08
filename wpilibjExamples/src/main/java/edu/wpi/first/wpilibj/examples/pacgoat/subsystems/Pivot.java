/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.examples.pacgoat.subsystems;

import edu.wpi.first.wpilibj.AnalogPotentiometer;
import edu.wpi.first.wpilibj.DigitalInput;
import edu.wpi.first.wpilibj.SpeedController;
import edu.wpi.first.wpilibj.Victor;
import edu.wpi.first.wpilibj.controller.PIDController;
import edu.wpi.first.wpilibj.interfaces.Potentiometer;
import edu.wpi.first.wpilibj2.command.PIDSubsystem;

import edu.wpi.first.wpilibj.examples.pacgoat.Robot;

import static edu.wpi.first.wpilibj.examples.pacgoat.Constants.PivotConstants.kDRealPivotController;
import static edu.wpi.first.wpilibj.examples.pacgoat.Constants.PivotConstants.kDSimPivotController;
import static edu.wpi.first.wpilibj.examples.pacgoat.Constants.PivotConstants.kIRealPivotController;
import static edu.wpi.first.wpilibj.examples.pacgoat.Constants.PivotConstants.kISimPivotController;
import static edu.wpi.first.wpilibj.examples.pacgoat.Constants.PivotConstants.kLowerLimitSwitchPort;
import static edu.wpi.first.wpilibj.examples.pacgoat.Constants.PivotConstants.kPRealPivotController;
import static edu.wpi.first.wpilibj.examples.pacgoat.Constants.PivotConstants.kPSimPivotController;
import static edu.wpi.first.wpilibj.examples.pacgoat.Constants.PivotConstants.kPivotMotorPort;
import static edu.wpi.first.wpilibj.examples.pacgoat.Constants.PivotConstants.kPivotPotPort;
import static edu.wpi.first.wpilibj.examples.pacgoat.Constants.PivotConstants.kToleranceRealPivotController;
import static edu.wpi.first.wpilibj.examples.pacgoat.Constants.PivotConstants.kToleranceSimPivotController;
import static edu.wpi.first.wpilibj.examples.pacgoat.Constants.PivotConstants.kUpperLimitSwitchPort;

/**
 * The Pivot subsystem contains the Van-door motor and the pot for PID control
 * of angle of the pivot and claw.
 */
public class Pivot extends PIDSubsystem {
  // Constants for some useful angles
  public static final double kCollect = 105;
  public static final double kLowGoal = 90;
  public static final double kShoot = 45;
  public static final double kShootNear = 30;

  // Sensors for measuring the position of the pivot.
  private final DigitalInput m_upperLimitSwitch = new DigitalInput(kUpperLimitSwitchPort);
  private final DigitalInput m_lowerLimitSwitch = new DigitalInput(kLowerLimitSwitchPort);

  // 0 degrees is vertical facing up.
  // Angle increases the more forward the pivot goes.
  private final Potentiometer m_pot = new AnalogPotentiometer(kPivotPotPort);

  // Motor to move the pivot.
  private final SpeedController m_motor = new Victor(kPivotMotorPort);

  /**
   * Create a new pivot subsystem.
   */
  public Pivot() {
    super(
        new PIDController(kPRealPivotController, kIRealPivotController, kDRealPivotController));
    getController().setTolerance(kToleranceRealPivotController);
    getController().disableContinuousInput();
    if (Robot.isSimulation()) { // PID is different in simulation.
      getController().setPID(kPSimPivotController, kISimPivotController, kDSimPivotController);
      getController().setTolerance(kToleranceSimPivotController);
    }

    // Put everything to the LiveWindow for testing.
    addChild("Upper Limit Switch", m_upperLimitSwitch);
    addChild("Lower Limit Switch", m_lowerLimitSwitch);
    addChild("Pot", (AnalogPotentiometer) m_pot);
    addChild("Motor", (Victor) m_motor);
    addChild("PIDSubsystem Controller", getController());
  }

  /**
   * The angle read in by the potentiometer.
   */
  @Override
  protected double getMeasurement() {
    return m_pot.get();
  }

  /**
   * Set the motor speed based off of the PID output.
   */
  @Override
  protected void useOutput(double output, double setpoint) {
    m_motor.set(output);
  }

  /**
   * If the pivot is at its upper limit.
   */
  public boolean isAtUpperLimit() {
    if (Robot.isSimulation()) { // Inverted from real robot
      return m_upperLimitSwitch.get();
    } else {
      return !m_upperLimitSwitch.get();
    }
  }

  /**
   * If the pivot is at its lower limit.
   */
  public boolean isAtLowerLimit() {
    if (Robot.isSimulation()) { // Inverted from real robot
      return m_lowerLimitSwitch.get();
    } else {
      return !m_lowerLimitSwitch.get();
    }
  }

  /**
   * The current angle of the pivot.
   */
  public double getAngle() {
    return m_pot.get();
  }
}
