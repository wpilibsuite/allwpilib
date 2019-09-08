/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.examples.mecanumbot;

import edu.wpi.first.wpilibj.AnalogGyro;
import edu.wpi.first.wpilibj.Encoder;
import edu.wpi.first.wpilibj.Spark;
import edu.wpi.first.wpilibj.controller.PIDController;
import edu.wpi.first.wpilibj.geometry.Rotation2d;
import edu.wpi.first.wpilibj.geometry.Translation2d;
import edu.wpi.first.wpilibj.kinematics.ChassisSpeeds;
import edu.wpi.first.wpilibj.kinematics.MecanumDriveKinematics;
import edu.wpi.first.wpilibj.kinematics.MecanumDriveOdometry;
import edu.wpi.first.wpilibj.kinematics.MecanumDriveWheelSpeeds;

/**
 * Represents a mecanum drive style drivetrain.
 */
@SuppressWarnings("PMD.TooManyFields")
public class Drivetrain {
  public static final double kMaxSpeed = 3.0; // 3 meters per second
  public static final double kMaxAngularSpeed = Math.PI; // 1/2 rotation per second

  private final Spark m_frontLeftMotor = new Spark(1);
  private final Spark m_frontRightMotor = new Spark(2);
  private final Spark m_backLeftMotor = new Spark(3);
  private final Spark m_backRightMotor = new Spark(4);

  private final Encoder m_frontLeftEncoder = new Encoder(0, 1);
  private final Encoder m_frontRightEncoder = new Encoder(0, 1);
  private final Encoder m_backLeftEncoder = new Encoder(0, 1);
  private final Encoder m_backRightEncoder = new Encoder(0, 1);

  private final Translation2d m_frontLeftLocation = new Translation2d(0.381, 0.381);
  private final Translation2d m_frontRightLocation = new Translation2d(0.381, -0.381);
  private final Translation2d m_backLeftLocation = new Translation2d(-0.381, 0.381);
  private final Translation2d m_backRightLocation = new Translation2d(-0.381, -0.381);

  private final PIDController m_frontLeftPIDController = new PIDController(1, 0, 0);
  private final PIDController m_frontRightPIDController = new PIDController(1, 0, 0);
  private final PIDController m_backLeftPIDController = new PIDController(1, 0, 0);
  private final PIDController m_backRightPIDController = new PIDController(1, 0, 0);

  private final AnalogGyro m_gyro = new AnalogGyro(0);

  private final MecanumDriveKinematics m_kinematics = new MecanumDriveKinematics(
      m_frontLeftLocation, m_frontRightLocation, m_backLeftLocation, m_backRightLocation
  );

  private final MecanumDriveOdometry m_odometry = new MecanumDriveOdometry(m_kinematics);

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
