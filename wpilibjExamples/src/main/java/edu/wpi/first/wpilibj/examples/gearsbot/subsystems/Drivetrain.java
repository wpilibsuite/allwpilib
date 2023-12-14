// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.examples.gearsbot.subsystems;

import static edu.wpi.first.wpilibj.examples.gearsbot.Constants.DriveConstants.*;

import edu.wpi.first.math.controller.PIDController;
import edu.wpi.first.wpilibj.AnalogGyro;
import edu.wpi.first.wpilibj.AnalogInput;
import edu.wpi.first.wpilibj.Encoder;
import edu.wpi.first.wpilibj.drive.DifferentialDrive;
import edu.wpi.first.wpilibj.examples.gearsbot.Robot;
import edu.wpi.first.wpilibj.motorcontrol.MotorController;
import edu.wpi.first.wpilibj.motorcontrol.MotorControllerGroup;
import edu.wpi.first.wpilibj.motorcontrol.PWMSparkMax;
import edu.wpi.first.wpilibj.smartdashboard.SmartDashboard;
import edu.wpi.first.wpilibj2.command.Command;
import edu.wpi.first.wpilibj2.command.SubsystemBase;
import java.util.function.DoubleSupplier;

/**
 * The Drivetrain subsystem incorporates the sensors and actuators attached to the robots chassis.
 * These include four drive motors, a left and right encoder and a gyro.
 */
public class Drivetrain extends SubsystemBase {
  private final MotorController m_leftMotor =
      new MotorControllerGroup(new PWMSparkMax(kLeftMotorPort1), new PWMSparkMax(kLeftMotorPort1));

  private final MotorController m_rightMotor =
      new MotorControllerGroup(new PWMSparkMax(kRightMotorPort2), new PWMSparkMax(kLeftMotorPort2));

  private final DifferentialDrive m_drive = new DifferentialDrive(m_leftMotor, m_rightMotor);

  private final Encoder m_leftEncoder =
      new Encoder(kLeftEncoderPorts[0], kLeftEncoderPorts[1], kLeftEncoderReversed);
  private final Encoder m_rightEncoder =
      new Encoder(kRightEncoderPorts[0], kRightEncoderPorts[1], kRightEncoderReversed);
  private final AnalogInput m_rangefinder = new AnalogInput(kRangeFinderPort);
  private final AnalogGyro m_gyro = new AnalogGyro(kAnalogGyroPort);

  /** Create a new drivetrain subsystem. */
  public Drivetrain() {
    // We need to invert one side of the drivetrain so that positive voltages
    // result in both sides moving forward. Depending on how your robot's
    // gearbox is constructed, you might have to invert the left side instead.
    m_rightMotor.setInverted(true);

    // Encoders may measure differently in the real world and in
    // simulation. In this example the robot moves 0.042 barleycorns
    // per tick in the real world, but the simulated encoders
    // simulate 360 tick encoders. This if statement allows for the
    // real robot to handle this difference in devices.
    if (Robot.isReal()) {
      m_leftEncoder.setDistancePerPulse(kEncoderDistancePerPulse);
      m_rightEncoder.setDistancePerPulse(kEncoderDistancePerPulse);
    } else {
      // Circumference = diameter in feet * pi. 360 tick simulated encoders.
      m_leftEncoder.setDistancePerPulse((4.0 / 12.0 * Math.PI) / 360.0);
      m_rightEncoder.setDistancePerPulse((4.0 / 12.0 * Math.PI) / 360.0);
    }

    // Let's name the sensors on the LiveWindow
    addChild("Drive", m_drive);
    addChild("Left Encoder", m_leftEncoder);
    addChild("Right Encoder", m_rightEncoder);
    addChild("Rangefinder", m_rangefinder);
    addChild("Gyro", m_gyro);
  }

  /**
   * Tank style driving for the Drivetrain.
   *
   * @param left Speed in range [-1,1]
   * @param right Speed in range [-1,1]
   */
  public void drive(double left, double right) {
    m_drive.tankDrive(left, right);
  }

  /**
   * Drives the robot tank style.
   *
   * @param left The control input for the left side of the drive
   * @param right The control input for the right sight of the drive
   * @return The tank drive command.
   */
  public Command tankDrive(DoubleSupplier left, DoubleSupplier right) {
    return run(() -> drive(left.getAsDouble(), right.getAsDouble()))
        .finallyDo(() -> drive(0, 0))
        .withName("driving");
  }

  /**
   * Get the robot's heading.
   *
   * @return The robots heading in degrees.
   */
  public double getHeading() {
    return m_gyro.getAngle();
  }

  /** Reset the robots sensors to the zero states. */
  public void reset() {
    m_gyro.reset();
    m_leftEncoder.reset();
    m_rightEncoder.reset();
  }

  /**
   * Get the average distance of the encoders since the last reset.
   *
   * @return The distance driven (average of left and right encoders).
   */
  public double getDistance() {
    return (m_leftEncoder.getDistance() + m_rightEncoder.getDistance()) / 2;
  }

  /**
   * Get the distance to the obstacle.
   *
   * @return The distance to the obstacle detected by the rangefinder.
   */
  public double getDistanceToObstacle() {
    // Really meters in simulation since it's a rangefinder...
    return m_rangefinder.getAverageVoltage();
  }

  /**
   * Drive until the robot is the given distance away from the box. Uses a local PID controller to
   * run a simple PID loop that is only enabled while this command is running. The input is the
   * averaged values of the left and right encoders.
   *
   * @param distance The distance away from the box to drive to
   * @return A command that drives the robot to the provided distance based on the rangefinder.
   */
  public Command driveDistanceFromObstacle(double distance) {
    return driveDistance(-distance, () -> -getDistanceToObstacle());
  }

  /**
   * Drive the given distance straight (negative values go backwards). Uses a local PID controller
   * to run a simple PID loop that is only enabled while this command is running. The input is the
   * averaged values of the left and right encoders.
   *
   * @param distance The distance to drive
   * @return The command to drive straight
   */
  public Command driveDistanceEncoders(double distance) {
    return driveDistance(distance, this::getDistance);
  }

  /**
   * A private factory for commands that drive a specific distance based on sensor inputs.
   *
   * @param setpoint The setpoint
   * @param measurement The measurement
   * @return
   */
  @SuppressWarnings("resource")
  private Command driveDistance(double setpoint, DoubleSupplier measurement) {
    PIDController controller = new PIDController(PositionPID.kP, PositionPID.kI, PositionPID.kD);
    controller.setSetpoint(setpoint);
    controller.setTolerance(0.01);

    return runOnce(this::reset)
        .andThen(
            run(
                () -> {
                  double out = controller.calculate(measurement.getAsDouble());
                  drive(out, out);
                }))
        .until(controller::atSetpoint)
        .withName("drive distance");
  }

  /** The log method puts interesting information to the SmartDashboard. */
  public void log() {
    SmartDashboard.putNumber("Left Distance", m_leftEncoder.getDistance());
    SmartDashboard.putNumber("Right Distance", m_rightEncoder.getDistance());
    SmartDashboard.putNumber("Left Speed", m_leftEncoder.getRate());
    SmartDashboard.putNumber("Right Speed", m_rightEncoder.getRate());
    SmartDashboard.putNumber("Gyro", m_gyro.getAngle());
  }

  /** Call log method every loop. */
  @Override
  public void periodic() {
    log();
  }
}
