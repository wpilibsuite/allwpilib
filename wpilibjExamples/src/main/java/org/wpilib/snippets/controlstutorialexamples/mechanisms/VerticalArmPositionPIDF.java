// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.snippets.controlstutorialexamples.mechanisms;

import org.wpilib.drivers.motor.PWMSparkMax;
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

/** Simple vertical arm position PIDF example showing gravity compensation and simulation. */
public class VerticalArmPositionPIDF implements AutoCloseable {
  // Physical mechanism constants
  double kGearing = 100.0;
  double kArmLength = 1.2; // meters
  double kArmMass = 5.0; // kg

  // Tuned controller constants - tune these like in the tutorial
  double kP = 40.0; // Feedback Proportional gain
  double kI = 0.0; // Feedback Integral gain
  double kD = 0.5; // Feedback Derivative gain
  double kG = 0.36; // Gravity-compensation gain

  // Electronics hardware
  int kMotorPort = 3;
  int kEncoderAChannel = 6;
  int kEncoderBChannel = 7;
  double kVerticalArmRadiansPerEncoderPulse = 2.0 * Math.PI / 2048.0;

  // Hardware
  DCMotor armMotor;
  Encoder encoder;
  PWMSparkMax motor;

  // Controls helpers
  PIDController controller;

  // Simulation support
  SingleJointedArmSim armSim;
  EncoderSim encoderSim;
  PWMMotorControllerSim motorSim;
  // Simulation sensor filters
  LinearFilter angleFilter;

  // State variables
  double desiredPosition = 0.0;
  double voltage = 0.0;
  double actualPosition = 0.0;

  /** Constructor: set up encoder, motor, and PID controller for the vertical arm. */
  public VerticalArmPositionPIDF() {
    // Set up quadrature encoder for position measurement
    encoder = new Encoder(kEncoderAChannel, kEncoderBChannel);
    encoder.setDistancePerPulse(kVerticalArmRadiansPerEncoderPulse);

    // Set up SPARK PWM motor controller
    motor = new PWMSparkMax(kMotorPort);

    // Set up WPILib's built-in PID controller for position control
    controller = new PIDController(kP, kI, kD);
  }

  // Initialize simulation components
  /**
   * Initializes the simulation components for this mechanism. This method should be called when
   * running in simulation mode to set up the physics simulation models for the vertical arm
   * mechanism.
   */
  public void initializeSimulation() {
    // Set up CIM motor model for simulation
    armMotor = DCMotor.getKrakenX60(2);

    // Set up simulation model for the vertical arm mechanism
    armSim =
        new SingleJointedArmSim(
            armMotor,
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
    encoderSim = new EncoderSim(encoder);

    // Create sensor filter for arm angle
    angleFilter = LinearFilter.singlePoleIIR(0.05, 0.02);

    // Set up simulation model for the motor controller
    motorSim = new PWMMotorControllerSim(motor);
  }

  /**
   * Updates the vertical arm control loop. This method should be called periodically (typically
   * every 20ms) to: 1. Read the current position from the encoder 2. Calculate the PID +
   * feedforward control output 3. Send the calculated voltage to the motor
   */
  public void update() {
    //////////////////////////////////////////////////
    // Step 1: Read Sensors
    actualPosition = encoder.getDistance();

    //////////////////////////////////////////////////
    // Step 2: Calculate

    // Gravity compensation proportional to cosine of the angle (0 = straight out)
    double gravityComp = kG * Math.cos(actualPosition);

    // Feedback Control
    double pidOutput = controller.calculate(actualPosition, desiredPosition);

    // Total control effort is sum of feedforward and feedback
    voltage = pidOutput + gravityComp;

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
   * Sets the desired position setpoint for the vertical arm.
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
    if (armSim != null) {
      armSim.setInput(motorSim.getThrottle() * RobotController.getBatteryVoltage());
      armSim.update(0.020);
      double filteredAngle = angleFilter.calculate(armSim.getAngle());
      encoderSim.setDistance(filteredAngle);
      RoboRioSim.setVInVoltage(
          BatterySim.calculateDefaultBatteryLoadedVoltage(armSim.getCurrentDraw()));
    }
  }

  /**
   * Updates telemetry data to SmartDashboard. This method should be called periodically to publish
   * mechanism state information for debugging and monitoring.
   */
  public void updateTelemetry() {
    SmartDashboard.putNumber("VerticalArmPositionPIDF/MotorVoltage_V", voltage);
    SmartDashboard.putNumber(
        "VerticalArmPositionPIDF/ActualPosition_degrees", Units.radiansToDegrees(actualPosition));
    SmartDashboard.putNumber(
        "VerticalArmPositionPIDF/DesiredPosition_degrees",
        Units.radiansToDegrees(desiredPosition));
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
