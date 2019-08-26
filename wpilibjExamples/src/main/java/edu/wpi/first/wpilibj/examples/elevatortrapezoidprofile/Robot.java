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
  private static double kDt = 0.02;

  private final Joystick m_joystick = new Joystick(1);
  private final Encoder m_encoder = new Encoder(1, 2);
  private final Spark m_motor = new Spark(1);
  private final PIDController m_controller = new PIDController(1.3, 0.0, 0.7, kDt);

  private final TrapezoidProfile.Constraints m_constraints =
      new TrapezoidProfile.Constraints(1.75, 0.75);
  private TrapezoidProfile.State m_goal = new TrapezoidProfile.State();
  private TrapezoidProfile.State m_setpoint = new TrapezoidProfile.State();

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

    // Create a motion profile with the given maximum velocity and maximum
    // acceleration constraints for the next setpoint, the desired goal, and the
    // current setpoint.
    var profile = new TrapezoidProfile(m_constraints, m_goal, m_setpoint);

    // Retrieve the profiled setpoint for the next timestep. This setpoint moves
    // toward the goal while obeying the constraints.
    m_setpoint = profile.calculate(kDt);

    double output = m_controller.calculate(m_encoder.getDistance(),
                                           m_setpoint.position);

    // Run controller with profiled setpoint and update motor output
    m_motor.set(output);
  }
}
