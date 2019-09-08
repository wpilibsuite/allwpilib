/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "frc/kinematics/DifferentialDriveKinematics.h"

using namespace frc;

DifferentialDriveKinematics::DifferentialDriveKinematics(
    units::meter_t trackWidth)
    : m_trackWidth(trackWidth) {}

ChassisSpeeds DifferentialDriveKinematics::ToChassisSpeeds(
    const DifferentialDriveWheelSpeeds& wheelSpeeds) const {
  return {(wheelSpeeds.left + wheelSpeeds.right) / 2.0, 0_mps,
          (wheelSpeeds.right - wheelSpeeds.left) / m_trackWidth * 1_rad};
}

DifferentialDriveWheelSpeeds DifferentialDriveKinematics::ToWheelSpeeds(
    const ChassisSpeeds& chassisSpeeds) const {
  return {chassisSpeeds.vx - m_trackWidth / 2 * chassisSpeeds.omega / 1_rad,
          chassisSpeeds.vx + m_trackWidth / 2 * chassisSpeeds.omega / 1_rad};
}
