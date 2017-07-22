/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "subsystems/DifferentialDriveController.h"

#include <cmath>

DifferentialDriveController::DifferentialDriveController() { m_Y.setZero(); }

void DifferentialDriveController::Enable() { m_loop.Enable(); }

void DifferentialDriveController::Disable() { m_loop.Disable(); }

void DifferentialDriveController::SetReferences(double leftPosition,
                                                double leftVelocity,
                                                double rightPosition,
                                                double rightVelocity) {
  Eigen::Matrix<double, 4, 1> nextR;
  nextR << leftPosition, leftVelocity, rightPosition, rightVelocity;
  m_loop.SetNextR(nextR);
}

bool DifferentialDriveController::AtReferences() const {
  return m_atReferences;
}

void DifferentialDriveController::SetMeasuredStates(double leftPosition,
                                                    double rightPosition) {
  m_Y << leftPosition, rightPosition;
}

double DifferentialDriveController::ControllerLeftVoltage() const {
  return m_loop.U(0);
}

double DifferentialDriveController::ControllerRightVoltage() const {
  return m_loop.U(1);
}

double DifferentialDriveController::EstimatedLeftPosition() const {
  return m_loop.Xhat(0);
}

double DifferentialDriveController::EstimatedLeftVelocity() const {
  return m_loop.Xhat(1);
}

double DifferentialDriveController::EstimatedRightPosition() const {
  return m_loop.Xhat(2);
}

double DifferentialDriveController::EstimatedRightVelocity() const {
  return m_loop.Xhat(3);
}

double DifferentialDriveController::LeftPositionError() const {
  return m_loop.Error()(0, 0);
}

double DifferentialDriveController::LeftVelocityError() const {
  return m_loop.Error()(1, 0);
}

double DifferentialDriveController::RightPositionError() const {
  return m_loop.Error()(2, 0);
}

double DifferentialDriveController::RightVelocityError() const {
  return m_loop.Error()(3, 0);
}

void DifferentialDriveController::Update() {
  m_loop.Correct(m_Y);

  auto error = m_loop.Error();
  m_atReferences = std::abs(error(0, 0)) < kPositionTolerance &&
                   std::abs(error(1, 0)) < kVelocityTolerance &&
                   std::abs(error(2, 0)) < kPositionTolerance &&
                   std::abs(error(3, 0)) < kVelocityTolerance;

  m_loop.Predict();
}

void DifferentialDriveController::Reset() { m_loop.Reset(); }
