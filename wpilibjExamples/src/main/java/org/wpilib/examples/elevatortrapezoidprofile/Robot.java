// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.examples.elevatortrapezoidprofile;

import org.wpilib.driverstation.Joystick;
import org.wpilib.framework.TimedRobot;
import org.wpilib.math.controller.SimpleMotorFeedforward;
import org.wpilib.math.trajectory.TrapezoidProfile;

public class Robot extends TimedRobot {
  private static double kDt = 0.02;

  private final Joystick joystick = new Joystick(1);
  private final ExampleSmartMotorController motor = new ExampleSmartMotorController(1);
  // Note: These gains are fake, and will have to be tuned for your robot.
  private final SimpleMotorFeedforward feedforward = new SimpleMotorFeedforward(1, 1.5);

  // Create a motion profile with the given maximum velocity and maximum
  // acceleration constraints for the next setpoint.
  private final TrapezoidProfile profile =
      new TrapezoidProfile(new TrapezoidProfile.Constraints(1.75, 0.75));
  private TrapezoidProfile.State goal = new TrapezoidProfile.State();
  private TrapezoidProfile.State setpoint = new TrapezoidProfile.State();

  public Robot() {
    // Note: These gains are fake, and will have to be tuned for your robot.
    motor.setPID(1.3, 0.0, 0.7);
  }

  @Override
  public void teleopPeriodic() {
    if (joystick.getRawButtonPressed(2)) {
      goal = new TrapezoidProfile.State(5, 0);
    } else if (joystick.getRawButtonPressed(3)) {
      goal = new TrapezoidProfile.State();
    }

    // Retrieve the profiled setpoint for the next timestep. This setpoint moves
    // toward the goal while obeying the constraints.
    setpoint = profile.calculate(kDt, setpoint, goal);

    // Send setpoint to offboard controller PID
    motor.setSetpoint(
        ExampleSmartMotorController.PIDMode.kPosition,
        setpoint.position,
        feedforward.calculate(setpoint.velocity) / 12.0);
  }
}
