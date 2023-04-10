// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.examples.gearsbot.subsystems;

import edu.wpi.first.math.controller.PIDController;
import edu.wpi.first.wpilibj.AnalogPotentiometer;
import edu.wpi.first.wpilibj.examples.gearsbot.Constants.ElevatorConstants;
import edu.wpi.first.wpilibj.examples.gearsbot.Robot;
import edu.wpi.first.wpilibj.motorcontrol.Victor;
import edu.wpi.first.wpilibj.smartdashboard.SmartDashboard;
import edu.wpi.first.wpilibj2.command.PIDSubsystem;

/**
 * The elevator subsystem uses PID to go to a given height. Unfortunately, in it's current state PID
 * values for simulation are different than in the real world do to minor differences.
 */
public class Elevator extends PIDSubsystem {
  private final Victor m_motor = new Victor(ElevatorConstants.kMotorPort);
  private final AnalogPotentiometer m_pot;

  /** Create a new elevator subsystem. */
  public Elevator() {
    super(
        new PIDController(
            ElevatorConstants.kP_real, ElevatorConstants.kI_real, ElevatorConstants.kD));

    if (Robot.isSimulation()) { // Check for simulation and update PID values
      getController()
          .setPID(
              ElevatorConstants.kP_simulation,
              ElevatorConstants.kI_simulation,
              ElevatorConstants.kD);
    }
    getController().setTolerance(ElevatorConstants.kTolerance);

    // Conversion value of potentiometer varies between the real world and
    // simulation

    if (Robot.isReal()) {
      m_pot = new AnalogPotentiometer(ElevatorConstants.kPotentiometerPort, -2.0 / 5);
    } else {
      // Defaults to meters
      m_pot = new AnalogPotentiometer(ElevatorConstants.kPotentiometerPort);
    }

    // Let's name everything on the LiveWindow
    addChild("Motor", m_motor);
    addChild("Pot", m_pot);
  }

  /** The log method puts interesting information to the SmartDashboard. */
  public void log() {
    SmartDashboard.putData("Elevator Pot", m_pot);
  }

  /**
   * Use the potentiometer as the PID sensor. This method is automatically called by the subsystem.
   */
  @Override
  public double getMeasurement() {
    return m_pot.get();
  }

  /** Use the motor as the PID output. This method is automatically called by the subsystem. */
  @Override
  public void useOutput(double output, double setpoint) {
    m_motor.set(output);
  }

  /** Call log method every loop. */
  @Override
  public void periodic() {
    log();
  }
}
