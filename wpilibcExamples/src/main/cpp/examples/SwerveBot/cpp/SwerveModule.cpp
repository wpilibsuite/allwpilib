/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "SwerveModule.h"

#include <frc/geometry/Rotation2d.h>
#include <wpi/math>
#include <units/units.h>

SwerveModule::SwerveModule(const int driveMotorChannel,
                           const int turningMotorChannel)
    : m_driveMotor(driveMotorChannel), m_turningMotor(turningMotorChannel) {
  // Set the distance per pulse for the drive encoder. We can simply use the
  // distance traveled for one rotation of the wheel divided by the encoder
  // resolution.
  m_driveEncoder.SetDistancePerPulse(2 * wpi::math::pi * kWheelRadius /
                                     kEncoderResolution);

  // Set the distance (in this case, angle) per pulse for the turning encoder.
  // This is the the angle through an entire rotation (2 * wpi::math::pi)
  // divided by the encoder resolution.
  m_turningEncoder.SetDistancePerPulse(2 * wpi::math::pi / kEncoderResolution);

  // Limit the PID Controller's input range between -pi and pi and set the input
  // to be continuous.
  m_turningPIDController.EnableContinuousInput(-units::radian_t(wpi::math::pi),
                                               units::radian_t(wpi::math::pi));
}

frc::SwerveModuleState SwerveModule::GetState() const {
  return {units::meters_per_second_t{m_driveEncoder.GetRate()},
          frc::Rotation2d(units::radian_t(m_turningEncoder.Get()))};
}

void SwerveModule::SetDesiredState(const frc::SwerveModuleState& state) {

  // Optimize swerve angle rotation because you never need to rotate more than
  // 90 degrees. For example if you move forward and back you just want to reverse
  // the drive motor not rotate the module 180
  frc::SwerveModuleState finalStateAfterOptimization = state;
  frc::Rotation2d deltaAngle{finalStateAfterOptimization.angle.Degrees() - GetState().angle.Degrees()};
  if (units::math::abs(deltaAngle.Degrees()) > 90_deg && units::math::abs(deltaAngle.Degrees()) < 270_deg) {
      units::degree_t finalAngle = units::math::fmod(finalStateAfterOptimization.angle.Degrees() + 180_deg, 360_deg);
      finalStateAfterOptimization.angle = frc::Rotation2d(finalAngle);
      finalStateAfterOptimization.speed = -finalStateAfterOptimization.speed;
  }

  // Calculate the drive output from the drive PID controller.
  const auto driveOutput = m_drivePIDController.Calculate(
      m_driveEncoder.GetRate(), finalStateAfterOptimization.speed.to<double>());

  // Calculate the turning motor output from the turning PID controller.
  const auto turnOutput = m_turningPIDController.Calculate(
      units::radian_t(m_turningEncoder.Get()), finalStateAfterOptimization.angle.Radians());

  // Set the motor outputs.
  m_driveMotor.Set(driveOutput);
  m_turningMotor.Set(turnOutput);
}
