/*----------------------------------------------------------------------------*/
/* Copyright (c) 2008-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "frc/PIDController.h"

#include "frc/Notifier.h"
#include "frc/PIDOutput.h"
#include "frc/smartdashboard/SendableBuilder.h"

using namespace frc;

PIDController::PIDController(double Kp, double Ki, double Kd, PIDSource* source,
                             PIDOutput* output, double period)
    : PIDController(Kp, Ki, Kd, 0.0, *source, *output, period) {}

PIDController::PIDController(double Kp, double Ki, double Kd, double Kf,
                             PIDSource* source, PIDOutput* output,
                             double period)
    : PIDController(Kp, Ki, Kd, Kf, *source, *output, period) {}

PIDController::PIDController(double Kp, double Ki, double Kd, PIDSource& source,
                             PIDOutput& output, double period)
    : PIDController(Kp, Ki, Kd, 0.0, source, output, period) {}

PIDController::PIDController(double Kp, double Ki, double Kd, double Kf,
                             PIDSource& source, PIDOutput& output,
                             double period)
    : PIDBase(Kp, Ki, Kd, Kf, source, output) {
  m_controlLoop = std::make_unique<Notifier>(&PIDController::Calculate, this);
  m_controlLoop->StartPeriodic(period);
}

PIDController::~PIDController() {
  // Forcefully stopping the notifier so the callback can successfully run.
  m_controlLoop->Stop();
}

void PIDController::Enable() {
  {
    std::lock_guard<wpi::mutex> lock(m_thisMutex);
    m_enabled = true;
  }
}

void PIDController::Disable() {
  {
    // Ensures m_enabled modification and PIDWrite() call occur atomically
    std::lock_guard<wpi::mutex> pidWriteLock(m_pidWriteMutex);
    {
      std::lock_guard<wpi::mutex> mainLock(m_thisMutex);
      m_enabled = false;
    }

    m_pidOutput->PIDWrite(0);
  }
}

void PIDController::SetEnabled(bool enable) {
  if (enable) {
    Enable();
  } else {
    Disable();
  }
}

bool PIDController::IsEnabled() const {
  std::lock_guard<wpi::mutex> lock(m_thisMutex);
  return m_enabled;
}

void PIDController::Reset() {
  Disable();

  PIDBase::Reset();
}

void PIDController::InitSendable(SendableBuilder& builder) {
  PIDBase::InitSendable(builder);
  builder.AddBooleanProperty("enabled", [=]() { return IsEnabled(); },
                             [=](bool value) { SetEnabled(value); });
}
