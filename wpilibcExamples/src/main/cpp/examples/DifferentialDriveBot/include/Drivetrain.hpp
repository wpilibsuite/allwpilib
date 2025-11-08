// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <numbers>

#include <wpi/hardware/imu/OnboardIMU.hpp>
#include <wpi/hardware/motor/PWMSparkMax.hpp>
#include <wpi/hardware/rotation/Encoder.hpp>
#include <wpi/math/controller/PIDController.hpp>
#include <wpi/math/controller/SimpleMotorFeedforward.hpp>
#include <wpi/math/kinematics/DifferentialDriveKinematics.hpp>
#include <wpi/math/kinematics/DifferentialDriveOdometry.hpp>
#include <wpi/units/angle.hpp>
#include <wpi/units/angular_velocity.hpp>
#include <wpi/units/length.hpp>
#include <wpi/units/velocity.hpp>

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

    m_imu.ResetYaw();
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

  static constexpr wpi::units::meters_per_second_t kMaxSpeed =
      3.0_mps;  // 3 meters per second
  static constexpr wpi::units::radians_per_second_t kMaxAngularSpeed{
      std::numbers::pi};  // 1/2 rotation per second

  void SetSpeeds(const wpi::math::DifferentialDriveWheelSpeeds& speeds);
  void Drive(wpi::units::meters_per_second_t xSpeed,
             wpi::units::radians_per_second_t rot);
  void UpdateOdometry();

 private:
  static constexpr wpi::units::meter_t kTrackwidth = 0.381_m * 2;
  static constexpr double kWheelRadius = 0.0508;  // meters
  static constexpr int kEncoderResolution = 4096;

  wpi::PWMSparkMax m_leftLeader{1};
  wpi::PWMSparkMax m_leftFollower{2};
  wpi::PWMSparkMax m_rightLeader{3};
  wpi::PWMSparkMax m_rightFollower{4};

  wpi::Encoder m_leftEncoder{0, 1};
  wpi::Encoder m_rightEncoder{2, 3};

  wpi::math::PIDController m_leftPIDController{1.0, 0.0, 0.0};
  wpi::math::PIDController m_rightPIDController{1.0, 0.0, 0.0};

  wpi::OnboardIMU m_imu{wpi::OnboardIMU::kFlat};

  wpi::math::DifferentialDriveKinematics m_kinematics{kTrackwidth};
  wpi::math::DifferentialDriveOdometry m_odometry{
      m_imu.GetRotation2d(), wpi::units::meter_t{m_leftEncoder.GetDistance()},
      wpi::units::meter_t{m_rightEncoder.GetDistance()}};

  // Gains are for example purposes only - must be determined for your own
  // robot!
  wpi::math::SimpleMotorFeedforward<wpi::units::meters> m_feedforward{1_V, 3_V / 1_mps};
};
