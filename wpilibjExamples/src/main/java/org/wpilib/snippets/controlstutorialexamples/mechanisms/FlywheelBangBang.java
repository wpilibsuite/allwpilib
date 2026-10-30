// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.snippets.controlstutorialexamples.mechanisms;

import org.wpilib.drivers.motor.PWMSparkMax;
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
  DCMotor flywheelMotor;
  Encoder encoder;
  PWMSparkMax motor;

  // Simulation support
  FlywheelSim flywheelSim;
  EncoderSim encoderSim;
  PWMMotorControllerSim motorSim;
  // Simulation sensor filters (single-pole IIR ~20ms)
  LinearFilter encoderFilter;

  // State Variables
  double desiredVelocity = 0.0;
  double voltage = 0.0;
  double actualVelocity = 0.0;

  /** Constructor: set up encoder and motor controller for the bang-bang example. */
  public FlywheelBangBang() {
    // Set up quadrature encoder for velocity measurement
    encoder = new Encoder(kEncoderAChannel, kEncoderBChannel);
    encoder.setDistancePerPulse(kFlywheelRadiansPerEncoderPulse);

    // Set up SPARK PWM motor controller
    motor = new PWMSparkMax(kMotorPort);
  }

  // Initialize simulation components
  /**
   * Initializes the simulation components for this mechanism. This method should be called when
   * running in simulation mode to set up the physics simulation models for the flywheel mechanism.
   */
  public void initializeSimulation() {
    // Set up Vex 775 Pro motor model for simulation
    flywheelMotor = DCMotor.getVex775Pro(1);

    // Build a state-space plant from physical constants and create a FlywheelSim.
    var plant =
        Models.flywheelFromPhysicalConstants(flywheelMotor, kFlywheelMomentOfInertia, kGearing);
    flywheelSim = new FlywheelSim(plant, flywheelMotor);

    // Set up simulation model for the encoder
    encoderSim = new EncoderSim(encoder);

    // Create sensor filter for encoder feedback (20ms time constant, 20ms period)
    encoderFilter = LinearFilter.singlePoleIIR(0.05, 0.02);

    // Set up simulation model for the motor controller
    motorSim = new PWMMotorControllerSim(motor);
  }

  /**
   * Updates the flywheel control loop. This method should be called periodically (typically every
   * 20ms) to: 1. Read the current velocity from the encoder 2. Calculate the bang-bang control
   * output 3. Send the calculated voltage to the motor
   */
  public void update() {
    //////////////////////////////////////////////////
    // Step 1: Read Sensors
    actualVelocity = encoder.getRate();

    //////////////////////////////////////////////////
    // Step 2: Calculate
    // Bang-bang control: full power if below setpoint, off if above
    if (actualVelocity < desiredVelocity) {
      voltage = 12.0; // full power
    } else {
      voltage = 0.0; // off
    }

    //////////////////////////////////////////////////
    // Step 3: Send Outputs
    motor.setVoltage(voltage);
  }

  /**
   * Sets the desired velocity setpoint for the flywheel.
   *
   * @param setpoint The desired velocity in radians per second
   */
  public void setSetpoint(double setpoint) {
    desiredVelocity = setpoint;
  }

  /**
   * Updates the simulation models. This method should be called periodically when running in
   * simulation mode to update the physics simulation and synchronize simulated sensors.
   */
  public void updateSimulation() {
    if (flywheelSim != null) {
      double vbat = RobotController.getBatteryVoltage();

      double volts = motorSim.getThrottle() * vbat;

      if (volts > vbat) {
        volts = vbat;
      } else if (volts < -vbat) {
        volts = -vbat;
      }

      flywheelSim.setInputVoltage(volts);
      flywheelSim.update(0.020);
      double filteredRadPerSec = encoderFilter.calculate(flywheelSim.getAngularVelocity());
      encoderSim.setRate(filteredRadPerSec);
      RoboRioSim.setVInVoltage(
          BatterySim.calculateDefaultBatteryLoadedVoltage(flywheelSim.getCurrentDraw()));
    }
  }

  /**
   * Updates telemetry data to SmartDashboard. This method should be called periodically to publish
   * mechanism state information for debugging and monitoring.
   */
  public void updateTelemetry() {
    SmartDashboard.putNumber("FlywheelBangBang/MotorVoltage_V", voltage);
    SmartDashboard.putNumber(
        "FlywheelBangBang/ActualVelocity_RPM",
        Units.radiansPerSecondToRotationsPerMinute(actualVelocity));
    SmartDashboard.putNumber(
        "FlywheelBangBang/DesiredVelocity_RPM",
        Units.radiansPerSecondToRotationsPerMinute(desiredVelocity));
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
