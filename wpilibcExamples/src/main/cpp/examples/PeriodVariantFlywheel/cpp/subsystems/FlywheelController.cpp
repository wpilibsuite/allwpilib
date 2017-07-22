/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include <cmath>

#include "subsystems/Flywheel.h"

FlywheelController::FlywheelController() { m_Y.setZero(); }

void FlywheelController::Enable() { m_loop.Enable(); }

void FlywheelController::Disable() { m_loop.Disable(); }

void FlywheelController::SetVelocityReference(double angularVelocity) {
  Eigen::Matrix<double, 1, 1> nextR;
  nextR << angularVelocity;
  m_loop.SetNextR(nextR);
}

bool FlywheelController::AtReference() const { return m_atReference; }

void FlywheelController::SetMeasuredVelocity(double angularVelocity) {
  // Update angular velocity in the model.
  m_Y << angularVelocity;
}

double FlywheelController::ControllerVoltage() const { return m_loop.U(0); }

double FlywheelController::GetEstimatedVelocity() const {
  return m_loop.Xhat(0);
}

double FlywheelController::Error() const { return m_loop.Error()(0, 0); }

void FlywheelController::Update(std::chrono::nanoseconds dt) {
  if (std::abs(m_loop.NextR(0)) < 1.0) {
    // Kill power at low angular velocities.
    m_loop.Disable();
  }

  m_loop.Correct(m_Y);

  m_atReference = std::abs(Error()) < kTolerance && m_loop.NextR(0) > 1.0;

  m_loop.Predict(dt);
}

void FlywheelController::Reset() { m_loop.Reset(); }
