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
import edu.wpi.first.wpilibj.command.PIDSubsystem;
import edu.wpi.first.wpilibj.interfaces.Potentiometer;

import edu.wpi.first.wpilibj.examples.pacgoat.Robot;

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
  private final DigitalInput m_upperLimitSwitch = new DigitalInput(13);
  private final DigitalInput m_lowerLimitSwitch = new DigitalInput(12);

  // 0 degrees is vertical facing up.
  // Angle increases the more forward the pivot goes.
  private final Potentiometer m_pot = new AnalogPotentiometer(1);

  // Motor to move the pivot.
  private final SpeedController m_motor = new Victor(5);

  /**
   * Create a new pivot subsystem.
   */
  public Pivot() {
    super("Pivot", 7.0, 0.0, 8.0);
    setAbsoluteTolerance(0.005);
    getPIDController().setContinuous(false);
    if (Robot.isSimulation()) { // PID is different in simulation.
      getPIDController().setPID(0.5, 0.001, 2);
      setAbsoluteTolerance(5);
    }

    // Put everything to the LiveWindow for testing.
    addChild("Upper Limit Switch", m_upperLimitSwitch);
    addChild("Lower Limit Switch", m_lowerLimitSwitch);
    addChild("Pot", (AnalogPotentiometer) m_pot);
    addChild("Motor", (Victor) m_motor);
    addChild("PIDSubsystem Controller", getPIDController());
  }

  /**
   * No default command, if PID is enabled, the current setpoint will be
   * maintained.
   */
  @Override
  public void initDefaultCommand() {
  }

  /**
   * The angle read in by the potentiometer.
   */
  @Override
  protected double returnPIDInput() {
    return m_pot.get();
  }

  /**
   * Set the motor speed based off of the PID output.
   */
  @Override
  protected void usePIDOutput(double output) {
    m_motor.pidWrite(output);
  }

  /**
   * If the pivot is at its upper limit.
   */
  public boolean isAtUpperLimit() {
    // TODO: inverted from real robot (prefix with !)
    return m_upperLimitSwitch.get();
  }

  /**
   * If the pivot is at its lower limit.
   */
  public boolean isAtLowerLimit() {
    // TODO: inverted from real robot (prefix with !)
    return m_lowerLimitSwitch.get();
  }

  /**
   * The current angle of the pivot.
   */
  public double getAngle() {
    return m_pot.get();
  }
}
