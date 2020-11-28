/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <frc/AnalogGyro.h>
#include <frc/Encoder.h>
#include <frc/PWMVictorSPX.h>
#include <frc/controller/PIDController.h>
#include <frc/controller/SimpleMotorFeedforward.h>
#include <frc/estimator/MecanumDrivePoseEstimator.h>
#include <frc/geometry/Translation2d.h>
#include <frc/kinematics/MecanumDriveKinematics.h>
#include <frc/kinematics/MecanumDriveOdometry.h>
#include <frc/kinematics/MecanumDriveWheelSpeeds.h>
#include <wpi/math>

/**
 * Represents a mecanum drive style drivetrain.
 */
class Drivetrain {
 public:
  Drivetrain() { m_gyro.Reset(); }

  frc::MecanumDriveWheelSpeeds GetCurrentState() const;
  void SetSpeeds(const frc::MecanumDriveWheelSpeeds& wheelSpeeds);
  void Drive(units::meters_per_second_t xSpeed,
             units::meters_per_second_t ySpeed, units::radians_per_second_t rot,
             bool fieldRelative);
  void UpdateOdometry();

  static constexpr auto kMaxSpeed = 3.0_mps;  // 3 meters per second
  static constexpr units::radians_per_second_t kMaxAngularSpeed{
      wpi::math::pi};  // 1/2 rotation per second

 private:
  frc::PWMVictorSPX m_frontLeftMotor{1};
  frc::PWMVictorSPX m_frontRightMotor{2};
  frc::PWMVictorSPX m_backLeftMotor{3};
  frc::PWMVictorSPX m_backRightMotor{4};

  frc::Encoder m_frontLeftEncoder{0, 1};
  frc::Encoder m_frontRightEncoder{2, 3};
  frc::Encoder m_backLeftEncoder{4, 5};
  frc::Encoder m_backRightEncoder{6, 7};

  frc::Translation2d m_frontLeftLocation{0.381_m, 0.381_m};
  frc::Translation2d m_frontRightLocation{0.381_m, -0.381_m};
  frc::Translation2d m_backLeftLocation{-0.381_m, 0.381_m};
  frc::Translation2d m_backRightLocation{-0.381_m, -0.381_m};

  frc2::PIDController m_frontLeftPIDController{1.0, 0.0, 0.0};
  frc2::PIDController m_frontRightPIDController{1.0, 0.0, 0.0};
  frc2::PIDController m_backLeftPIDController{1.0, 0.0, 0.0};
  frc2::PIDController m_backRightPIDController{1.0, 0.0, 0.0};

  frc::AnalogGyro m_gyro{0};

  frc::MecanumDriveKinematics m_kinematics{
      m_frontLeftLocation, m_frontRightLocation, m_backLeftLocation,
      m_backRightLocation};

  // Gains are for example purposes only - must be determined for your own
  // robot!
  frc::SimpleMotorFeedforward<units::meters> m_feedforward{1_V, 3_V / 1_mps};

  // Gains are for example purposes only - must be determined for your own
  // robot!
  frc::MecanumDrivePoseEstimator m_poseEstimator{
      frc::Rotation2d(), frc::Pose2d(), m_kinematics,
      {0.1, 0.1, 0.1},   {0.05},        {0.1, 0.1, 0.1}};
};
