// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.snippets.controlstutorialexamples.mechanisms;

import org.wpilib.hardware.motor.PWMSparkMax;
import org.wpilib.hardware.rotation.Encoder;
import org.wpilib.math.system.DCMotor;
import org.wpilib.simulation.BatterySim;
import org.wpilib.simulation.EncoderSim;
import org.wpilib.simulation.PWMMotorControllerSim;
import org.wpilib.simulation.RoboRioSim;
import org.wpilib.simulation.SingleJointedArmSim;
import org.wpilib.smartdashboard.SmartDashboard;
import org.wpilib.system.RobotController;

public class FlywheelBangBang implements AutoCloseable {
  // No tunable gains!

  // Electronics Hardware: CIM motor controlled via SPARK PWM motor controller
  private DCMotor m_flywheelMotor;
  private Encoder m_encoder;
  private PWMSparkMax m_motor;
  private int kMotorPort = 0;
  private int kEncoderAChannel = 0;
  private int kEncoderBChannel = 1;
  private double kFlywheelRadiansPerEncoderPulse = 2.0 * Math.PI / 2048.0;

  // Simulation Support
  private SingleJointedArmSim m_flywheelSim;
  private EncoderSim m_encoderSim;
  private PWMMotorControllerSim m_motorSim;

  // State Variables
  private double m_desiredVelocity = 0.0;
  private double m_voltage = 0.0;
  private double m_actualVelocity = 0.0;

  public FlywheelBangBang() {
    // Set up quadrature encoder for velocity measurement
    m_encoder = new Encoder(kEncoderAChannel, kEncoderBChannel);
    m_encoder.setDistancePerPulse(kFlywheelRadiansPerEncoderPulse);

    // Set up SPARK PWM motor controller
    m_motor = new PWMSparkMax(kMotorPort);
  }

  // Initialize simulation components
  /**
   * Initializes the simulation components for this mechanism.
   * This method should be called when running in simulation mode to set up
   * the physics simulation models for the flywheel mechanism.
   */
  public void initializeSimulation() {
    // Set up CIM motor model for simulation
    m_flywheelMotor = DCMotor.getCIM(1);

    // Set up simulation model for the flywheel mechanism
    m_flywheelSim = new SingleJointedArmSim(
        m_flywheelMotor,
        1.0, // gearing
        SingleJointedArmSim.estimateMOI(0.1, 0.1), // sample values for flywheel
        0.1, // sample length
        -Math.PI * 100, // large range
        Math.PI * 100,
        true,
        0.0);

    // Set up simulation model for the encoder
    m_encoderSim = new EncoderSim(m_encoder);

    // Set up simulation model for the motor controller
    m_motorSim = new PWMMotorControllerSim(m_motor);
  }

  /**
   * Updates the flywheel control loop.
   * This method should be called periodically (typically every 20ms) to:
   * 1. Read the current velocity from the encoder
   * 2. Calculate the bang-bang control output
   * 3. Send the calculated voltage to the motor
   */
  public void update() {
    // Step 1: Read Sensors
    m_actualVelocity = m_encoder.getRate();

    // Step 2: Calculate
    // Bang-bang control: full power if below setpoint, off if above
    if (m_actualVelocity < m_desiredVelocity) {
      m_voltage = 12.0; // full power
    } else {
      m_voltage = 0.0; // off
    }

    // Step 3: Send Outputs
    m_motor.setVoltage(m_voltage);
  }

  /**
   * Sets the desired velocity setpoint for the flywheel.
   *
   * @param setpoint The desired velocity in RPM
   */
  public void setSetpoint(double setpoint) {
    m_desiredVelocity = setpoint;
  }

  /**
   * Updates the simulation models.
   * This method should be called periodically when running in simulation mode
   * to update the physics simulation and synchronize simulated sensors.
   */
  public void updateSimulation() {
    if (m_flywheelSim != null) {
      m_flywheelSim.setInput(m_motorSim.getThrottle() * RobotController.getBatteryVoltage());
      m_flywheelSim.update(0.020);
      m_encoderSim.setRate(m_flywheelSim.getVelocity());
      RoboRioSim.setVInVoltage(
          BatterySim.calculateDefaultBatteryLoadedVoltage(m_flywheelSim.getCurrentDraw()));
    }
  }

  /**
   * Updates telemetry data to SmartDashboard.
   * This method should be called periodically to publish mechanism state
   * information for debugging and monitoring.
   */
  public void updateTelemetry() {
    SmartDashboard.putNumber("FlywheelBangBang/MotorVoltage", m_voltage);
    SmartDashboard.putNumber("FlywheelBangBang/ActualVelocity", m_actualVelocity);
    SmartDashboard.putNumber("FlywheelBangBang/DesiredVelocity", m_desiredVelocity);
  }

  /**
   * Closes hardware resources.
   * This method should be called when the mechanism is no longer needed
   * to properly clean up encoder and motor controller resources.
   */
  @Override
  public void close() {
    m_encoder.close();
    m_motor.close();
  }
}
