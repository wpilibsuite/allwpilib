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
  private final FlywheelBangBang m_flywheelBangBang = new FlywheelBangBang();
  private final FlywheelPIDF m_flywheelPIDF = new FlywheelPIDF();
  private final TurretPositionPIDF m_turretPositionPIDF = new TurretPositionPIDF();
  private final VerticalArmPositionPIDF m_verticalArmPositionPIDF = new VerticalArmPositionPIDF();
  private final ElevatorPIDF m_elevatorPIDF = new ElevatorPIDF();

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

  private boolean m_usingSetpoint1 = false;
  private int m_toggleCounter = 0;

  /** Creates a new Robot instance to house all mechanisms. */
  public Robot() {
    if (RobotBase.isSimulation()) {
      m_flywheelBangBang.initializeSimulation();
      m_flywheelPIDF.initializeSimulation();
      m_turretPositionPIDF.initializeSimulation();
      m_verticalArmPositionPIDF.initializeSimulation();
      m_elevatorPIDF.initializeSimulation();
    }
  }

  @Override
  public void robotPeriodic() {
    // Update all mechanisms
    m_flywheelBangBang.update();
    m_flywheelPIDF.update();
    m_turretPositionPIDF.update();
    m_verticalArmPositionPIDF.update();
    m_elevatorPIDF.update();

    // Update telemetry for all mechanisms
    m_flywheelBangBang.updateTelemetry();
    m_flywheelPIDF.updateTelemetry();
    m_turretPositionPIDF.updateTelemetry();
    m_verticalArmPositionPIDF.updateTelemetry();
    m_elevatorPIDF.updateTelemetry();

    // Toggle setpoints every 5 seconds (250 iterations at 50Hz)
    m_toggleCounter++;
    if (m_toggleCounter >= 250) {
      m_usingSetpoint1 = !m_usingSetpoint1;
      m_toggleCounter = 0;

      // Set setpoints for all mechanisms
      m_flywheelBangBang.setSetpoint(m_usingSetpoint1 ? kFlywheelSetpoint1 : kFlywheelSetpoint2);
      m_flywheelPIDF.setSetpoint(m_usingSetpoint1 ? kFlywheelSetpoint1 : kFlywheelSetpoint2);
      m_turretPositionPIDF.setSetpoint(m_usingSetpoint1 ? kTurretSetpoint1 : kTurretSetpoint2);
      m_verticalArmPositionPIDF.setSetpoint(m_usingSetpoint1 ? kArmSetpoint1 : kArmSetpoint2);
      m_elevatorPIDF.setSetpoint(m_usingSetpoint1 ? kElevatorSetpoint1 : kElevatorSetpoint2);
    }
  }

  @Override
  public void simulationPeriodic() {
    // Update simulation for all mechanisms
    m_flywheelBangBang.updateSimulation();
    m_flywheelPIDF.updateSimulation();
    m_turretPositionPIDF.updateSimulation();
    m_verticalArmPositionPIDF.updateSimulation();
    m_elevatorPIDF.updateSimulation();
  }

  @Override
  public void disabledInit() {
    // Stop all mechanisms
    m_flywheelBangBang.setSetpoint(0.0);
    m_flywheelPIDF.setSetpoint(0.0);
    m_turretPositionPIDF.setSetpoint(0.0);
    m_verticalArmPositionPIDF.setSetpoint(0.0);
    m_elevatorPIDF.setSetpoint(0.0);
  }

  @Override
  public void close() {
    m_flywheelBangBang.close();
    m_flywheelPIDF.close();
    m_turretPositionPIDF.close();
    m_verticalArmPositionPIDF.close();
    m_elevatorPIDF.close();
    super.close();
  }
}
