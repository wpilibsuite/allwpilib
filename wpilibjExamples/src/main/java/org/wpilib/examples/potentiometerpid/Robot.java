// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.examples.potentiometerpid;

import org.wpilib.driverstation.Joystick;
import org.wpilib.framework.TimedRobot;
import org.wpilib.hardware.motor.PWMSparkMax;
import org.wpilib.hardware.rotation.AnalogPotentiometer;
import org.wpilib.math.controller.PIDController;

/**
 * This is a sample program to demonstrate how to use a soft potentiometer and a PID controller to
 * reach and maintain position setpoints on an elevator mechanism.
 */
public class Robot extends TimedRobot {
  static final int kPotChannel = 1;
  static final int kMotorChannel = 7;
  static final int kJoystickChannel = 3;

  // The elevator can move 1.5 meters from top to bottom
  static final double kFullHeight = 1.5;

  // Bottom, middle, and top elevator setpoints in meters
  static final double[] kSetpoints = {0.2, 0.8, 1.4};

  // proportional, integral, and derivative speed constants
  // DANGER: when tuning PID constants, high/inappropriate values for kP, kI,
  // and kD may cause dangerous, uncontrollable, or undesired behavior!
  private static final double kP = 0.7;
  private static final double kI = 0.35;
  private static final double kD = 0.25;

  private final PIDController m_pidController = new PIDController(kP, kI, kD);
  // Scaling is handled internally
  private final AnalogPotentiometer m_potentiometer =
      new AnalogPotentiometer(kPotChannel, kFullHeight);
  private final PWMSparkMax m_elevatorMotor = new PWMSparkMax(kMotorChannel);
  private final Joystick m_joystick = new Joystick(kJoystickChannel);

  private int m_index;

  @Override
  public void teleopInit() {
    // Move to the bottom setpoint when teleop starts
    m_index = 0;
    m_pidController.setSetpoint(kSetpoints[m_index]);
  }

  @Override
  public void teleopPeriodic() {
    // Read from the sensor
    double position = m_potentiometer.get();

    // Run the PID Controller
    double pidOut = m_pidController.calculate(position);

    // Apply PID output
    m_elevatorMotor.set(pidOut);

    // when the button is pressed once, the selected elevator setpoint is incremented
    if (m_joystick.getTriggerPressed()) {
      // index of the elevator setpoint wraps around.
      m_index = (m_index + 1) % kSetpoints.length;
      System.out.println("m_index = " + m_index);
      m_pidController.setSetpoint(kSetpoints[m_index]);
    }
  }

  @Override
  public void close() {
    m_elevatorMotor.close();
    m_potentiometer.close();
    m_pidController.close();
    m_index = 0;
    super.close();
  }
}
