/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.examples.gearsbot.subsystems;

import edu.wpi.first.wpilibj.AnalogPotentiometer;
import edu.wpi.first.wpilibj.Victor;
import edu.wpi.first.wpilibj.controller.PIDController;
import edu.wpi.first.wpilibj.smartdashboard.SmartDashboard;
import edu.wpi.first.wpilibj2.command.PIDSubsystem;

import edu.wpi.first.wpilibj.examples.gearsbot.Robot;

/**
 * The wrist subsystem is like the elevator, but with a rotational joint instead of a linear joint.
 */
public class Wrist extends PIDSubsystem {
  private final Victor m_motor;
  private final AnalogPotentiometer m_pot;

  private static final double kP_real = 1;
  private static final double kP_simulation = 0.05;

  /**
   * Create a new wrist subsystem.
   */
  public Wrist() {
    super(new PIDController(kP_real, 0, 0));
    if (Robot.isSimulation()) { // Check for simulation and update PID values
      getController().setPID(kP_simulation, 0, 0);
    }
    getController().setTolerance(2.5);

    m_motor = new Victor(6);

    // Conversion value of potentiometer varies between the real world and
    // simulation
    if (Robot.isReal()) {
      m_pot = new AnalogPotentiometer(3, -270.0 / 5);
    } else {
      m_pot = new AnalogPotentiometer(3); // Defaults to degrees
    }

    // Let's name everything on the LiveWindow
    addChild("Motor", m_motor);
    addChild("Pot", m_pot);
  }

  /**
   * The log method puts interesting information to the SmartDashboard.
   */
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

  /**
   * Use the motor as the PID output. This method is automatically called by the subsystem.
   */
  @Override
  public void useOutput(double output, double setpoint) {
    m_motor.set(output);
  }

  /**
   * Call log method every loop.
   */
  @Override
  public void periodic() {
    log();
  }
}
