/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include <cmath>

#include "subsystems/SingleJointedArm.h"

SingleJointedArmController::SingleJointedArmController() { m_Y.setZero(); }

void SingleJointedArmController::Enable() { m_loop.Enable(); }

void SingleJointedArmController::Disable() { m_loop.Disable(); }

void SingleJointedArmController::SetReferences(double angle,
                                               double angularVelocity) {
  Eigen::Matrix<double, 2, 1> nextR;
  nextR << angle, angularVelocity;
  m_loop.SetNextR(nextR);
}

bool SingleJointedArmController::AtReferences() const { return m_atReferences; }

void SingleJointedArmController::SetMeasuredAngle(double measuredAngle) {
  m_Y << measuredAngle;
}

double SingleJointedArmController::ControllerVoltage() const {
  return m_loop.U(0);
}

double SingleJointedArmController::EstimatedAngle() const {
  return m_loop.Xhat(0);
}

double SingleJointedArmController::EstimatedAngularVelocity() const {
  return m_loop.Xhat(1);
}

double SingleJointedArmController::AngleError() const {
  return m_loop.Error()(0, 0);
}

double SingleJointedArmController::AngularVelocityError() const {
  return m_loop.Error()(1, 0);
}

void SingleJointedArmController::Update() {
  m_loop.Correct(m_Y);

  auto error = m_loop.Error();
  m_atReferences = std::abs(error(0, 0)) < kAngleTolerance &&
                   std::abs(error(1, 0)) < kAngularVelocityTolerance;

  m_loop.Predict();
}

void SingleJointedArmController::Reset() { m_loop.Reset(); }
