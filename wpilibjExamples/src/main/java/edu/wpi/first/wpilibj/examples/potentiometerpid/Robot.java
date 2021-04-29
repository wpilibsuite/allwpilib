// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.examples.potentiometerpid;

import edu.wpi.first.math.controller.PIDController;
import edu.wpi.first.wpilibj.AnalogInput;
import edu.wpi.first.wpilibj.Joystick;
import edu.wpi.first.wpilibj.TimedRobot;
import edu.wpi.first.wpilibj.motorcontrol.MotorController;
import edu.wpi.first.wpilibj.motorcontrol.PWMSparkMax;

/**
 * This is a sample program to demonstrate how to use a soft potentiometer and a PID controller to
 * reach and maintain position setpoints on an elevator mechanism.
 */
public class Robot extends TimedRobot {
  private static final int kPotChannel = 1;
  private static final int kMotorChannel = 7;
  private static final int kJoystickChannel = 0;

  // bottom, middle, and top elevator setpoints
  private static final double[] kSetPoints = {1.0, 2.6, 4.3};

  // proportional, integral, and derivative speed constants; motor inverted
  // DANGER: when tuning PID constants, high/inappropriate values for kP, kI,
  // and kD may cause dangerous, uncontrollable, or undesired behavior!
  // these may need to be positive for a non-inverted motor
  private static final double kP = -5.0;
  private static final double kI = -0.02;
  private static final double kD = -2.0;

  private PIDController m_pidController;
  private AnalogInput m_potentiometer;
  private MotorController m_elevatorMotor;
  private Joystick m_joystick;

  private int m_index;
  private boolean m_previousButtonValue;

  @Override
  public void robotInit() {
    m_potentiometer = new AnalogInput(kPotChannel);
    m_elevatorMotor = new PWMSparkMax(kMotorChannel);
    m_joystick = new Joystick(kJoystickChannel);

    m_pidController = new PIDController(kP, kI, kD);
    m_pidController.setSetpoint(kSetPoints[m_index]);
  }

  @Override
  public void teleopPeriodic() {
    // Run the PID Controller
    double pidOut = m_pidController.calculate(m_potentiometer.getAverageVoltage());
    m_elevatorMotor.set(pidOut);

    // when the button is pressed once, the selected elevator setpoint
    // is incremented
    boolean currentButtonValue = m_joystick.getTrigger();
    if (currentButtonValue && !m_previousButtonValue) {
      // index of the elevator setpoint wraps around.
      m_index = (m_index + 1) % kSetPoints.length;
      m_pidController.setSetpoint(kSetPoints[m_index]);
    }
    m_previousButtonValue = currentButtonValue;
  }
}
