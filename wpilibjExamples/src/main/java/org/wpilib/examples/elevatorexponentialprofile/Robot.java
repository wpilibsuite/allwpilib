// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.examples.elevatorexponentialprofile;

import org.wpilib.driverstation.Joystick;
import org.wpilib.framework.TimedRobot;
import org.wpilib.math.controller.SimpleMotorFeedforward;
import org.wpilib.math.trajectory.ExponentialProfile;

public class Robot extends TimedRobot {
  private static double kDt = 0.02;

  private final Joystick joystick = new Joystick(1);
  private final ExampleSmartMotorController motor = new ExampleSmartMotorController(1);
  // Note: These gains are fake, and will have to be tuned for your robot.
  private final SimpleMotorFeedforward feedforward = new SimpleMotorFeedforward(1, 1, 1);

  // Create a motion profile with the given maximum voltage and characteristics kV, kA
  // These gains should match your feedforward kV, kA for best results.
  private final ExponentialProfile profile =
      new ExponentialProfile(ExponentialProfile.Constraints.fromCharacteristics(10, 1, 1));
  private ExponentialProfile.State goal = new ExponentialProfile.State(0, 0);
  private ExponentialProfile.State setpoint = new ExponentialProfile.State(0, 0);

  public Robot() {
    // Note: These gains are fake, and will have to be tuned for your robot.
    motor.setPID(1.3, 0.0, 0.7);
  }

  @Override
  public void teleopPeriodic() {
    if (joystick.getRawButtonPressed(2)) {
      goal = new ExponentialProfile.State(5, 0);
    } else if (joystick.getRawButtonPressed(3)) {
      goal = new ExponentialProfile.State(0, 0);
    }

    // Retrieve the profiled setpoint for the next timestep. This setpoint moves
    // toward the goal while obeying the constraints.
    ExponentialProfile.State next = profile.calculate(kDt, setpoint, goal);

    // Send setpoint to offboard controller PID
    motor.setSetpoint(
        ExampleSmartMotorController.PIDMode.kPosition,
        setpoint.position,
        feedforward.calculate(next.velocity) / 12.0);

    setpoint = next;
  }
}
