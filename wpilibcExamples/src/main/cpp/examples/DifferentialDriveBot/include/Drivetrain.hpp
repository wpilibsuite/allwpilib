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
#include "wpi/math/kinematics/DifferentialDriveKinematics.hpp"
#include "wpi/math/kinematics/DifferentialDriveOdometry.hpp"
#include "wpi/units/angle.hpp"
#include "wpi/units/angular_velocity.hpp"
#include "wpi/units/length.hpp"
#include "wpi/units/velocity.hpp"

/**
 * Represents a differential drive style drivetrain.
 */
class Drivetrain {
 public:
  Drivetrain() {
    leftLeader.AddFollower(leftFollower);
    rightLeader.AddFollower(rightFollower);

    // We need to invert one side of the drivetrain so that positive voltages
    // result in both sides moving forward. Depending on how your robot's
    // gearbox is constructed, you might have to invert the left side instead.
    rightLeader.SetInverted(true);

    imu.ResetYaw();
    // Set the distance per pulse for the drive encoders. We can simply use the
    // distance traveled for one rotation of the wheel divided by the encoder
    // resolution.
    leftEncoder.SetDistancePerPulse(2 * std::numbers::pi * kWheelRadius /
                                    kEncoderResolution);
    rightEncoder.SetDistancePerPulse(2 * std::numbers::pi * kWheelRadius /
                                     kEncoderResolution);

    leftEncoder.Reset();
    rightEncoder.Reset();
  }

  static constexpr wpi::units::meters_per_second_t kMaxVelocity =
      3.0_mps;  // 3 meters per second
  static constexpr wpi::units::radians_per_second_t kMaxAngularVelocity{
      std::numbers::pi};  // 1/2 rotation per second

  void SetVelocities(
      const wpi::math::DifferentialDriveWheelVelocities& velocities);
  void Drive(wpi::units::meters_per_second_t xVelocity,
             wpi::units::radians_per_second_t rot);
  void UpdateOdometry();

 private:
  static constexpr wpi::units::meter_t kTrackwidth = 0.381_m * 2;
  static constexpr double kWheelRadius = 0.0508;  // meters
  static constexpr int kEncoderResolution = 4096;

  wpi::PWMSparkMax leftLeader{1};
  wpi::PWMSparkMax leftFollower{2};
  wpi::PWMSparkMax rightLeader{3};
  wpi::PWMSparkMax rightFollower{4};

  wpi::Encoder leftEncoder{0, 1};
  wpi::Encoder rightEncoder{2, 3};

  wpi::math::PIDController leftPIDController{1.0, 0.0, 0.0};
  wpi::math::PIDController rightPIDController{1.0, 0.0, 0.0};

  wpi::OnboardIMU imu{wpi::OnboardIMU::FLAT};

  wpi::math::DifferentialDriveKinematics kinematics{kTrackwidth};
  wpi::math::DifferentialDriveOdometry odometry{
      imu.GetRotation2d(), wpi::units::meter_t{leftEncoder.GetDistance()},
      wpi::units::meter_t{rightEncoder.GetDistance()}};

  // Gains are for example purposes only - must be determined for your own
  // robot!
  wpi::math::SimpleMotorFeedforward<wpi::units::meters> feedforward{
      1_V, 3_V / 1_mps};
};
