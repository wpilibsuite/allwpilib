// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <numbers>

#include "wpi/hardware/imu/OnboardIMU.hpp"
#include "wpi/hardware/motor/PWMSparkMax.hpp"
#include "wpi/hardware/rotation/Encoder.hpp"
#include "wpi/math/controller/PIDController.hpp"
#include "wpi/math/controller/SimpleMotorFeedforward.hpp"
#include "wpi/math/estimator/MecanumDrivePoseEstimator.hpp"
#include "wpi/math/geometry/Translation2d.hpp"
#include "wpi/math/kinematics/MecanumDriveKinematics.hpp"
#include "wpi/math/kinematics/MecanumDriveOdometry.hpp"
#include "wpi/math/kinematics/MecanumDriveWheelSpeeds.hpp"

/**
 * Represents a mecanum drive style drivetrain.
 */
class Drivetrain {
 public:
  Drivetrain() {
    m_imu.ResetYaw();
    // We need to invert one side of the drivetrain so that positive voltages
    // result in both sides moving forward. Depending on how your robot's
    // gearbox is constructed, you might have to invert the left side instead.
    m_frontRightMotor.SetInverted(true);
    m_backRightMotor.SetInverted(true);
  }

  wpi::math::MecanumDriveWheelSpeeds GetCurrentState() const;
  wpi::math::MecanumDriveWheelPositions GetCurrentDistances() const;
  void SetSpeeds(const wpi::math::MecanumDriveWheelSpeeds& wheelSpeeds);
  void Drive(wpi::units::meters_per_second_t xSpeed,
             wpi::units::meters_per_second_t ySpeed,
             wpi::units::radians_per_second_t rot, bool fieldRelative,
             wpi::units::second_t period);
  void UpdateOdometry();

  static constexpr auto kMaxSpeed = 3.0_mps;  // 3 meters per second
  static constexpr wpi::units::radians_per_second_t kMaxAngularSpeed{
      std::numbers::pi};  // 1/2 rotation per second

 private:
  wpi::PWMSparkMax m_frontLeftMotor{1};
  wpi::PWMSparkMax m_frontRightMotor{2};
  wpi::PWMSparkMax m_backLeftMotor{3};
  wpi::PWMSparkMax m_backRightMotor{4};

  wpi::Encoder m_frontLeftEncoder{0, 1};
  wpi::Encoder m_frontRightEncoder{2, 3};
  wpi::Encoder m_backLeftEncoder{4, 5};
  wpi::Encoder m_backRightEncoder{6, 7};

  wpi::math::Translation2d m_frontLeftLocation{0.381_m, 0.381_m};
  wpi::math::Translation2d m_frontRightLocation{0.381_m, -0.381_m};
  wpi::math::Translation2d m_backLeftLocation{-0.381_m, 0.381_m};
  wpi::math::Translation2d m_backRightLocation{-0.381_m, -0.381_m};

  wpi::math::PIDController m_frontLeftPIDController{1.0, 0.0, 0.0};
  wpi::math::PIDController m_frontRightPIDController{1.0, 0.0, 0.0};
  wpi::math::PIDController m_backLeftPIDController{1.0, 0.0, 0.0};
  wpi::math::PIDController m_backRightPIDController{1.0, 0.0, 0.0};

  wpi::OnboardIMU m_imu{wpi::OnboardIMU::kFlat};

  wpi::math::MecanumDriveKinematics m_kinematics{
      m_frontLeftLocation, m_frontRightLocation, m_backLeftLocation,
      m_backRightLocation};

  // Gains are for example purposes only - must be determined for your own
  // robot!
  wpi::math::SimpleMotorFeedforward<wpi::units::meters> m_feedforward{
      1_V, 3_V / 1_mps};

  // Gains are for example purposes only - must be determined for your own
  // robot!
  wpi::math::MecanumDrivePoseEstimator m_poseEstimator{
      m_kinematics,        m_imu.GetRotation2d(), GetCurrentDistances(),
      wpi::math::Pose2d{}, {0.1, 0.1, 0.1},       {0.1, 0.1, 0.1}};
};
