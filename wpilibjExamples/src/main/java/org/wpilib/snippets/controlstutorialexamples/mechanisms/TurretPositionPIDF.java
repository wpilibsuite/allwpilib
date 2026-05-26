// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.snippets.controlstutorialexamples.mechanisms;

import org.wpilib.hardware.motor.PWMSparkMax;
import org.wpilib.hardware.rotation.Encoder;
import org.wpilib.math.controller.PIDController;
import org.wpilib.math.filter.LinearFilter;
import org.wpilib.math.system.DCMotor;
import org.wpilib.math.system.Models;
import org.wpilib.math.util.Units;
import org.wpilib.simulation.BatterySim;
import org.wpilib.simulation.EncoderSim;
import org.wpilib.simulation.FlywheelSim;
import org.wpilib.simulation.PWMMotorControllerSim;
import org.wpilib.simulation.RoboRioSim;
import org.wpilib.smartdashboard.SmartDashboard;
import org.wpilib.system.RobotController;

/** Turret position PIDF example using a flywheel-style plant for simulation. */
public class TurretPositionPIDF implements AutoCloseable {
  // Physical mechanism constants (sample values)
  double kTurretMassKg = 0.75; // sample mass in kg
  double kTurretRadiusMeters = 0.05; // sample radius in m
  double kGearing = 300.0; // Reduction (motor:output)
  double kMomentOfInertia = 0.5 * kTurretMassKg * Math.pow(kTurretRadiusMeters, 2);

  // Tuned controller constants - tune these like in the tutorial
  double kP = 100.0; // Feedback Proportional gain
  double kI = 0.0; // Feedback Integral gain
  double kD = 1.0; // Feedback Derivative gain

  // Electronics hardware
  int kMotorPort = 2;
  int kEncoderAChannel = 4;
  int kEncoderBChannel = 5;
  double kTurretRadiansPerEncoderPulse = 2.0 * Math.PI / 2048.0;

  // Hardware
  DCMotor m_turretMotor;
  Encoder m_encoder;
  PWMSparkMax m_motor;

  // Controls helpers
  PIDController m_controller;

  // Simulation support
  FlywheelSim m_turretSim;
  // Integrated angle for position tracking (FlywheelSim models only velocity)
  double m_turretAngle = 0.0;
  EncoderSim m_encoderSim;
  PWMMotorControllerSim m_motorSim;
  // Simulation sensor filters
  LinearFilter m_velocityFilter;

  // State variables
  double m_desiredPosition = 0.0;
  double m_voltage = 0.0;
  double m_actualPosition = 0.0;

  /** Constructor: set up encoder, motor and PID controller for the turret. */
  public TurretPositionPIDF() {
    // Set up quadrature encoder for position measurement
    m_encoder = new Encoder(kEncoderAChannel, kEncoderBChannel);
    m_encoder.setDistancePerPulse(kTurretRadiansPerEncoderPulse);

    // Set up SPARK PWM motor controller
    m_motor = new PWMSparkMax(kMotorPort);

    // Set up WPILib's built-in PID controller for position control
    m_controller = new PIDController(kP, kI, kD);
  }

  // Initialize simulation components
  /**
   * Initializes the simulation components for this mechanism. This method should be called when
   * running in simulation mode to set up the physics simulation models for the turret mechanism.
   */
  public void initializeSimulation() {
    // Set up Vex 775 Pro motor model for simulation
    m_turretMotor = DCMotor.getVex775Pro(1);

    // Build a flywheel-style plant for the turret and create the sim.
    var plant = Models.flywheelFromPhysicalConstants(m_turretMotor, kMomentOfInertia, kGearing);
    m_turretSim = new FlywheelSim(plant, m_turretMotor);

    // Set up simulation model for the encoder
    m_encoderSim = new EncoderSim(m_encoder);

    // Create sensor filter for angular velocity feedback
    m_velocityFilter = LinearFilter.singlePoleIIR(0.05, 0.02);

    // Set up simulation model for the motor controller
    m_motorSim = new PWMMotorControllerSim(m_motor);
  }

  /**
   * Updates the turret control loop. This method should be called periodically (typically every
   * 20ms) to: 1. Read the current position from the encoder 2. Calculate the PID + feedforward
   * control output 3. Send the calculated voltage to the motor
   */
  public void update() {
    //////////////////////////////////////////////////
    // Step 1: Read Sensors
    m_actualPosition = m_encoder.getDistance();

    //////////////////////////////////////////////////
    // Step 2: Calculate Control

    // Position-based Feedback control only
    m_voltage = m_controller.calculate(m_actualPosition, m_desiredPosition);

    // Clamp voltage command to physically possible range
    if (m_voltage > 12.0) {
      m_voltage = 12.0;
    } else if (m_voltage < -12.0) {
      m_voltage = -12.0;
    }

    //////////////////////////////////////////////////
    // Step 3: Send Outputs
    m_motor.setVoltage(m_voltage);
  }

  /**
   * Sets the desired position setpoint for the turret.
   *
   * @param setpoint The desired position in radians
   */
  public void setSetpoint(double setpoint) {
    m_desiredPosition = setpoint;
  }

  /**
   * Updates the simulation models. This method should be called periodically when running in
   * simulation mode to update the physics simulation and synchronize simulated sensors.
   */
  public void updateSimulation() {
    if (m_turretSim != null) {
      double vbat = RobotController.getBatteryVoltage();
      double volts = m_motorSim.getThrottle() * vbat;
      if (volts > vbat) {
        volts = vbat;
      } else if (volts < -vbat) {
        volts = -vbat;
      }
      m_turretSim.setInputVoltage(volts);
      m_turretSim.update(0.020);
      // Integrate filtered angular velocity to obtain position for the encoder
      double filteredVel = m_velocityFilter.calculate(m_turretSim.getAngularVelocity());
      m_turretAngle += filteredVel * 0.020;
      m_encoderSim.setDistance(m_turretAngle);
      RoboRioSim.setVInVoltage(
          BatterySim.calculateDefaultBatteryLoadedVoltage(m_turretSim.getCurrentDraw()));
    }
  }

  /**
   * Updates telemetry data to SmartDashboard. This method should be called periodically to publish
   * mechanism state information for debugging and monitoring.
   */
  public void updateTelemetry() {
    SmartDashboard.putNumber("TurretPositionPIDF/MotorVoltage_V", m_voltage);
    SmartDashboard.putNumber(
        "TurretPositionPIDF/ActualPosition_degrees", Units.radiansToDegrees(m_actualPosition));
    SmartDashboard.putNumber(
        "TurretPositionPIDF/DesiredPosition_degrees", Units.radiansToDegrees(m_desiredPosition));
  }

  /**
   * Closes hardware resources. This method should be called when the mechanism is no longer needed
   * to properly clean up encoder and motor controller resources.
   */
  @Override
  public void close() {
    m_encoder.close();
    m_motor.close();
  }
}
