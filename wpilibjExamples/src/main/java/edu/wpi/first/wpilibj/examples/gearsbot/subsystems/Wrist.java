// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.examples.gearsbot.subsystems;

import static edu.wpi.first.wpilibj.examples.gearsbot.Constants.WristConstants.*;

import edu.wpi.first.math.controller.PIDController;
import edu.wpi.first.wpilibj.AnalogPotentiometer;
import edu.wpi.first.wpilibj.examples.gearsbot.Robot;
import edu.wpi.first.wpilibj.motorcontrol.Victor;
import edu.wpi.first.wpilibj.smartdashboard.SmartDashboard;
import edu.wpi.first.wpilibj2.command.Command;
import edu.wpi.first.wpilibj2.command.SubsystemBase;
import java.util.function.DoubleSupplier;

/**
 * The wrist subsystem is like the elevator, but with a rotational joint instead of a linear joint.
 */
public class Wrist extends SubsystemBase {
  private final Victor m_motor = new Victor(kMotorPort);
  private final AnalogPotentiometer m_pot;
  private final PIDController m_controller;

  /** Create a new wrist subsystem. */
  public Wrist() {
    m_controller = new PIDController(kP, kI, kD);
    m_controller.setTolerance(kTolerance);

    // Conversion value of potentiometer varies between the real world and
    // simulation
    if (Robot.isReal()) {
      m_pot = new AnalogPotentiometer(kPotentiometerPort, -270.0 / 5);
    } else {
      // Defaults to degrees
      m_pot = new AnalogPotentiometer(kPotentiometerPort);
    }

    // Let's name everything on the LiveWindow
    addChild("Motor", m_motor);
    addChild("Pot", m_pot);

    // Configure default command to hold the wrist steady
    setDefaultCommand(goTo(m_controller::getSetpoint).withName("holding angle"));
  }

  /**
   * Use the potentiometer as the PID sensor. This method is automatically called by the subsystem.
   */
  public double getAngle() {
    return m_pot.get();
  }

  public Command goTo(DoubleSupplier angle) {
    return run(() -> m_motor.set(m_controller.calculate(getAngle(), angle.getAsDouble())))
        .until(m_controller::atSetpoint)
        .withName("wrist to angle")
        .asProxy();
  }

  /** The log method puts interesting information to the SmartDashboard. */
  public void log() {
    SmartDashboard.putData("Wrist Angle", m_pot);
  }

  /** Call log method every loop. */
  @Override
  public void periodic() {
    log();
  }
}
