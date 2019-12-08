/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.examples.mecanumbot;

import edu.wpi.first.wpilibj.AnalogGyro;
import edu.wpi.first.wpilibj.Encoder;
import edu.wpi.first.wpilibj.PWMVictorSPX;
import edu.wpi.first.wpilibj.SpeedController;
import edu.wpi.first.wpilibj.controller.PIDController;
import edu.wpi.first.wpilibj.geometry.Rotation2d;
import edu.wpi.first.wpilibj.kinematics.ChassisSpeeds;
import edu.wpi.first.wpilibj.kinematics.MecanumDriveKinematics;
import edu.wpi.first.wpilibj.kinematics.MecanumDriveOdometry;
import edu.wpi.first.wpilibj.kinematics.MecanumDriveWheelSpeeds;

import static edu.wpi.first.wpilibj.examples.mecanumbot.Constants.DriveConstants.kBackLeftEncoderPorts;
import static edu.wpi.first.wpilibj.examples.mecanumbot.Constants.DriveConstants.kBackLeftEncoderReversed;
import static edu.wpi.first.wpilibj.examples.mecanumbot.Constants.DriveConstants.kBackLeftLocation;
import static edu.wpi.first.wpilibj.examples.mecanumbot.Constants.DriveConstants.kBackLeftMotorPort;
import static edu.wpi.first.wpilibj.examples.mecanumbot.Constants.DriveConstants.kBackRightEncoderPorts;
import static edu.wpi.first.wpilibj.examples.mecanumbot.Constants.DriveConstants.kBackRightEncoderReversed;
import static edu.wpi.first.wpilibj.examples.mecanumbot.Constants.DriveConstants.kBackRightLocation;
import static edu.wpi.first.wpilibj.examples.mecanumbot.Constants.DriveConstants.kBackRightMotorPort;
import static edu.wpi.first.wpilibj.examples.mecanumbot.Constants.DriveConstants.kFrontLeftEncoderPorts;
import static edu.wpi.first.wpilibj.examples.mecanumbot.Constants.DriveConstants.kFrontLeftEncoderReversed;
import static edu.wpi.first.wpilibj.examples.mecanumbot.Constants.DriveConstants.kFrontLeftLocation;
import static edu.wpi.first.wpilibj.examples.mecanumbot.Constants.DriveConstants.kFrontLeftMotorPort;
import static edu.wpi.first.wpilibj.examples.mecanumbot.Constants.DriveConstants.kFrontRightEncoderPorts;
import static edu.wpi.first.wpilibj.examples.mecanumbot.Constants.DriveConstants.kFrontRightEncoderReversed;
import static edu.wpi.first.wpilibj.examples.mecanumbot.Constants.DriveConstants.kFrontRightLocation;
import static edu.wpi.first.wpilibj.examples.mecanumbot.Constants.DriveConstants.kFrontRightMotorPort;
import static edu.wpi.first.wpilibj.examples.mecanumbot.Constants.DriveConstants.kMaxSpeed;
import static edu.wpi.first.wpilibj.examples.mecanumbot.Constants.DriveConstants.kPbackLeftController;
import static edu.wpi.first.wpilibj.examples.mecanumbot.Constants.DriveConstants.kPbackRightController;
import static edu.wpi.first.wpilibj.examples.mecanumbot.Constants.DriveConstants.kPfrontLeftController;
import static edu.wpi.first.wpilibj.examples.mecanumbot.Constants.DriveConstants.kPfrontRightController;

/**
 * Represents a mecanum drive style drivetrain.
 */
@SuppressWarnings({"PMD.TooManyFields", "PMD.ExcessiveImports"})
public class Drivetrain {
  private final SpeedController m_frontLeftMotor = new PWMVictorSPX(kFrontLeftMotorPort);
  private final SpeedController m_frontRightMotor = new PWMVictorSPX(kFrontRightMotorPort);
  private final SpeedController m_backLeftMotor = new PWMVictorSPX(kBackLeftMotorPort);
  private final SpeedController m_backRightMotor = new PWMVictorSPX(kBackRightMotorPort);

  private final Encoder m_frontLeftEncoder = new Encoder(
      kFrontLeftEncoderPorts[0], kFrontLeftEncoderPorts[1], kFrontLeftEncoderReversed);
  private final Encoder m_frontRightEncoder = new Encoder(
      kFrontRightEncoderPorts[0], kFrontRightEncoderPorts[1], kFrontRightEncoderReversed);
  private final Encoder m_backLeftEncoder = new Encoder(
      kBackLeftEncoderPorts[0], kBackLeftEncoderPorts[1], kBackLeftEncoderReversed);
  private final Encoder m_backRightEncoder = new Encoder(
      kBackRightEncoderPorts[0], kBackRightEncoderPorts[1], kBackRightEncoderReversed);

  private final PIDController m_frontLeftPIDController = new PIDController(
        kPfrontLeftController, 0, 0);
  private final PIDController m_frontRightPIDController = new PIDController(
        kPfrontRightController, 0, 0);
  private final PIDController m_backLeftPIDController = new PIDController(
        kPbackLeftController, 0, 0);
  private final PIDController m_backRightPIDController = new PIDController(
        kPbackRightController, 0, 0);

  private final AnalogGyro m_gyro = new AnalogGyro(0);

  private final MecanumDriveKinematics m_kinematics = new MecanumDriveKinematics(
      kFrontLeftLocation, kFrontRightLocation, kBackLeftLocation, kBackRightLocation
  );

  private final MecanumDriveOdometry m_odometry = new MecanumDriveOdometry(m_kinematics,
      getAngle());

  /**
   * Constructs a MecanumDrive and resets the gyro.
   */
  public Drivetrain() {
    m_gyro.reset();
  }

  /**
   * Returns the angle of the robot as a Rotation2d.
   *
   * @return The angle of the robot.
   */
  public Rotation2d getAngle() {
    // Negating the angle because WPILib gyros are CW positive.
    return Rotation2d.fromDegrees(-m_gyro.getAngle());
  }

  /**
   * Returns the current state of the drivetrain.
   *
   * @return The current state of the drivetrain.
   */
  public MecanumDriveWheelSpeeds getCurrentState() {
    return new MecanumDriveWheelSpeeds(
        m_frontLeftEncoder.getRate(),
        m_frontRightEncoder.getRate(),
        m_backLeftEncoder.getRate(),
        m_backRightEncoder.getRate()
    );
  }

  /**
   * Set the desired speeds for each wheel.
   *
   * @param speeds The desired wheel speeds.
   */
  public void setSpeeds(MecanumDriveWheelSpeeds speeds) {
    final var frontLeftOutput = m_frontLeftPIDController.calculate(
        m_frontLeftEncoder.getRate(), speeds.frontLeftMetersPerSecond
    );
    final var frontRightOutput = m_frontRightPIDController.calculate(
        m_frontRightEncoder.getRate(), speeds.frontRightMetersPerSecond
    );
    final var backLeftOutput = m_backLeftPIDController.calculate(
        m_backLeftEncoder.getRate(), speeds.rearLeftMetersPerSecond
    );
    final var backRightOutput = m_backRightPIDController.calculate(
        m_backRightEncoder.getRate(), speeds.rearRightMetersPerSecond
    );

    m_frontLeftMotor.set(frontLeftOutput);
    m_frontRightMotor.set(frontRightOutput);
    m_backLeftMotor.set(backLeftOutput);
    m_backRightMotor.set(backRightOutput);
  }

  /**
   * Method to drive the robot using joystick info.
   *
   * @param xSpeed        Speed of the robot in the x direction (forward).
   * @param ySpeed        Speed of the robot in the y direction (sideways).
   * @param rot           Angular rate of the robot.
   * @param fieldRelative Whether the provided x and y speeds are relative to the field.
   */
  @SuppressWarnings("ParameterName")
  public void drive(double xSpeed, double ySpeed, double rot, boolean fieldRelative) {
    var mecanumDriveWheelSpeeds = m_kinematics.toWheelSpeeds(
        fieldRelative ? ChassisSpeeds.fromFieldRelativeSpeeds(
            xSpeed, ySpeed, rot, getAngle()
        ) : new ChassisSpeeds(xSpeed, ySpeed, rot)
    );
    mecanumDriveWheelSpeeds.normalize(kMaxSpeed);
    setSpeeds(mecanumDriveWheelSpeeds);
  }

  /**
   * Updates the field relative position of the robot.
   */
  public void updateOdometry() {
    m_odometry.update(getAngle(), getCurrentState());
  }
}
