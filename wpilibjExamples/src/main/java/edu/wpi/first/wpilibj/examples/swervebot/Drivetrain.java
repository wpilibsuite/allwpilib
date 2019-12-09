/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.examples.swervebot;

import edu.wpi.first.wpilibj.AnalogGyro;
import edu.wpi.first.wpilibj.geometry.Rotation2d;
import edu.wpi.first.wpilibj.kinematics.ChassisSpeeds;
import edu.wpi.first.wpilibj.kinematics.SwerveDriveKinematics;
import edu.wpi.first.wpilibj.kinematics.SwerveDriveOdometry;

import static edu.wpi.first.wpilibj.examples.swervebot.Constants.DriveConstants.kBackLeftDriveEncoderPorts;
import static edu.wpi.first.wpilibj.examples.swervebot.Constants.DriveConstants.kBackLeftDriveMotorPort;
import static edu.wpi.first.wpilibj.examples.swervebot.Constants.DriveConstants.kBackLeftLocation;
import static edu.wpi.first.wpilibj.examples.swervebot.Constants.DriveConstants.kBackLeftTurningEncoderPorts;
import static edu.wpi.first.wpilibj.examples.swervebot.Constants.DriveConstants.kBackLeftTurningMotorPort;
import static edu.wpi.first.wpilibj.examples.swervebot.Constants.DriveConstants.kBackRightDriveEncoderPorts;
import static edu.wpi.first.wpilibj.examples.swervebot.Constants.DriveConstants.kBackRightDriveMotorPort;
import static edu.wpi.first.wpilibj.examples.swervebot.Constants.DriveConstants.kBackRightLocation;
import static edu.wpi.first.wpilibj.examples.swervebot.Constants.DriveConstants.kBackRightTurningEncoderPorts;
import static edu.wpi.first.wpilibj.examples.swervebot.Constants.DriveConstants.kBackRightTurningMotorPort;
import static edu.wpi.first.wpilibj.examples.swervebot.Constants.DriveConstants.kFrontLeftDriveEncoderPorts;
import static edu.wpi.first.wpilibj.examples.swervebot.Constants.DriveConstants.kFrontLeftDriveMotorPort;
import static edu.wpi.first.wpilibj.examples.swervebot.Constants.DriveConstants.kFrontLeftLocation;
import static edu.wpi.first.wpilibj.examples.swervebot.Constants.DriveConstants.kFrontLeftTurningEncoderPorts;
import static edu.wpi.first.wpilibj.examples.swervebot.Constants.DriveConstants.kFrontLeftTurningMotorPort;
import static edu.wpi.first.wpilibj.examples.swervebot.Constants.DriveConstants.kFrontRightDriveEncoderPorts;
import static edu.wpi.first.wpilibj.examples.swervebot.Constants.DriveConstants.kFrontRightDriveMotorPort;
import static edu.wpi.first.wpilibj.examples.swervebot.Constants.DriveConstants.kFrontRightLocation;
import static edu.wpi.first.wpilibj.examples.swervebot.Constants.DriveConstants.kFrontRightTurningEncoderPorts;
import static edu.wpi.first.wpilibj.examples.swervebot.Constants.DriveConstants.kFrontRightTurningMotorPort;
import static edu.wpi.first.wpilibj.examples.swervebot.Constants.DriveConstants.kMaxSpeed;

/**
 * Represents a swerve drive style drivetrain.
 */
@SuppressWarnings("PMD.ExcessiveImports")
public class Drivetrain {
  private final SwerveModule m_frontLeft = new SwerveModule(kFrontLeftDriveMotorPort,
      kFrontLeftTurningMotorPort,
      kFrontLeftDriveEncoderPorts,
      kFrontLeftTurningEncoderPorts);
  private final SwerveModule m_frontRight = new SwerveModule(kFrontRightDriveMotorPort,
      kFrontRightTurningMotorPort,
      kFrontRightDriveEncoderPorts,
      kFrontRightTurningEncoderPorts);
  private final SwerveModule m_backLeft = new SwerveModule(kBackLeftDriveMotorPort,
      kBackLeftTurningMotorPort,
      kBackLeftDriveEncoderPorts,
      kBackLeftTurningEncoderPorts);
  private final SwerveModule m_backRight = new SwerveModule(kBackRightDriveMotorPort,
      kBackRightTurningMotorPort,
      kBackRightDriveEncoderPorts,
      kBackRightTurningEncoderPorts);

  private final AnalogGyro m_gyro = new AnalogGyro(0);

  private final SwerveDriveKinematics m_kinematics = new SwerveDriveKinematics(
      kFrontLeftLocation, kFrontRightLocation, kBackLeftLocation, kBackRightLocation
  );

  private final SwerveDriveOdometry m_odometry = new SwerveDriveOdometry(m_kinematics, getAngle());

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
   * Method to drive the robot using joystick info.
   *
   * @param xSpeed        Speed of the robot in the x direction (forward).
   * @param ySpeed        Speed of the robot in the y direction (sideways).
   * @param rot           Angular rate of the robot.
   * @param fieldRelative Whether the provided x and y speeds are relative to the field.
   */
  @SuppressWarnings("ParameterName")
  public void drive(double xSpeed, double ySpeed, double rot, boolean fieldRelative) {
    var swerveModuleStates = m_kinematics.toSwerveModuleStates(
        fieldRelative ? ChassisSpeeds.fromFieldRelativeSpeeds(
            xSpeed, ySpeed, rot, getAngle())
            : new ChassisSpeeds(xSpeed, ySpeed, rot)
    );
    SwerveDriveKinematics.normalizeWheelSpeeds(swerveModuleStates, kMaxSpeed);
    m_frontLeft.setDesiredState(swerveModuleStates[0]);
    m_frontRight.setDesiredState(swerveModuleStates[1]);
    m_backLeft.setDesiredState(swerveModuleStates[2]);
    m_backRight.setDesiredState(swerveModuleStates[3]);
  }

  /**
   * Updates the field relative position of the robot.
   */
  public void updateOdometry() {
    m_odometry.update(
        getAngle(),
        m_frontLeft.getState(),
        m_frontRight.getState(),
        m_backLeft.getState(),
        m_backRight.getState()
    );
  }
}
