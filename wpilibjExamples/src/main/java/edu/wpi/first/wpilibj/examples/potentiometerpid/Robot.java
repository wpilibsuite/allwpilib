/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.examples.potentiometerpid;

import edu.wpi.first.wpilibj.AnalogInput;
import edu.wpi.first.wpilibj.Joystick;
import edu.wpi.first.wpilibj.PWMVictorSPX;
import edu.wpi.first.wpilibj.SpeedController;
import edu.wpi.first.wpilibj.TimedRobot;
import edu.wpi.first.wpilibj.experimental.controller.ControllerRunner;
import edu.wpi.first.wpilibj.experimental.controller.PIDController;

/**
 * This is a sample program to demonstrate how to use a soft potentiometer and a
 * PID controller to reach and maintain position setpoints on an elevator
 * mechanism.
 */
public class Robot extends TimedRobot {
  private static final int kPotChannel = 1;
  private static final int kMotorChannel = 7;
  private static final int kJoystickChannel = 0;

  // bottom, middle, and top elevator setpoints
  private static final double[] kReferences = {1.0, 2.6, 4.3};

  // proportional, integral, and derivative speed constants; motor inverted
  // DANGER: when tuning PID constants, high/inappropriate values for kP, kI,
  // and kD may cause dangerous, uncontrollable, or undesired behavior!
  // these may need to be positive for a non-inverted motor
  private static final double kP = -5.0;
  private static final double kI = -0.02;
  private static final double kD = -2.0;

  private PIDController m_pidController;
  private ControllerRunner m_pidRunner;
  @SuppressWarnings("PMD.SingularField")
  private AnalogInput m_potentiometer;
  @SuppressWarnings("PMD.SingularField")
  private SpeedController m_elevatorMotor;
  private Joystick m_joystick;

  private int m_index;
  private boolean m_previousButtonValue;

  @Override
  public void robotInit() {
    m_potentiometer = new AnalogInput(kPotChannel);
    m_elevatorMotor = new PWMVictorSPX(kMotorChannel);
    m_joystick = new Joystick(kJoystickChannel);

    m_pidController = new PIDController(kP, kI, kD, m_potentiometer::getAverageVoltage);
    m_pidController.setInputRange(0, 5);
    m_pidRunner = new ControllerRunner(m_pidController, m_elevatorMotor::set);
  }

  @Override
  public void teleopInit() {
    m_pidRunner.enable();
  }

  @Override
  public void teleopPeriodic() {
    // when the button is pressed once, the selected elevator setpoint
    // is incremented
    boolean currentButtonValue = m_joystick.getTrigger();
    if (currentButtonValue && !m_previousButtonValue) {
      // index of the elevator reference wraps around.
      m_index = (m_index + 1) % kReferences.length;
    }
    m_previousButtonValue = currentButtonValue;

    m_pidController.setReference(kReferences[m_index]);
  }
}
