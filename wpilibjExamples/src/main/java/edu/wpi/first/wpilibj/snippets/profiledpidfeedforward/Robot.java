// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.snippets.profiledpidfeedforward;

import edu.wpi.first.math.controller.ProfiledPIDController;
import edu.wpi.first.math.controller.SimpleMotorFeedforward;
import edu.wpi.first.math.trajectory.TrapezoidProfile;
import edu.wpi.first.wpilibj.Encoder;
import edu.wpi.first.wpilibj.TimedRobot;
import edu.wpi.first.wpilibj.motorcontrol.PWMSparkMax;

/**
 * ProfiledPIDController with feedforward snippets for frc-docs.
 * https://docs.wpilib.org/en/stable/docs/software/advanced-controls/controllers/profiled-pidcontroller.html
 */
public class Robot extends TimedRobot {
  private final ProfiledPIDController m_controller =
      new ProfiledPIDController(1.0, 0.0, 0.0, new TrapezoidProfile.Constraints(5.0, 10.0));
  private final SimpleMotorFeedforward m_feedforward = new SimpleMotorFeedforward(0.5, 1.5, 0.3);
  private final Encoder m_encoder = new Encoder(0, 1);
  private final PWMSparkMax m_motor = new PWMSparkMax(0);

  double m_lastSpeed = 0;

  /** Called once at the beginning of the robot program. */
  public Robot() {
    m_encoder.setDistancePerPulse(1.0 / 256.0);
  }

  // Controls a simple motor's position using a SimpleMotorFeedforward
  // and a ProfiledPIDController
  public void goToPosition(double goalPosition) {
    double pidVal = m_controller.calculate(m_encoder.getDistance(), goalPosition);
    m_motor.setVoltage(
        pidVal + m_feedforward.calculate(m_lastSpeed, m_controller.getSetpoint().velocity));
    m_lastSpeed = m_controller.getSetpoint().velocity;
  }

  @Override
  public void teleopPeriodic() {
    // Example usage: move to position 10.0
    goToPosition(10.0);
  }
}
