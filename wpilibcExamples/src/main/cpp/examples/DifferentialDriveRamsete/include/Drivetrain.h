/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <units/units.h>

#include <frc/AnalogGyro.h>
#include <frc/Encoder.h>
#include <frc/Spark.h>
#include <frc/SpeedControllerGroup.h>
#include <frc/controller/PIDController.h>
#include <frc/kinematics/DifferentialDriveKinematics.h>
#include <frc/kinematics/DifferentialDriveOdometry.h>
#include <frc2/command/SubsystemBase.h>
#include <wpi/math>

/**
 * Represents a differential drive style drivetrain.
 */
class Drivetrain : public frc2::SubsystemBase {
 public:
  Drivetrain() {
    m_gyro.Reset();
    // Set the distance per pulse for the drive encoders. We can simply use the
    // distance traveled for one rotation of the wheel divided by the encoder
    // resolution.
    m_leftEncoder.SetDistancePerPulse(2 * wpi::math::pi * kWheelRadius /
                                      kEncoderResolution);
    m_rightEncoder.SetDistancePerPulse(2 * wpi::math::pi * kWheelRadius /
                                       kEncoderResolution);
  }

  /**
   * Get the robot angle as a Rotation2d.
   */
  frc::Rotation2d GetAngle() const {
    // Negating the angle because WPILib Gyros are CW positive.
    return frc::Rotation2d(units::degree_t(-m_gyro.GetAngle()));
  }

  /**
   * Get the current pose of the robot.
   */
  const frc::Pose2d& GetPose() const { return m_pose; }

  static constexpr units::meters_per_second_t kMaxSpeed =
      3.0_mps;  // 3 meters per second
  static constexpr units::radians_per_second_t kMaxAngularSpeed{
      wpi::math::pi};  // 1/2 rotation per second

  frc::DifferentialDriveWheelSpeeds GetSpeeds() const;
  void SetSpeeds(const frc::DifferentialDriveWheelSpeeds& speeds);
  void SetSpeeds(const frc::ChassisSpeeds& speeds);
  void Periodic() override;

  void Reset(const frc::Pose2d& pose) {
    m_gyro.Reset();
    m_odometry.ResetPosition(pose);
  }

 private:
  static constexpr units::meter_t kTrackWidth = 0.381_m * 2;
  static constexpr double kWheelRadius = 0.0508;  // meters
  static constexpr int kEncoderResolution = 4096;

  // Motors
  frc::Spark m_leftMaster{1};
  frc::Spark m_leftFollower{2};
  frc::Spark m_rightMaster{3};
  frc::Spark m_rightFollower{4};

  // Speed Controller Groups
  frc::SpeedControllerGroup m_leftGroup{m_leftMaster, m_leftFollower};
  frc::SpeedControllerGroup m_rightGroup{m_rightMaster, m_rightFollower};

  // Encoders
  frc::Encoder m_leftEncoder{0, 1};
  frc::Encoder m_rightEncoder{2, 3};

  // PID Controllers to track individual velocities
  frc2::PIDController m_leftPIDController{1.0, 0.0, 0.0};
  frc2::PIDController m_rightPIDController{1.0, 0.0, 0.0};

  // Gyro
  frc::AnalogGyro m_gyro{0};

  // Kinematics object to convert between chassis speeds and wheel speeds
  frc::DifferentialDriveKinematics m_kinematics{kTrackWidth};

  // Odometry to track field relative pose.
  frc::DifferentialDriveOdometry m_odometry{m_kinematics};

  // The current pose.
  frc::Pose2d m_pose;
};
