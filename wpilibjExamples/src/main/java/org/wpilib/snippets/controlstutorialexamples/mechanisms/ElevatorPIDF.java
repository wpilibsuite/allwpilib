// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.snippets.controlstutorialexamples.mechanisms;

import org.wpilib.drivers.motor.PWMSparkMax;
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
  DCMotor elevatorMotor;
  Encoder encoder;
  PWMSparkMax motor;

  // Controls Helpers: WPILib built-in classes for position control with motion profiling
  TrapezoidProfile profile;
  TrapezoidProfile.State profileState = new TrapezoidProfile.State();
  PIDController controller;
  ElevatorFeedforward feedforward;

  // Simulation Support
  ElevatorSim elevatorSim;
  EncoderSim encoderSim;
  PWMMotorControllerSim motorSim;
  // Simulation sensor filters
  LinearFilter positionFilter;

  // State Variables
  double desiredPosition = 0.0;
  double voltage = 0.0;
  double actualPosition = 0.0;
  double profiledPosition = 0.0;

  /** Constructor: set up encoder, motor controller, PID and controllers, and profiler. */
  public ElevatorPIDF() {
    // Set up quadrature encoder for position measurement
    encoder = new Encoder(kEncoderAChannel, kEncoderBChannel);
    encoder.setDistancePerPulse(kElevatorHeightMetersPerEncoderPulse);

    // Set up SPARK PWM motor controller
    motor = new PWMSparkMax(kMotorPort);

    // Set up trapezoidal profile for motion profiling
    profile =
        new TrapezoidProfile(
            new TrapezoidProfile.Constraints(kProfileMaxVelocity, kProfileMaxAcceleration));

    // Set up WPILib's built-in PID controller for position control
    controller = new PIDController(kP, kI, kD);

    // Set up elevator feedforward controller for gravity and velocity compensation
    feedforward = new ElevatorFeedforward(kS, kG, kV, 0.0);
  }

  // Initialize simulation components
  /**
   * Initializes the simulation components for this mechanism. This method should be called when
   * running in simulation mode to set up the physics simulation models for the elevator mechanism.
   */
  public void initializeSimulation() {
    // Set up Kraken X60 motor model for simulation
    elevatorMotor = DCMotor.getKrakenX60(2);

    // Set up simulation model for the elevator mechanism
    elevatorSim =
        new ElevatorSim(
            elevatorMotor,
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
    encoderSim = new EncoderSim(encoder);

    // Create sensor filter for elevator position
    positionFilter = LinearFilter.singlePoleIIR(0.05, 0.02);

    // Set up simulation model for the motor controller
    motorSim = new PWMMotorControllerSim(motor);
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
    actualPosition = encoder.getDistance();

    //////////////////////////////////////////////////
    // Step 2: Calculate Profile
    TrapezoidProfile.State goal = new TrapezoidProfile.State(desiredPosition, 0.0);
    profileState = profile.calculate(0.020, profileState, goal);
    profiledPosition = profileState.position;

    //////////////////////////////////////////////////
    // Step 3: Calculate Control

    // Velocity-based feedforward, using profiler's output
    double feedforwardOutput = feedforward.calculate(profileState.velocity);

    // Position-based feedback control
    double pidOutput = controller.calculate(actualPosition, profiledPosition);

    // Total control effort is sum of feedforward and feedback
    voltage = pidOutput + feedforwardOutput;

    // Clamp voltage command to physically possible range
    if (voltage > 12.0) {
      voltage = 12.0;
    } else if (voltage < -12.0) {
      voltage = -12.0;
    }

    //////////////////////////////////////////////////
    // Step 4: Send Outputs
    motor.setVoltage(voltage);
  }

  /**
   * Sets the desired position setpoint for the elevator.
   *
   * @param setpoint The desired position in meters
   */
  public void setSetpoint(double setpoint) {
    desiredPosition = setpoint;
  }

  /**
   * Updates the simulation models. This method should be called periodically when running in
   * simulation mode to update the physics simulation and synchronize simulated sensors.
   */
  public void updateSimulation() {
    if (elevatorSim != null) {
      double vbat = RobotController.getBatteryVoltage();

      double volts = motorSim.getThrottle() * vbat;

      if (volts > vbat) {
        volts = vbat;
      } else if (volts < -vbat) {
        volts = -vbat;
      }

      elevatorSim.setInput(volts);
      elevatorSim.update(0.020);
      double filteredPos = positionFilter.calculate(elevatorSim.getPosition());
      encoderSim.setDistance(filteredPos);
      RoboRioSim.setVInVoltage(
          BatterySim.calculateDefaultBatteryLoadedVoltage(elevatorSim.getCurrentDraw()));
    }
  }

  /**
   * Updates telemetry data to SmartDashboard. This method should be called periodically to publish
   * mechanism state information for debugging and monitoring.
   */
  public void updateTelemetry() {
    SmartDashboard.putNumber("ElevatorPIDF/MotorVoltage_V", voltage);
    SmartDashboard.putNumber("ElevatorPIDF/ActualPosition_m", actualPosition);
    SmartDashboard.putNumber("ElevatorPIDF/ProfiledPosition_m", profiledPosition);
    SmartDashboard.putNumber("ElevatorPIDF/DesiredPosition_m", desiredPosition);
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
