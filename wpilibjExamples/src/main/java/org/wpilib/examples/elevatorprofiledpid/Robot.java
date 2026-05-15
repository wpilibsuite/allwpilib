// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.examples.elevatorprofiledpid;

import org.wpilib.driverstation.Joystick;
import org.wpilib.framework.TimedRobot;
import org.wpilib.hardware.motor.PWMSparkMax;
import org.wpilib.hardware.rotation.Encoder;
import org.wpilib.math.controller.ElevatorFeedforward;
import org.wpilib.math.controller.ProfiledPIDController;
import org.wpilib.math.trajectory.TrapezoidProfile;

public class Robot extends TimedRobot {
  private static double kDt = 0.02;
  private static double kMaxVelocity = 1.75;
  private static double kMaxAcceleration = 0.75;
  private static double kP = 1.3;
  private static double kI = 0.0;
  private static double kD = 0.7;
  private static double kS = 1.1;
  private static double kG = 1.2;
  private static double kV = 1.3;

  private final Joystick joystick = new Joystick(1);
  private final Encoder encoder = new Encoder(1, 2);
  private final PWMSparkMax motor = new PWMSparkMax(1);

  // Create a PID controller whose setpoint's change is subject to maximum
  // velocity and acceleration constraints.
  private final TrapezoidProfile.Constraints constraints =
      new TrapezoidProfile.Constraints(kMaxVelocity, kMaxAcceleration);
  private final ProfiledPIDController controller =
      new ProfiledPIDController(kP, kI, kD, constraints, kDt);
  private final ElevatorFeedforward feedforward = new ElevatorFeedforward(kS, kG, kV);

  public Robot() {
    encoder.setDistancePerPulse(1.0 / 360.0 * 2.0 * Math.PI * 1.5);
  }

  @Override
  public void teleopPeriodic() {
    if (joystick.getRawButtonPressed(2)) {
      controller.setGoal(5);
    } else if (joystick.getRawButtonPressed(3)) {
      controller.setGoal(0);
    }

    // Run controller and update motor output
    motor.setVoltage(
        controller.calculate(encoder.getDistance())
            + feedforward.calculate(controller.getSetpoint().velocity));
  }
}
