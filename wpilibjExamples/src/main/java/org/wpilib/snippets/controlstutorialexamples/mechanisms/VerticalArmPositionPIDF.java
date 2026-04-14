// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.snippets.controlstutorialexamples.mechanisms;

import org.wpilib.hardware.motor.PWMSparkMax;
import org.wpilib.hardware.rotation.Encoder;
import org.wpilib.math.controller.PIDController;
import org.wpilib.math.filter.LinearFilter;
import org.wpilib.math.system.DCMotor;
import org.wpilib.math.util.Units;
import org.wpilib.simulation.BatterySim;
import org.wpilib.simulation.EncoderSim;
import org.wpilib.simulation.PWMMotorControllerSim;
import org.wpilib.simulation.RoboRioSim;
import org.wpilib.simulation.SingleJointedArmSim;
import org.wpilib.smartdashboard.SmartDashboard;
import org.wpilib.system.RobotController;

// Suppression is intentional - this file shows a "simple-as-possible" implementation
// that a beginner might reference. It is not intended to show "best" coding practices.
@SuppressWarnings("all")
public class VerticalArmPositionPIDF implements AutoCloseable {
  // Physical mechanism constants
  private double kGearing = 100.0;
  private double kArmLength = 1.2; // meters
  private double kArmMass = 5.0; // kg

  // Tuned Controller Constants - Tune these like in the tutorial!
  private double kKp = 40.0;
  private double kKi = 0.0;
  private double kKd = 0.5;
  private double kG = 0.36; // Gravity-comepnsation gain

  // Electronics Hardware: CIM motor controlled via SPARK PWM motor controller
  private int kMotorPort = 3;
  private int kEncoderAChannel = 6;
  private int kEncoderBChannel = 7;
  private double kVerticalArmRadiansPerEncoderPulse = 2.0 * Math.PI / 2048.0;
  private DCMotor m_armMotor;
  private Encoder m_encoder;
  private PWMSparkMax m_motor;

  // Controls Helpers: WPILib built-in classes for position control
  private PIDController m_controller;

  // Simulation Support
  private SingleJointedArmSim m_armSim;
  private EncoderSim m_encoderSim;
  private PWMMotorControllerSim m_motorSim;
  // Simulation sensor filters
  private LinearFilter m_angleFilter;

  // State Variables
  private double m_desiredPosition = 0.0;
  private double m_voltage = 0.0;
  private double m_actualPosition = 0.0;

  public VerticalArmPositionPIDF() {
    // Set up quadrature encoder for position measurement
    m_encoder = new Encoder(kEncoderAChannel, kEncoderBChannel);
    m_encoder.setDistancePerPulse(kVerticalArmRadiansPerEncoderPulse);

    // Set up SPARK PWM motor controller
    m_motor = new PWMSparkMax(kMotorPort);

    // Set up WPILib's built-in PID controller for position control
    m_controller = new PIDController(kKp, kKi, kKd);
  }

  // Initialize simulation components
  /**
   * Initializes the simulation components for this mechanism. This method should be called when
   * running in simulation mode to set up the physics simulation models for the vertical arm
   * mechanism.
   */
  public void initializeSimulation() {
    // Set up CIM motor model for simulation
    m_armMotor = DCMotor.getKrakenX60(2);

    // Set up simulation model for the vertical arm mechanism
    m_armSim =
        new SingleJointedArmSim(
            m_armMotor,
            kGearing,
            kArmMass
                / (kArmLength
                    * kArmLength), // moment of inertia assumed to be point mass at end of arm.
            kArmLength,
            -Math.PI, // full rotation range
            Math.PI,
            true,
            0.0);

    // Set up simulation model for the encoder
    m_encoderSim = new EncoderSim(m_encoder);

    // Create sensor filter for arm angle
    m_angleFilter = LinearFilter.singlePoleIIR(0.05, 0.02);

    // Set up simulation model for the motor controller
    m_motorSim = new PWMMotorControllerSim(m_motor);
  }

  /**
   * Updates the vertical arm control loop. This method should be called periodically (typically
   * every 20ms) to: 1. Read the current position from the encoder 2. Calculate the PID +
   * feedforward control output 3. Send the calculated voltage to the motor
   */
  public void update() {
    // Step 1: Read Sensors
    m_actualPosition = m_encoder.getDistance();

    // Step 2: Calculate
    double pidOutput = m_controller.calculate(m_actualPosition, m_desiredPosition);
    // Gravity compensation proportional to cosine of the angle (0 = straight out)
    double gravityComp = kG * Math.cos(m_actualPosition);
    m_voltage = pidOutput + gravityComp;

    // Step 3: Send Outputs
    m_motor.setVoltage(m_voltage);
  }

  /**
   * Sets the desired position setpoint for the vertical arm.
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
    if (m_armSim != null) {
      m_armSim.setInput(m_motorSim.getThrottle() * RobotController.getBatteryVoltage());
      m_armSim.update(0.020);
      double filteredAngle = m_angleFilter.calculate(m_armSim.getAngle());
      m_encoderSim.setDistance(filteredAngle);
      RoboRioSim.setVInVoltage(
          BatterySim.calculateDefaultBatteryLoadedVoltage(m_armSim.getCurrentDraw()));
    }
  }

  /**
   * Updates telemetry data to SmartDashboard. This method should be called periodically to publish
   * mechanism state information for debugging and monitoring.
   */
  public void updateTelemetry() {
    SmartDashboard.putNumber("VerticalArmPositionPIDF/MotorVoltage_V", m_voltage);
    SmartDashboard.putNumber(
        "VerticalArmPositionPIDF/ActualPosition_degrees", Units.radiansToDegrees(m_actualPosition));
    SmartDashboard.putNumber(
        "VerticalArmPositionPIDF/DesiredPosition_degrees",
        Units.radiansToDegrees(m_desiredPosition));
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
