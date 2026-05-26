// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.snippets.controlstutorialexamples.mechanisms;

import org.wpilib.hardware.motor.PWMSparkMax;
import org.wpilib.hardware.rotation.Encoder;
import org.wpilib.math.controller.ElevatorFeedforward;
import org.wpilib.math.controller.PIDController;
import org.wpilib.math.filter.LinearFilter;
import org.wpilib.math.system.DCMotor;
import org.wpilib.math.trajectory.TrapezoidProfile;
import org.wpilib.simulation.BatterySim;
import org.wpilib.simulation.ElevatorSim;
import org.wpilib.simulation.EncoderSim;
import org.wpilib.simulation.PWMMotorControllerSim;
import org.wpilib.simulation.RoboRioSim;
import org.wpilib.smartdashboard.SmartDashboard;
import org.wpilib.system.RobotController;

/** Elevator PIDF example with trapezoidal profiling and simulation. */
public class ElevatorPIDF implements AutoCloseable {
  // Physical mechanism constants
  double kGearing = 15.0;
  double kDrumRadius = 0.0508; // 2 inches in meters
  double kCarriageMass = 9.0; // kg
  double kMinHeight = 0.0; // m
  double kMaxHeight = 1.5; // m

  // Tuned Controller Constants - Tune these like in the tutorial
  double kP = 50.0; // Feedback Proportional Gain
  double kI = 0.0; // Feedback Integral Gain
  double kD = 0.0; // Feedback Derivative Gain
  double kS = 0.0; // FeedForward Static Friction Constant
  double kG = 0.18; // FeedForward Gravity Constant
  double kV = 6.5; // FeedForward Velocity Constant
  double kProfileMaxVelocity = 2.45; // Profiler max velocity (m/s)
  double kProfileMaxAcceleration = 2.45; // Profiler max acceleration (m/s^2)

  // Electronics Hardware: CIM motor controlled via SPARK PWM motor controller
  int kMotorPort = 4;
  int kEncoderAChannel = 8;
  int kEncoderBChannel = 9;
  double kElevatorHeightMetersPerEncoderPulse = 2.0 * Math.PI * kDrumRadius / 4096.0; // 2" drum
  DCMotor m_elevatorMotor;
  Encoder m_encoder;
  PWMSparkMax m_motor;

  // Controls Helpers: WPILib built-in classes for position control with motion profiling
  TrapezoidProfile m_profile;
  TrapezoidProfile.State m_profileState = new TrapezoidProfile.State();
  PIDController m_controller;
  ElevatorFeedforward m_feedforward;

  // Simulation Support
  ElevatorSim m_elevatorSim;
  EncoderSim m_encoderSim;
  PWMMotorControllerSim m_motorSim;
  // Simulation sensor filters
  LinearFilter m_positionFilter;

  // State Variables
  double m_desiredPosition = 0.0;
  double m_voltage = 0.0;
  double m_actualPosition = 0.0;
  double m_profiledPosition = 0.0;

  /** Constructor: set up encoder, motor controller, PID and controllers, and profiler. */
  public ElevatorPIDF() {
    // Set up quadrature encoder for position measurement
    m_encoder = new Encoder(kEncoderAChannel, kEncoderBChannel);
    m_encoder.setDistancePerPulse(kElevatorHeightMetersPerEncoderPulse);

    // Set up SPARK PWM motor controller
    m_motor = new PWMSparkMax(kMotorPort);

    // Set up trapezoidal profile for motion profiling
    m_profile =
        new TrapezoidProfile(
            new TrapezoidProfile.Constraints(kProfileMaxVelocity, kProfileMaxAcceleration));

    // Set up WPILib's built-in PID controller for position control
    m_controller = new PIDController(kP, kI, kD);

    // Set up elevator feedforward controller for gravity and velocity compensation
    m_feedforward = new ElevatorFeedforward(kS, kG, kV, 0.0);
  }

  // Initialize simulation components
  /**
   * Initializes the simulation components for this mechanism. This method should be called when
   * running in simulation mode to set up the physics simulation models for the elevator mechanism.
   */
  public void initializeSimulation() {
    // Set up Kraken X60 motor model for simulation
    m_elevatorMotor = DCMotor.getKrakenX60(2);

    // Set up simulation model for the elevator mechanism
    m_elevatorSim =
        new ElevatorSim(
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

    // Create sensor filter for elevator position
    m_positionFilter = LinearFilter.singlePoleIIR(0.05, 0.02);

    // Set up simulation model for the motor controller
    m_motorSim = new PWMMotorControllerSim(m_motor);
  }

  /**
   * Updates the elevator control loop. This method should be called periodically (typically every
   * 20ms) to: 1. Read the current position from the encoder 2. Calculate the trapezoidal profile
   * setpoint 3. Calculate the PID + feedforward control output 4. Send the calculated voltage to
   * the motor
   */
  public void update() {
    //////////////////////////////////////////////////
    // Step 1: Read Sensors
    m_actualPosition = m_encoder.getDistance();

    //////////////////////////////////////////////////
    // Step 2: Calculate Profile
    TrapezoidProfile.State goal = new TrapezoidProfile.State(m_desiredPosition, 0.0);
    m_profileState = m_profile.calculate(0.020, m_profileState, goal);
    m_profiledPosition = m_profileState.position;

    //////////////////////////////////////////////////
    // Step 3: Calculate Control

    // Velocity-based feedforward, using profiler's output
    double feedforwardOutput = m_feedforward.calculate(m_profileState.velocity);

    // Position-based feedback control
    double pidOutput = m_controller.calculate(m_actualPosition, m_profiledPosition);

    // Total control effort is sum of feedforward and feedback
    m_voltage = pidOutput + feedforwardOutput;

    // Clamp voltage command to physically possible range
    if (m_voltage > 12.0) {
      m_voltage = 12.0;
    } else if (m_voltage < -12.0) {
      m_voltage = -12.0;
    }

    //////////////////////////////////////////////////
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
   * Updates the simulation models. This method should be called periodically when running in
   * simulation mode to update the physics simulation and synchronize simulated sensors.
   */
  public void updateSimulation() {
    if (m_elevatorSim != null) {
      double vbat = RobotController.getBatteryVoltage();

      double volts = m_motorSim.getThrottle() * vbat;

      if (volts > vbat) {
        volts = vbat;
      } else if (volts < -vbat) {
        volts = -vbat;
      }

      m_elevatorSim.setInput(volts);
      m_elevatorSim.update(0.020);
      double filteredPos = m_positionFilter.calculate(m_elevatorSim.getPosition());
      m_encoderSim.setDistance(filteredPos);
      RoboRioSim.setVInVoltage(
          BatterySim.calculateDefaultBatteryLoadedVoltage(m_elevatorSim.getCurrentDraw()));
    }
  }

  /**
   * Updates telemetry data to SmartDashboard. This method should be called periodically to publish
   * mechanism state information for debugging and monitoring.
   */
  public void updateTelemetry() {
    SmartDashboard.putNumber("ElevatorPIDF/MotorVoltage_V", m_voltage);
    SmartDashboard.putNumber("ElevatorPIDF/ActualPosition_m", m_actualPosition);
    SmartDashboard.putNumber("ElevatorPIDF/ProfiledPosition_m", m_profiledPosition);
    SmartDashboard.putNumber("ElevatorPIDF/DesiredPosition_m", m_desiredPosition);
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
