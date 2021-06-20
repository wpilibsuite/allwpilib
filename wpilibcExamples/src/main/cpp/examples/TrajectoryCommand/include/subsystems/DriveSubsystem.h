// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <frc/ADXRS450_Gyro.h>
#include <frc/Encoder.h>
#include <frc/controller/PIDController.h>
#include <frc/controller/RamseteController.h>
#include <frc/controller/SimpleMotorFeedforward.h>
#include <frc/drive/DifferentialDrive.h>
#include <frc/geometry/Pose2d.h>
#include <frc/kinematics/DifferentialDriveOdometry.h>
#include <frc/motorcontrol/MotorControllerGroup.h>
#include <frc/motorcontrol/PWMSparkMax.h>
#include <frc2/command/Command.h>
#include <frc2/command/SubsystemBase.h>
#include <units/voltage.h>

#include "Constants.h"

class DriveSubsystem : public frc2::SubsystemBase {
 public:
  DriveSubsystem();

  /**
   * Will be called periodically whenever the CommandScheduler runs.
   */
  void Periodic() override;

  // Subsystem methods go here.

  /**
   * Drives the robot using arcade controls.
   *
   * @param fwd The commanded forward movement
   * @param rot The commanded rotation
   */
  void ArcadeDrive(double fwd, double rot);

  /**
   * Reaches the given target state. Used internally to follow the
   * TrajectoryCommand's output.
   *
   * @param targetState The target state struct.
   */
  void FollowState(const frc::Trajectory::State& targetState);

  /**
   * Resets the drive encoders to currently read a position of 0.
   */
  void ResetEncoders();

  /**
   * Gets the average distance of the TWO encoders.
   *
   * @return The average of the TWO encoder readings
   */
  double GetAverageEncoderDistance();

  /**
   * Sets the max output of the drive.  Useful for scaling the drive to drive
   * more slowly.
   *
   * @param maxOutput The maximum output to which the drive will be constrained
   */
  void SetMaxOutput(double maxOutput);

  /**
   * Returns the heading of the robot.
   *
   * @return The robot's heading in degrees, from -180 to 180
   */
  units::degree_t GetHeading() const;

  /**
   * Returns the turn rate of the robot.
   *
   * @return The turn rate of the robot, in degrees per second
   */
  double GetTurnRate();

  /**
   * Returns the currently-estimated pose of the robot.
   *
   * @return The pose.
   */
  frc::Pose2d GetPose();

  /**
   * Resets the odometry to the specified pose.
   *
   * @param pose The pose to which to set the odometry.
   */
  void ResetOdometry(frc::Pose2d pose);

  /**
   * Build a command group for the given trajectory, which includes:
   *
   * <p>- resetting the odometry to the trajectory's initial pose
   *
   * <p>- following the trajectory
   *
   * <p>- stopping at the end of the path
   *
   * @param trajectory The path to follow
   * @return A command group that tracks the given trajectory
   */
  frc2::Command* BuildTrajectoryGroup(const frc::Trajectory& trajectory);

 private:
  // Components (e.g. motor controllers and sensors) should generally be
  // declared private and exposed only through public methods.

  // The motor controllers
  frc::PWMSparkMax m_left1;
  frc::PWMSparkMax m_left2;
  frc::PWMSparkMax m_right1;
  frc::PWMSparkMax m_right2;

  // The motors on the left side of the drive
  frc::MotorControllerGroup m_leftMotors{m_left1, m_left2};

  // The motors on the right side of the drive
  frc::MotorControllerGroup m_rightMotors{m_right1, m_right2};

  // The robot's drive
  frc::DifferentialDrive m_drive{m_leftMotors, m_rightMotors};

  // The left-side drive encoder
  frc::Encoder m_leftEncoder;

  // The right-side drive encoder
  frc::Encoder m_rightEncoder;

  // The gyro sensor
  frc::ADXRS450_Gyro m_gyro;

  // The RAMSETE controller
  frc::RamseteController m_ramseteController;

  // The PID controllers for each side
  frc::PIDController m_leftController;
  frc::PIDController m_rightController;

  // The feedforward
  // If the two sides of the drivetrain are different,
  // a separate feedforward object can be used for each side
  frc::SimpleMotorFeedforward<units::meters> m_feedforward;

  // Odometry class for tracking robot pose
  frc::DifferentialDriveOdometry m_odometry;

  // Track previous target velocities for feedforward calculation
  frc::DifferentialDriveWheelSpeeds m_previousSpeeds;
};
