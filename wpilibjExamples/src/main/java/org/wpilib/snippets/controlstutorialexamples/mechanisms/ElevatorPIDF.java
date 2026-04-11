// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.snippets.controlstutorialexamples.mechanisms;

import org.wpilib.hardware.motor.PWMSparkMax;
import org.wpilib.hardware.rotation.Encoder;
import org.wpilib.math.controller.ElevatorFeedforward;
import org.wpilib.math.controller.PIDController;
import org.wpilib.math.system.DCMotor;
import org.wpilib.math.trajectory.TrapezoidProfile;
import org.wpilib.simulation.BatterySim;
import org.wpilib.simulation.ElevatorSim;
import org.wpilib.simulation.EncoderSim;
import org.wpilib.simulation.PWMMotorControllerSim;
import org.wpilib.simulation.RoboRioSim;
import org.wpilib.smartdashboard.SmartDashboard;
import org.wpilib.system.RobotController;

public class ElevatorPIDF implements AutoCloseable {
  // Physical mechanism constants
  private double kGearing = 10.0;
  private double kDrumRadius = 0.0508; // 2 inches in meters
  private double kCarriageMass = 4.0; // kg
  private double kMinHeight = 0.0; // m
  private double kMaxHeight = 1.25; // m

  // Tuned Controller Constants - Tune these like in the tutorial!
  private double kKp = 5.0;
  private double kKi = 0.0;
  private double kKd = 0.0;
  private double kProfileMaxVelocity = 2.45; // m/s
  private double kProfileMaxAcceleration = 2.45; // m/s^2
  private double kElevatorKs = 0.0; // static friction
  private double kElevatorKg = 0.762; // gravity
  private double kElevatorKv = 0.762; // velocity

  // Electronics Hardware: CIM motor controlled via SPARK PWM motor controller
  private int kMotorPort = 3;
  private int kEncoderAChannel = 6;
  private int kEncoderBChannel = 7;
  private double kElevatorHeightMetersPerEncoderPulse = 2.0 * Math.PI * 0.0508 / 4096.0; // 2" drum
  private DCMotor m_elevatorMotor;
  private Encoder m_encoder;
  private PWMSparkMax m_motor;

  // Controls Helpers: WPILib built-in classes for position control with motion profiling
  private TrapezoidProfile m_profile;
  private TrapezoidProfile.State m_profileState = new TrapezoidProfile.State();
  private PIDController m_controller;
  private ElevatorFeedforward m_feedforward;

  // Simulation Support
  private ElevatorSim m_elevatorSim;
  private EncoderSim m_encoderSim;
  private PWMMotorControllerSim m_motorSim;

  // State Variables
  private double m_desiredPosition = 0.0;
  private double m_voltage = 0.0;
  private double m_actualPosition = 0.0;
  private double m_profiledPosition = 0.0;

  public ElevatorPIDF() {
    // Set up quadrature encoder for position measurement
    m_encoder = new Encoder(kEncoderAChannel, kEncoderBChannel);
    m_encoder.setDistancePerPulse(kElevatorHeightMetersPerEncoderPulse);
    
    // Set up SPARK PWM motor controller
    m_motor = new PWMSparkMax(kMotorPort);
    
    // Set up trapezoidal profile for motion profiling
    m_profile = new TrapezoidProfile(
        new TrapezoidProfile.Constraints(kProfileMaxVelocity, kProfileMaxAcceleration));
    
    // Set up WPILib's built-in PID controller for position control
    m_controller = new PIDController(kKp, kKi, kKd);
    
    // Set up elevator feedforward controller for gravity and velocity compensation
    m_feedforward = new ElevatorFeedforward(kElevatorKs, kElevatorKg, kElevatorKv, 0.0);
  }

  // Initialize simulation components 
  /**
   * Initializes the simulation components for this mechanism.
   * This method should be called when running in simulation mode to set up
   * the physics simulation models for the elevator mechanism.
   */
  public void initializeSimulation() {
    // Set up CIM motor model for simulation
    m_elevatorMotor = DCMotor.getCIM(1);
    
    // Set up simulation model for the elevator mechanism
    m_elevatorSim = new ElevatorSim(
        m_elevatorMotor,
        kGearing,
        kCarriageMass,
        kDrumRadius,
        kMinHeight,
        kMaxHeight,
        true,
        0.0,
        0.01,
        0.0);
    
    // Set up simulation model for the encoder
    m_encoderSim = new EncoderSim(m_encoder);
    
    // Set up simulation model for the motor controller
    m_motorSim = new PWMMotorControllerSim(m_motor);
  }

  /**
   * Updates the elevator control loop.
   * This method should be called periodically (typically every 20ms) to:
   * 1. Read the current position from the encoder
   * 2. Calculate the trapezoidal profile setpoint
   * 3. Calculate the PID + feedforward control output
   * 4. Send the calculated voltage to the motor
   */
  public void update() {
    // Step 1: Read Sensors
    m_actualPosition = m_encoder.getDistance();

    // Step 2: Calculate Profile
    TrapezoidProfile.State goal = new TrapezoidProfile.State(m_desiredPosition, 0.0);
    m_profileState = m_profile.calculate(0.020, m_profileState, goal);
    m_profiledPosition = m_profileState.position;

    // Step 3: Calculate Control
    double pidOutput = m_controller.calculate(m_actualPosition, m_profiledPosition);
    double feedforwardOutput = m_feedforward.calculate(m_profileState.velocity);
    m_voltage = pidOutput + feedforwardOutput;

    // Step 4: Send Outputs
    m_motor.setVoltage(m_voltage);
  }

  /**
   * Sets the desired position setpoint for the elevator.
   *
   * @param setpoint The desired position in meters
   */
  public void setSetpoint(double setpoint) {
    m_desiredPosition = setpoint;
  }

  /**
   * Updates the simulation models.
   * This method should be called periodically when running in simulation mode
   * to update the physics simulation and synchronize simulated sensors.
   */
  public void updateSimulation() {
    if (m_elevatorSim != null) {
      m_elevatorSim.setInput(m_motorSim.getThrottle() * RobotController.getBatteryVoltage());
      m_elevatorSim.update(0.020);
      m_encoderSim.setDistance(m_elevatorSim.getPosition());
      RoboRioSim.setVInVoltage(
          BatterySim.calculateDefaultBatteryLoadedVoltage(m_elevatorSim.getCurrentDraw()));
    }
  }

  /**
   * Updates telemetry data to SmartDashboard.
   * This method should be called periodically to publish mechanism state
   * information for debugging and monitoring.
   */
  public void updateTelemetry() {
    SmartDashboard.putNumber("ElevatorPIDF/MotorVoltage", m_voltage);
    SmartDashboard.putNumber("ElevatorPIDF/ActualPosition", m_actualPosition);
    SmartDashboard.putNumber("ElevatorPIDF/ProfiledPosition", m_profiledPosition);
    SmartDashboard.putNumber("ElevatorPIDF/DesiredPosition", m_desiredPosition);
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