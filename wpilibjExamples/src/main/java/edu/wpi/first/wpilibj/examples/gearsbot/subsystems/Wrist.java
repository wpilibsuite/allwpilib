// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.examples.gearsbot.subsystems;

import edu.wpi.first.math.controller.PIDController;
import edu.wpi.first.wpilibj.AnalogPotentiometer;
import edu.wpi.first.wpilibj.examples.gearsbot.Constants.WristConstants;
import edu.wpi.first.wpilibj.examples.gearsbot.Robot;
import edu.wpi.first.wpilibj.motorcontrol.Victor;
import edu.wpi.first.wpilibj.smartdashboard.SmartDashboard;
import edu.wpi.first.wpilibj2.command.PIDSubsystem;

/**
 * The wrist subsystem is like the elevator, but with a rotational joint instead of a linear joint.
 */
public class Wrist extends PIDSubsystem {
  private final Victor m_motor = new Victor(WristConstants.kMotorPort);
  private final AnalogPotentiometer m_pot;

  /** Create a new wrist subsystem. */
  public Wrist() {
    super(new PIDController(WristConstants.kP, WristConstants.kI, WristConstants.kD));
    getController().setTolerance(WristConstants.kTolerance);

    // Conversion value of potentiometer varies between the real world and
    // simulation
    if (Robot.isReal()) {
      m_pot = new AnalogPotentiometer(WristConstants.kPotentiometerPort, -270.0 / 5);
    } else {
      // Defaults to degrees
      m_pot = new AnalogPotentiometer(WristConstants.kPotentiometerPort);
    }

    // Let's name everything on the LiveWindow
    addChild("Motor", m_motor);
    addChild("Pot", m_pot);
  }

  /** The log method puts interesting information to the SmartDashboard. */
  public void log() {
    SmartDashboard.putData("Wrist Angle", m_pot);
  }

  /**
   * Use the potentiometer as the PID sensor. This method is automatically called by the subsystem.
   */
  @Override
  public double getMeasurement() {
    return m_pot.get();
  }

  /** Use the motor as the PID output. This method is automatically called by the subsystem. */
  @Override
  public void useOutput(double output, double setpoint) {
    m_motor.set(output);
  }

  /** Call log method every loop. */
  @Override
  public void periodic() {
    log();
  }
}
