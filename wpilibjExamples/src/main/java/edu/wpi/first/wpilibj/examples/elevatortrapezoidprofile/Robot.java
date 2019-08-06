/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.examples.elevatortrapezoidprofile;

import edu.wpi.first.wpilibj.Encoder;
import edu.wpi.first.wpilibj.Joystick;
import edu.wpi.first.wpilibj.Spark;
import edu.wpi.first.wpilibj.TimedRobot;
import edu.wpi.first.wpilibj.controller.PIDController;
import edu.wpi.first.wpilibj.trajectory.TrapezoidProfile;

public class Robot extends TimedRobot {
  Joystick m_joystick = new Joystick(1);
  Encoder m_encoder = new Encoder(1, 2);
  Spark m_motor = new Spark(1);
  PIDController m_controller = new PIDController(1.3, 0.0, 0.7, 0.05);

  TrapezoidProfile.Constraints m_constraints = new TrapezoidProfile.Constraints(1.75, 0.75);
  TrapezoidProfile.State m_goal = new TrapezoidProfile.State();
  TrapezoidProfile.State m_setpoint = new TrapezoidProfile.State();

  @Override
  public void robotInit() {
    m_encoder.setDistancePerPulse(1.0 / 360.0 * 2.0 * 3.1415 * 1.5);
  }

  @Override
  public void teleopPeriodic() {
    if (m_joystick.getRawButtonPressed(2)) {
      m_goal = new TrapezoidProfile.State(5, 0);
    } else if (m_joystick.getRawButtonPressed(3)) {
      m_goal = new TrapezoidProfile.State(0, 0);
    }

    var profile = new TrapezoidProfile(m_constraints, m_goal, m_setpoint);
    m_setpoint = profile.calculate(0.05);

    double output = m_controller.calculate(m_encoder.getDistance(),
                                           m_setpoint.position);
    m_motor.set(output);
  }
}
