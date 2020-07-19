/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "frc/kinematics/MecanumDriveKinematics.h"

using namespace frc;

MecanumDriveWheelSpeeds MecanumDriveKinematics::ToWheelSpeeds(
    const ChassisSpeeds& chassisSpeeds,
    const Translation2d& centerOfRotation) const {
  // We have a new center of rotation. We need to compute the matrix again.
  if (centerOfRotation != m_previousCoR) {
    auto fl = m_frontLeftWheel - centerOfRotation;
    auto fr = m_frontRightWheel - centerOfRotation;
    auto rl = m_rearLeftWheel - centerOfRotation;
    auto rr = m_rearRightWheel - centerOfRotation;

    SetInverseKinematics(fl, fr, rl, rr);

    m_previousCoR = centerOfRotation;
  }

  Eigen::Vector3d chassisSpeedsVector;
  chassisSpeedsVector << chassisSpeeds.vx.to<double>(),
      chassisSpeeds.vy.to<double>(), chassisSpeeds.omega.to<double>();

  Eigen::Matrix<double, 4, 1> wheelsMatrix =
      m_inverseKinematics * chassisSpeedsVector;

  MecanumDriveWheelSpeeds wheelSpeeds;
  wheelSpeeds.frontLeft = units::meters_per_second_t{wheelsMatrix(0, 0)};
  wheelSpeeds.frontRight = units::meters_per_second_t{wheelsMatrix(1, 0)};
  wheelSpeeds.rearLeft = units::meters_per_second_t{wheelsMatrix(2, 0)};
  wheelSpeeds.rearRight = units::meters_per_second_t{wheelsMatrix(3, 0)};
  return wheelSpeeds;
}

ChassisSpeeds MecanumDriveKinematics::ToChassisSpeeds(
    const MecanumDriveWheelSpeeds& wheelSpeeds) const {
  Eigen::Matrix<double, 4, 1> wheelSpeedsMatrix;
  // clang-format off
  wheelSpeedsMatrix << wheelSpeeds.frontLeft.to<double>(), wheelSpeeds.frontRight.to<double>(),
                       wheelSpeeds.rearLeft.to<double>(), wheelSpeeds.rearRight.to<double>();
  // clang-format on

  Eigen::Vector3d chassisSpeedsVector =
      m_forwardKinematics.solve(wheelSpeedsMatrix);

  return {units::meters_per_second_t{chassisSpeedsVector(0)},
          units::meters_per_second_t{chassisSpeedsVector(1)},
          units::radians_per_second_t{chassisSpeedsVector(2)}};
}

void MecanumDriveKinematics::SetInverseKinematics(Translation2d fl,
                                                  Translation2d fr,
                                                  Translation2d rl,
                                                  Translation2d rr) const {
  // clang-format off
  m_inverseKinematics << 1, -1, (-(fl.X() + fl.Y())).template to<double>(),
                         1,  1, (fr.X() - fr.Y()).template to<double>(),
                         1,  1, (rl.X() - rl.Y()).template to<double>(),
                         1, -1, (-(rr.X() + rr.Y())).template to<double>();
  // clang-format on
  m_inverseKinematics /= std::sqrt(2);
}
