// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.snippets.profiledpidfeedforward;

import org.wpilib.framework.TimedRobot;
import org.wpilib.hardware.motor.PWMSparkMax;
import org.wpilib.hardware.rotation.Encoder;
import org.wpilib.math.controller.ProfiledPIDController;
import org.wpilib.math.controller.SimpleMotorFeedforward;
import org.wpilib.math.trajectory.TrapezoidProfile;

/**
 * ProfiledPIDController with feedforward snippets for frc-docs.
 * https://docs.wpilib.org/en/stable/docs/software/advanced-controls/controllers/profiled-pidcontroller.html
 */
public class Robot extends TimedRobot {
  private final ProfiledPIDController controller =
      new ProfiledPIDController(1.0, 0.0, 0.0, new TrapezoidProfile.Constraints(5.0, 10.0));
  private final SimpleMotorFeedforward feedforward = new SimpleMotorFeedforward(0.5, 1.5, 0.3);
  private final Encoder encoder = new Encoder(0, 1);
  private final PWMSparkMax motor = new PWMSparkMax(0);

  double lastVelocity;

  /** Called once at the beginning of the robot program. */
  public Robot() {
    encoder.setDistancePerPulse(1.0 / 256.0);
  }

  /**
   * Controls a simple motor's position using a SimpleMotorFeedforward and a ProfiledPIDController.
   *
   * @param goalPosition the desired position
   */
  public void goToPosition(double goalPosition) {
    double pidVal = controller.calculate(encoder.getDistance(), goalPosition);
    motor.setVoltage(
        pidVal + feedforward.calculate(lastVelocity, controller.getSetpoint().velocity));
    lastVelocity = controller.getSetpoint().velocity;
  }

  @Override
  public void teleopPeriodic() {
    // Example usage: move to position 10.0
    goToPosition(10.0);
  }
}
