// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.snippets.controlstutorialexamples.mechanisms;

import org.wpilib.drivers.motor.PWMSparkMax;
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
  DCMotor turretMotor;
  Encoder encoder;
  PWMSparkMax motor;

  // Controls helpers
  PIDController controller;

  // Simulation support
  FlywheelSim turretSim;
  // Integrated angle for position tracking (FlywheelSim models only velocity)
  double turretAngle = 0.0;
  EncoderSim encoderSim;
  PWMMotorControllerSim motorSim;
  // Simulation sensor filters
  LinearFilter velocityFilter;

  // State variables
  double desiredPosition = 0.0;
  double voltage = 0.0;
  double actualPosition = 0.0;

  /** Constructor: set up encoder, motor and PID controller for the turret. */
  public TurretPositionPIDF() {
    // Set up quadrature encoder for position measurement
    encoder = new Encoder(kEncoderAChannel, kEncoderBChannel);
    encoder.setDistancePerPulse(kTurretRadiansPerEncoderPulse);

    // Set up SPARK PWM motor controller
    motor = new PWMSparkMax(kMotorPort);

    // Set up WPILib's built-in PID controller for position control
    controller = new PIDController(kP, kI, kD);
  }

  // Initialize simulation components
  /**
   * Initializes the simulation components for this mechanism. This method should be called when
   * running in simulation mode to set up the physics simulation models for the turret mechanism.
   */
  public void initializeSimulation() {
    // Set up Vex 775 Pro motor model for simulation
    turretMotor = DCMotor.getVex775Pro(1);

    // Build a flywheel-style plant for the turret and create the sim.
    var plant = Models.flywheelFromPhysicalConstants(turretMotor, kMomentOfInertia, kGearing);
    turretSim = new FlywheelSim(plant, turretMotor);

    // Set up simulation model for the encoder
    encoderSim = new EncoderSim(encoder);

    // Create sensor filter for angular velocity feedback
    velocityFilter = LinearFilter.singlePoleIIR(0.05, 0.02);

    // Set up simulation model for the motor controller
    motorSim = new PWMMotorControllerSim(motor);
  }

  /**
   * Updates the turret control loop. This method should be called periodically (typically every
   * 20ms) to: 1. Read the current position from the encoder 2. Calculate the PID + feedforward
   * control output 3. Send the calculated voltage to the motor
   */
  public void update() {
    //////////////////////////////////////////////////
    // Step 1: Read Sensors
    actualPosition = encoder.getDistance();

    //////////////////////////////////////////////////
    // Step 2: Calculate Control

    // Position-based Feedback control only
    voltage = controller.calculate(actualPosition, desiredPosition);

    // Clamp voltage command to physically possible range
    if (voltage > 12.0) {
      voltage = 12.0;
    } else if (voltage < -12.0) {
      voltage = -12.0;
    }

    //////////////////////////////////////////////////
    // Step 3: Send Outputs
    motor.setVoltage(voltage);
  }

  /**
   * Sets the desired position setpoint for the turret.
   *
   * @param setpoint The desired position in radians
   */
  public void setSetpoint(double setpoint) {
    desiredPosition = setpoint;
  }

  /**
   * Updates the simulation models. This method should be called periodically when running in
   * simulation mode to update the physics simulation and synchronize simulated sensors.
   */
  public void updateSimulation() {
    if (turretSim != null) {
      double vbat = RobotController.getBatteryVoltage();
      double volts = motorSim.getThrottle() * vbat;
      if (volts > vbat) {
        volts = vbat;
      } else if (volts < -vbat) {
        volts = -vbat;
      }
      turretSim.setInputVoltage(volts);
      turretSim.update(0.020);
      // Integrate filtered angular velocity to obtain position for the encoder
      double filteredVel = velocityFilter.calculate(turretSim.getAngularVelocity());
      turretAngle += filteredVel * 0.020;
      encoderSim.setDistance(turretAngle);
      RoboRioSim.setVInVoltage(
          BatterySim.calculateDefaultBatteryLoadedVoltage(turretSim.getCurrentDraw()));
    }
  }

  /**
   * Updates telemetry data to SmartDashboard. This method should be called periodically to publish
   * mechanism state information for debugging and monitoring.
   */
  public void updateTelemetry() {
    SmartDashboard.putNumber("TurretPositionPIDF/MotorVoltage_V", voltage);
    SmartDashboard.putNumber(
        "TurretPositionPIDF/ActualPosition_degrees", Units.radiansToDegrees(actualPosition));
    SmartDashboard.putNumber(
        "TurretPositionPIDF/DesiredPosition_degrees", Units.radiansToDegrees(desiredPosition));
  }

  /**
   * Closes hardware resources. This method should be called when the mechanism is no longer needed
   * to properly clean up encoder and motor controller resources.
   */
  @Override
  public void close() {
    encoder.close();
    motor.close();
  }
}
