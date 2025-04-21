// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.examples.elevatorexponentialprofile;

import edu.wpi.first.math.controller.SimpleMotorFeedforward;
import edu.wpi.first.math.trajectory.ExponentialProfile;
import edu.wpi.first.wpilibj.Joystick;
import edu.wpi.first.wpilibj.TimedRobot;

public class Robot extends TimedRobot {
  private static double kDt = 0.02;

  private final Joystick m_joystick = new Joystick(1);
  private final ExampleSmartMotorController m_motor = new ExampleSmartMotorController(1);
  // Note: These gains are fake, and will have to be tuned for your robot.
  private final SimpleMotorFeedforward m_feedforward = new SimpleMotorFeedforward(1, 1, 1);

  // Create a motion profile with the given maximum voltage and characteristics kV, kA
  // These gains should match your feedforward kV, kA for best results.
  private final ExponentialProfile m_profile =
      new ExponentialProfile(ExponentialProfile.Constraints.fromCharacteristics(10, 1, 1));
  private ExponentialProfile.State m_goal = new ExponentialProfile.State(0, 0);
  private ExponentialProfile.State m_setpoint = new ExponentialProfile.State(0, 0);

  public Robot() {
    // Note: These gains are fake, and will have to be tuned for your robot.
    m_motor.setPID(1.3, 0.0, 0.7);
  }

  @Override
  public void teleopPeriodic() {
    if (m_joystick.getRawButtonPressed(2)) {
      m_goal = new ExponentialProfile.State(5, 0);
    } else if (m_joystick.getRawButtonPressed(3)) {
      m_goal = new ExponentialProfile.State(0, 0);
    }

    // Retrieve the profiled setpoint for the next timestep. This setpoint moves
    // toward the goal while obeying the constraints.
    ExponentialProfile.State next = m_profile.calculate(kDt, m_setpoint, m_goal);

    // Send setpoint to offboard controller PID
    m_motor.setSetpoint(
        ExampleSmartMotorController.PIDMode.kPosition,
        m_setpoint.position,
        m_feedforward.calculate(next.velocity) / 12.0);

    m_setpoint = next;
  }
}
