// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.snippets.controlstutorialexamples.mechanisms;

import org.wpilib.hardware.motor.PWMSparkMax;
import org.wpilib.hardware.rotation.Encoder;
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

/** Simple flywheel bang-bang example with simulation and telemetry. */
public class FlywheelBangBang implements AutoCloseable {
  // Physical mechanism constants
  double kFlywheelMassKg = 2.55; // sample value
  double kFlywheelRadiusMeters = 0.0762; // sample value
  double kFlywheelMomentOfInertia = 0.5 * kFlywheelMassKg * Math.pow(kFlywheelRadiusMeters, 2);
  double kGearing = 5.0; // Reduction (motor:output)

  // No tunable constants!

  // Electronics hardware
  int kMotorPort = 0;
  int kEncoderAChannel = 0;
  int kEncoderBChannel = 1;
  double kFlywheelRadiansPerEncoderPulse = 2.0 * Math.PI / 2048.0;

  // Hardware
  DCMotor m_flywheelMotor;
  Encoder m_encoder;
  PWMSparkMax m_motor;

  // Simulation support
  FlywheelSim m_flywheelSim;
  EncoderSim m_encoderSim;
  PWMMotorControllerSim m_motorSim;
  // Simulation sensor filters (single-pole IIR ~20ms)
  LinearFilter m_encoderFilter;

  // State Variables
  double m_desiredVelocity = 0.0;
  double m_voltage = 0.0;
  double m_actualVelocity = 0.0;

  /** Constructor: set up encoder and motor controller for the bang-bang example. */
  public FlywheelBangBang() {
    // Set up quadrature encoder for velocity measurement
    m_encoder = new Encoder(kEncoderAChannel, kEncoderBChannel);
    m_encoder.setDistancePerPulse(kFlywheelRadiansPerEncoderPulse);

    // Set up SPARK PWM motor controller
    m_motor = new PWMSparkMax(kMotorPort);
  }

  // Initialize simulation components
  /**
   * Initializes the simulation components for this mechanism. This method should be called when
   * running in simulation mode to set up the physics simulation models for the flywheel mechanism.
   */
  public void initializeSimulation() {
    // Set up Vex 775 Pro motor model for simulation
    m_flywheelMotor = DCMotor.getVex775Pro(1);

    // Build a state-space plant from physical constants and create a FlywheelSim.
    var plant =
        Models.flywheelFromPhysicalConstants(m_flywheelMotor, kFlywheelMomentOfInertia, kGearing);
    m_flywheelSim = new FlywheelSim(plant, m_flywheelMotor);

    // Set up simulation model for the encoder
    m_encoderSim = new EncoderSim(m_encoder);

    // Create sensor filter for encoder feedback (20ms time constant, 20ms period)
    m_encoderFilter = LinearFilter.singlePoleIIR(0.05, 0.02);

    // Set up simulation model for the motor controller
    m_motorSim = new PWMMotorControllerSim(m_motor);
  }

  /**
   * Updates the flywheel control loop. This method should be called periodically (typically every
   * 20ms) to: 1. Read the current velocity from the encoder 2. Calculate the bang-bang control
   * output 3. Send the calculated voltage to the motor
   */
  public void update() {
    //////////////////////////////////////////////////
    // Step 1: Read Sensors
    m_actualVelocity = m_encoder.getRate();

    //////////////////////////////////////////////////
    // Step 2: Calculate
    // Bang-bang control: full power if below setpoint, off if above
    if (m_actualVelocity < m_desiredVelocity) {
      m_voltage = 12.0; // full power
    } else {
      m_voltage = 0.0; // off
    }

    //////////////////////////////////////////////////
    // Step 3: Send Outputs
    m_motor.setVoltage(m_voltage);
  }

  /**
   * Sets the desired velocity setpoint for the flywheel.
   *
   * @param setpoint The desired velocity in radians per second
   */
  public void setSetpoint(double setpoint) {
    m_desiredVelocity = setpoint;
  }

  /**
   * Updates the simulation models. This method should be called periodically when running in
   * simulation mode to update the physics simulation and synchronize simulated sensors.
   */
  public void updateSimulation() {
    if (m_flywheelSim != null) {
      double vbat = RobotController.getBatteryVoltage();

      double volts = m_motorSim.getThrottle() * vbat;

      if (volts > vbat) {
        volts = vbat;
      } else if (volts < -vbat) {
        volts = -vbat;
      }

      m_flywheelSim.setInputVoltage(volts);
      m_flywheelSim.update(0.020);
      double filteredRadPerSec = m_encoderFilter.calculate(m_flywheelSim.getAngularVelocity());
      m_encoderSim.setRate(filteredRadPerSec);
      RoboRioSim.setVInVoltage(
          BatterySim.calculateDefaultBatteryLoadedVoltage(m_flywheelSim.getCurrentDraw()));
    }
  }

  /**
   * Updates telemetry data to SmartDashboard. This method should be called periodically to publish
   * mechanism state information for debugging and monitoring.
   */
  public void updateTelemetry() {
    SmartDashboard.putNumber("FlywheelBangBang/MotorVoltage_V", m_voltage);
    SmartDashboard.putNumber(
        "FlywheelBangBang/ActualVelocity_RPM",
        Units.radiansPerSecondToRotationsPerMinute(m_actualVelocity));
    SmartDashboard.putNumber(
        "FlywheelBangBang/DesiredVelocity_RPM",
        Units.radiansPerSecondToRotationsPerMinute(m_desiredVelocity));
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
