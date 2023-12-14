// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.examples.gearsbot.subsystems;

import static edu.wpi.first.wpilibj.examples.gearsbot.Constants.ElevatorConstants.*;

import edu.wpi.first.math.controller.PIDController;
import edu.wpi.first.wpilibj.AnalogPotentiometer;
import edu.wpi.first.wpilibj.examples.gearsbot.Robot;
import edu.wpi.first.wpilibj.motorcontrol.Victor;
import edu.wpi.first.wpilibj.smartdashboard.SmartDashboard;
import edu.wpi.first.wpilibj2.command.Command;
import edu.wpi.first.wpilibj2.command.SubsystemBase;
import java.util.function.DoubleSupplier;

/**
 * The elevator subsystem uses PID to go to a given height. Unfortunately, in it's current state PID
 * values for simulation are different than in the real world do to minor differences.
 */
public class Elevator extends SubsystemBase {
  private final Victor m_motor = new Victor(kMotorPort);
  private final AnalogPotentiometer m_pot;
  private final PIDController m_controller;

  /** Create a new elevator subsystem. */
  public Elevator() {
    m_controller = new PIDController(kP_real, kI_real, kD);

    if (Robot.isSimulation()) { // Check for simulation and update PID values
      m_controller.setPID(kP_simulation, kI_simulation, kD);
    }
    m_controller.setTolerance(kTolerance);

    // Conversion value of potentiometer varies between the real world and
    // simulation

    if (Robot.isReal()) {
      m_pot = new AnalogPotentiometer(kPotentiometerPort, -2.0 / 5);
    } else {
      // Defaults to meters
      m_pot = new AnalogPotentiometer(kPotentiometerPort);
    }

    // Let's name everything on the LiveWindow
    addChild("Motor", m_motor);
    addChild("Pot", m_pot);

    // Configure default command to hold the elevator steady
    setDefaultCommand(goTo(m_controller::getSetpoint).withName("holding height"));
  }

  /**
   * Use the potentiometer as the PID sensor. This method is automatically called by the subsystem.
   */
  public double getHeight() {
    return m_pot.get();
  }

  /**
   * The elevator subsystem uses PID to go to a given height. Unfortunately, in it's current state
   * PID values for simulation are different than in the real world do to minor differences.
   *
   * @param height The setpoint to set the elevator to
   * @return The command to move the elevator to its setpoint
   */
  public Command goTo(DoubleSupplier height) {
    return run(() -> m_motor.set(m_controller.calculate(getHeight(), height.getAsDouble())))
        .until(m_controller::atSetpoint)
        .withName("elevator to height")
        .asProxy();
  }

  /** The log method puts interesting information to the SmartDashboard. */
  public void log() {
    SmartDashboard.putData("Elevator Pot", m_pot);
  }

  /** Call log method every loop. */
  @Override
  public void periodic() {
    log();
  }
}
