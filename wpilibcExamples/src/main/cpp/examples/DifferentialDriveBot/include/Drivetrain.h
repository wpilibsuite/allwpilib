// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <numbers>

#include <frc/AnalogGyro.h>
#include <frc/Encoder.h>
#include <frc/controller/PIDController.h>
#include <frc/controller/SimpleMotorFeedforward.h>
#include <frc/kinematics/DifferentialDriveKinematics.h>
#include <frc/kinematics/DifferentialDriveOdometry.h>
#include <frc/motorcontrol/PWMSparkMax.h>
#include <units/angle.h>
#include <units/angular_velocity.h>
#include <units/length.h>
#include <units/velocity.h>

/**
 * Represents a differential drive style drivetrain.
 */
class Drivetrain {
 public:
  Drivetrain() {
    m_leftLeader.AddFollower(m_leftFollower);
    m_rightLeader.AddFollower(m_rightFollower);

    // We need to invert one side of the drivetrain so that positive voltages
    // result in both sides moving forward. Depending on how your robot's
    // gearbox is constructed, you might have to invert the left side instead.
    m_rightLeader.SetInverted(true);

    m_gyro.Reset();
    // Set the distance per pulse for the drive encoders. We can simply use the
    // distance traveled for one rotation of the wheel divided by the encoder
    // resolution.
    m_leftEncoder.SetDistancePerPulse(2 * std::numbers::pi * kWheelRadius /
                                      kEncoderResolution);
    m_rightEncoder.SetDistancePerPulse(2 * std::numbers::pi * kWheelRadius /
                                       kEncoderResolution);

    m_leftEncoder.Reset();
    m_rightEncoder.Reset();
  }

  static constexpr units::meters_per_second_t kMaxSpeed =
      3.0_mps;  // 3 meters per second
  static constexpr units::radians_per_second_t kMaxAngularSpeed{
      std::numbers::pi};  // 1/2 rotation per second

  void SetSpeeds(const frc::DifferentialDriveWheelSpeeds& speeds);
  void Drive(units::meters_per_second_t xSpeed,
             units::radians_per_second_t rot);
  void UpdateOdometry();

 private:
  static constexpr units::meter_t kTrackwidth = 0.381_m * 2;
  static constexpr double kWheelRadius = 0.0508;  // meters
  static constexpr int kEncoderResolution = 4096;

  frc::PWMSparkMax m_leftLeader{1};
  frc::PWMSparkMax m_leftFollower{2};
  frc::PWMSparkMax m_rightLeader{3};
  frc::PWMSparkMax m_rightFollower{4};

  frc::Encoder m_leftEncoder{0, 1};
  frc::Encoder m_rightEncoder{2, 3};

  frc::PIDController m_leftPIDController{1.0, 0.0, 0.0};
  frc::PIDController m_rightPIDController{1.0, 0.0, 0.0};

  frc::AnalogGyro m_gyro{0};

  frc::DifferentialDriveKinematics m_kinematics{kTrackwidth};
  frc::DifferentialDriveOdometry m_odometry{
      m_gyro.GetRotation2d(), units::meter_t{m_leftEncoder.GetDistance()},
      units::meter_t{m_rightEncoder.GetDistance()}};

  // Gains are for example purposes only - must be determined for your own
  // robot!
  frc::SimpleMotorFeedforward<units::meters> m_feedforward{1_V, 3_V / 1_mps};
};
