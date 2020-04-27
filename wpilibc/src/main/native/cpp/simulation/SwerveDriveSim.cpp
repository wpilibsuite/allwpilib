/*----------------------------------------------------------------------------*/
/* Copyright (c) 2020 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "frc/simulation/SwerveDriveSim.h"

#include "frc/StateSpaceUtil.h"

using namespace frc;
using namespace frc::sim;

SimSwerveModule::SimSwerveModule(const DCMotor& motor, double gearing,
                                 units::meter_t wheelRadius,
                                 const Translation2d& position,
                                 const LinearSystem<2, 1, 1>& azimuthSystem)
    : m_driveMotor(motor),
      m_gearing(gearing),
      m_wheelRadius(wheelRadius),
      m_position(position),
      m_azimuthSystem(azimuthSystem) {
  m_azimuthState = Eigen::Matrix<double, 2, 1>::Zero();
}

Rotation2d SimSwerveModule::GetAzimuthAngle() const {
  return Rotation2d{
      units::math::NormalizeAngle(units::radian_t(m_azimuthState(0, 0)))};
}

units::newton_t SimSwerveModule::EstimateModuleForce(
    units::meters_per_second_t wheelVelocity,
    units::volt_t wheelVoltage) const {
  // By the elevator equations of motion presented in Controls Engineering in
  // FRC, F_m = (G Kt)/(R r) Voltage - (G^2 Kt)/(R r^2 Kv) velocity.
  const auto& G = m_gearing;
  const auto& r = m_wheelRadius;
  const auto& m = m_driveMotor;

  units::newton_t a = G * m.Kt / (m.R * r) * wheelVoltage;
  units::newton_t b =
      (G * G * m.Kt) / (m.R * r * r * m.Kv) * wheelVelocity * 1_rad;

  return a - b;
}

Rotation2d SimSwerveModule::Update(units::volt_t azimuthVoltage,
                                   units::second_t dt) {
  m_azimuthState = m_azimuthSystem.CalculateX(
      m_azimuthState, frc::MakeMatrix<1, 1>(azimuthVoltage.to<double>()), dt);
  return Rotation2d(units::radian_t(m_azimuthState(0, 0)));
}

const Translation2d& SimSwerveModule::GetModulePosition() const {
  return m_position;
}
