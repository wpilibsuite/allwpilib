// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.examples.elevatorexponentialsimulation;

import org.wpilib.driverstation.Joystick;
import org.wpilib.examples.elevatorexponentialsimulation.subsystems.Elevator;
import org.wpilib.framework.TimedRobot;

/** This is a sample program to demonstrate the use of elevator simulation. */
public class Robot extends TimedRobot {
  private final Joystick joystick = new Joystick(Constants.kJoystickPort);
  private final Elevator elevator = new Elevator();

  public Robot() {
    super(0.020);
  }

  @Override
  public void robotPeriodic() {
    // Update the telemetry, including mechanism visualization, regardless of mode.
    elevator.updateTelemetry();
  }

  @Override
  public void simulationPeriodic() {
    // Update the simulation model.
    elevator.simulationPeriodic();
  }

  @Override
  public void teleopInit() {
    elevator.reset();
  }

  @Override
  public void teleopPeriodic() {
    if (joystick.getTrigger()) {
      // Here, we set the constant setpoint of 10 meters.
      elevator.reachGoal(Constants.kSetpoint);
    } else {
      // Otherwise, we update the setpoint to 1 meter.
      elevator.reachGoal(Constants.kLowerkSetpoint);
    }
  }

  @Override
  public void disabledInit() {
    // This just makes sure that our simulation code knows that the motor's off.
    elevator.stop();
  }

  @Override
  public void close() {
    elevator.close();
    super.close();
  }
}
