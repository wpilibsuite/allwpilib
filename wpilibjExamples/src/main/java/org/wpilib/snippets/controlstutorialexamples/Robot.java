// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.snippets.controlstutorialexamples;

import org.wpilib.framework.RobotBase;
import org.wpilib.framework.TimedRobot;
import org.wpilib.math.util.Units;
import org.wpilib.snippets.controlstutorialexamples.mechanisms.ElevatorPIDF;
import org.wpilib.snippets.controlstutorialexamples.mechanisms.FlywheelBangBang;
import org.wpilib.snippets.controlstutorialexamples.mechanisms.FlywheelPIDF;
import org.wpilib.snippets.controlstutorialexamples.mechanisms.TurretPositionPIDF;
import org.wpilib.snippets.controlstutorialexamples.mechanisms.VerticalArmPositionPIDF;

/** This is a sample program to demonstrate various control mechanisms. */
public class Robot extends TimedRobot {
  private final FlywheelBangBang flywheelBangBang = new FlywheelBangBang();
  private final FlywheelPIDF flywheelPIDF = new FlywheelPIDF();
  private final TurretPositionPIDF turretPositionPIDF = new TurretPositionPIDF();
  private final VerticalArmPositionPIDF verticalArmPositionPIDF = new VerticalArmPositionPIDF();
  private final ElevatorPIDF elevatorPIDF = new ElevatorPIDF();

  // Setpoint constants
  private static final double kFlywheelSetpoint1 =
      Units.rotationsPerMinuteToRadiansPerSecond(500.0);
  private static final double kFlywheelSetpoint2 = Units.rotationsPerMinuteToRadiansPerSecond(0.0);
  private static final double kTurretSetpoint1 = Math.toRadians(45.0);
  private static final double kTurretSetpoint2 = Math.toRadians(-45.0);
  private static final double kArmSetpoint1 = Math.toRadians(45.0);
  private static final double kArmSetpoint2 = Math.toRadians(-10.0);
  private static final double kElevatorSetpoint1 = 0.85; // meters
  private static final double kElevatorSetpoint2 = 0.1; // meters

  private boolean usingSetpoint1 = false;
  private int toggleCounter = 0;

  /** Creates a new Robot instance to house all mechanisms. */
  public Robot() {
    if (RobotBase.isSimulation()) {
      flywheelBangBang.initializeSimulation();
      flywheelPIDF.initializeSimulation();
      turretPositionPIDF.initializeSimulation();
      verticalArmPositionPIDF.initializeSimulation();
      elevatorPIDF.initializeSimulation();
    }
  }

  @Override
  public void robotPeriodic() {
    // Update all mechanisms
    flywheelBangBang.update();
    flywheelPIDF.update();
    turretPositionPIDF.update();
    verticalArmPositionPIDF.update();
    elevatorPIDF.update();

    // Update telemetry for all mechanisms
    flywheelBangBang.updateTelemetry();
    flywheelPIDF.updateTelemetry();
    turretPositionPIDF.updateTelemetry();
    verticalArmPositionPIDF.updateTelemetry();
    elevatorPIDF.updateTelemetry();

    // Toggle setpoints every 5 seconds (250 iterations at 50Hz)
    toggleCounter++;
    if (toggleCounter >= 250) {
      usingSetpoint1 = !usingSetpoint1;
      toggleCounter = 0;

      // Set setpoints for all mechanisms
      flywheelBangBang.setSetpoint(usingSetpoint1 ? kFlywheelSetpoint1 : kFlywheelSetpoint2);
      flywheelPIDF.setSetpoint(usingSetpoint1 ? kFlywheelSetpoint1 : kFlywheelSetpoint2);
      turretPositionPIDF.setSetpoint(usingSetpoint1 ? kTurretSetpoint1 : kTurretSetpoint2);
      verticalArmPositionPIDF.setSetpoint(usingSetpoint1 ? kArmSetpoint1 : kArmSetpoint2);
      elevatorPIDF.setSetpoint(usingSetpoint1 ? kElevatorSetpoint1 : kElevatorSetpoint2);
    }
  }

  @Override
  public void simulationPeriodic() {
    // Update simulation for all mechanisms
    flywheelBangBang.updateSimulation();
    flywheelPIDF.updateSimulation();
    turretPositionPIDF.updateSimulation();
    verticalArmPositionPIDF.updateSimulation();
    elevatorPIDF.updateSimulation();
  }

  @Override
  public void disabledInit() {
    // Stop all mechanisms
    flywheelBangBang.setSetpoint(0.0);
    flywheelPIDF.setSetpoint(0.0);
    turretPositionPIDF.setSetpoint(0.0);
    verticalArmPositionPIDF.setSetpoint(0.0);
    elevatorPIDF.setSetpoint(0.0);
  }

  @Override
  public void close() {
    flywheelBangBang.close();
    flywheelPIDF.close();
    turretPositionPIDF.close();
    verticalArmPositionPIDF.close();
    elevatorPIDF.close();
    super.close();
  }
}
