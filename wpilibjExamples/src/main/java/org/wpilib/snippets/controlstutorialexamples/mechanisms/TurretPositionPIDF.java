// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.snippets.controlstutorialexamples.mechanisms;

import org.wpilib.hardware.motor.PWMSparkMax;
import org.wpilib.hardware.rotation.Encoder;
import org.wpilib.math.controller.PIDController;
import org.wpilib.math.system.DCMotor;
import org.wpilib.simulation.BatterySim;
import org.wpilib.simulation.EncoderSim;
import org.wpilib.simulation.PWMMotorControllerSim;
import org.wpilib.simulation.RoboRioSim;
import org.wpilib.simulation.SingleJointedArmSim;
import org.wpilib.smartdashboard.SmartDashboard;
import org.wpilib.system.RobotController;

@SuppressWarnings("all")
public class TurretPositionPIDF implements AutoCloseable {
  // Physical mechanism constants
  private double kGearing = 300.0;
  private double kMomentOfInertia = 0.005; // kg * m^2

  // Tuned Controller Constants - Tune these like in the tutorial!
  private double kKp = 5.0;
  private double kKi = 0.0;
  private double kKd = 0.0;

  // Electronics Hardware: CIM motor controlled via SPARK PWM motor controller
  private int kMotorPort = 2;
  private int kEncoderAChannel = 4;
  private int kEncoderBChannel = 5;
  private double kTurretRadiansPerEncoderPulse = 2.0 * Math.PI / 2048.0;
  private DCMotor m_turretMotor;
  private Encoder m_encoder;
  private PWMSparkMax m_motor;

  // Controls Helpers: WPILib built-in classes for position control
  private PIDController m_controller;

  // Simulation Support
  private SingleJointedArmSim m_turretSim;
  private EncoderSim m_encoderSim;
  private PWMMotorControllerSim m_motorSim;

  // State Variables
  private double m_desiredPosition = 0.0;
  private double m_voltage = 0.0;
  private double m_actualPosition = 0.0;

  public TurretPositionPIDF() {
    // Set up quadrature encoder for position measurement
    m_encoder = new Encoder(kEncoderAChannel, kEncoderBChannel);
    m_encoder.setDistancePerPulse(kTurretRadiansPerEncoderPulse);

    // Set up SPARK PWM motor controller
    m_motor = new PWMSparkMax(kMotorPort);

    // Set up WPILib's built-in PID controller for position control
    m_controller = new PIDController(kKp, kKi, kKd);
  }

  // Initialize simulation components
  /**
   * Initializes the simulation components for this mechanism. This method should be called when
   * running in simulation mode to set up the physics simulation models for the turret mechanism.
   */
  public void initializeSimulation() {
    // Set up CIM motor model for simulation
    m_turretMotor = DCMotor.getCIM(1);

    // Set up simulation model for the turret mechanism
    m_turretSim =
        new SingleJointedArmSim(
            m_turretMotor,
            kGearing,
            kMomentOfInertia,
            0.1, // sample length
            -Math.PI, // full rotation range
            Math.PI,
            true,
            0.0);

    // Set up simulation model for the encoder
    m_encoderSim = new EncoderSim(m_encoder);

    // Set up simulation model for the motor controller
    m_motorSim = new PWMMotorControllerSim(m_motor);
  }

  /**
   * Updates the turret control loop. This method should be called periodically (typically every
   * 20ms) to: 1. Read the current position from the encoder 2. Calculate the PID + feedforward
   * control output 3. Send the calculated voltage to the motor
   */
  public void update() {
    // Step 1: Read Sensors
    m_actualPosition = m_encoder.getDistance();

    // Step 2: Calculate
    m_voltage = m_controller.calculate(m_actualPosition, m_desiredPosition);

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
      m_turretSim.setInput(m_motorSim.getThrottle() * RobotController.getBatteryVoltage());
      m_turretSim.update(0.020);
      m_encoderSim.setDistance(m_turretSim.getAngle());
      RoboRioSim.setVInVoltage(
          BatterySim.calculateDefaultBatteryLoadedVoltage(m_turretSim.getCurrentDraw()));
    }
  }

  /**
   * Updates telemetry data to SmartDashboard. This method should be called periodically to publish
   * mechanism state information for debugging and monitoring.
   */
  public void updateTelemetry() {
    SmartDashboard.putNumber("TurretPositionPIDF/MotorVoltage", m_voltage);
    SmartDashboard.putNumber("TurretPositionPIDF/ActualPosition", m_actualPosition);
    SmartDashboard.putNumber("TurretPositionPIDF/DesiredPosition", m_desiredPosition);
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
