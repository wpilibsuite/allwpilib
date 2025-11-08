// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <numbers>

#include "wpi/hardware/rotation/Encoder.hpp"
#include "wpi/hardware/imu/OnboardIMU.hpp"
#include "wpi/math/controller/PIDController.hpp"
#include "wpi/math/controller/SimpleMotorFeedforward.hpp"
#include "wpi/math/geometry/Translation2d.hpp"
#include "wpi/math/kinematics/MecanumDriveKinematics.hpp"
#include "wpi/math/kinematics/MecanumDriveOdometry.hpp"
#include "wpi/math/kinematics/MecanumDriveWheelSpeeds.hpp"
#include "wpi/hardware/motor/PWMSparkMax.hpp"

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

  frc::MecanumDriveWheelSpeeds GetCurrentState() const;
  frc::MecanumDriveWheelPositions GetCurrentWheelDistances() const;
  void SetSpeeds(const frc::MecanumDriveWheelSpeeds& wheelSpeeds);
  void Drive(units::meters_per_second_t xSpeed,
             units::meters_per_second_t ySpeed, units::radians_per_second_t rot,
             bool fieldRelative, units::second_t period);
  void UpdateOdometry();

  static constexpr units::meters_per_second_t kMaxSpeed =
      3.0_mps;  // 3 meters per second
  static constexpr units::radians_per_second_t kMaxAngularSpeed{
      std::numbers::pi};  // 1/2 rotation per second

 private:
  frc::PWMSparkMax m_frontLeftMotor{1};
  frc::PWMSparkMax m_frontRightMotor{2};
  frc::PWMSparkMax m_backLeftMotor{3};
  frc::PWMSparkMax m_backRightMotor{4};

  frc::Encoder m_frontLeftEncoder{0, 1};
  frc::Encoder m_frontRightEncoder{2, 3};
  frc::Encoder m_backLeftEncoder{4, 5};
  frc::Encoder m_backRightEncoder{6, 7};

  frc::Translation2d m_frontLeftLocation{0.381_m, 0.381_m};
  frc::Translation2d m_frontRightLocation{0.381_m, -0.381_m};
  frc::Translation2d m_backLeftLocation{-0.381_m, 0.381_m};
  frc::Translation2d m_backRightLocation{-0.381_m, -0.381_m};

  frc::PIDController m_frontLeftPIDController{1.0, 0.0, 0.0};
  frc::PIDController m_frontRightPIDController{1.0, 0.0, 0.0};
  frc::PIDController m_backLeftPIDController{1.0, 0.0, 0.0};
  frc::PIDController m_backRightPIDController{1.0, 0.0, 0.0};

  frc::OnboardIMU m_imu{frc::OnboardIMU::kFlat};

  frc::MecanumDriveKinematics m_kinematics{
      m_frontLeftLocation, m_frontRightLocation, m_backLeftLocation,
      m_backRightLocation};

  frc::MecanumDriveOdometry m_odometry{m_kinematics, m_imu.GetRotation2d(),
                                       GetCurrentWheelDistances()};

  // Gains are for example purposes only - must be determined for your own
  // robot!
  frc::SimpleMotorFeedforward<units::meters> m_feedforward{1_V, 3_V / 1_mps};
};
