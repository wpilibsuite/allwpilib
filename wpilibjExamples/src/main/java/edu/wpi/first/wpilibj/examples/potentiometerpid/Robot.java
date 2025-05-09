// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.examples.potentiometerpid;

import edu.wpi.first.math.controller.PIDController;
import edu.wpi.first.wpilibj.AnalogPotentiometer;
import edu.wpi.first.wpilibj.Joystick;
import edu.wpi.first.wpilibj.TimedRobot;
import edu.wpi.first.wpilibj.motorcontrol.PWMSparkMax;

/**
 * This is a sample program to demonstrate how to use a soft potentiometer and a PID controller to
 * reach and maintain position setpoints on an elevator mechanism.
 */
public class Robot extends TimedRobot {
  static final int POT_CHANNEL = 1;
  static final int MOTOR_CHANNEL = 7;
  static final int JOYSTICK_CHANNEL = 3;

  // The elevator can move 1.5 meters from top to bottom
  static final double FULL_HEIGHT = 1.5;

  // Bottom, middle, and top elevator setpoints in meters
  static final double[] SETPOINTS = {0.2, 0.8, 1.4};

  // proportional, integral, and derivative speed constants
  // DANGER: when tuning PID constants, high/inappropriate values for kP, kI,
  // and kD may cause dangerous, uncontrollable, or undesired behavior!
  private static final double kP = 0.7;
  private static final double kI = 0.35;
  private static final double kD = 0.25;

  private final PIDController m_pidController = new PIDController(kP, kI, kD);
  // Scaling is handled internally
  private final AnalogPotentiometer m_potentiometer =
      new AnalogPotentiometer(POT_CHANNEL, FULL_HEIGHT);
  private final PWMSparkMax m_elevatorMotor = new PWMSparkMax(MOTOR_CHANNEL);
  private final Joystick m_joystick = new Joystick(JOYSTICK_CHANNEL);

  private int m_index;

  @Override
  public void teleopInit() {
    // Move to the bottom setpoint when teleop starts
    m_index = 0;
    m_pidController.setSetpoint(SETPOINTS[m_index]);
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
      m_index = (m_index + 1) % SETPOINTS.length;
      System.out.println("m_index = " + m_index);
      m_pidController.setSetpoint(SETPOINTS[m_index]);
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
