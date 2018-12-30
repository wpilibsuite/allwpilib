/*----------------------------------------------------------------------------*/
/* Copyright (c) 2008-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "frc/MotorSafety.h"

#include <algorithm>
#include <utility>

#include <wpi/SmallString.h>

#include "frc/WPIErrors.h"

using namespace frc;

MotorSafety::MotorSafety(MotorSafety&& rhs)
    : ErrorBase(std::move(rhs)), m_enabled(std::move(rhs.m_enabled)) {
  m_watchdog = Watchdog(rhs.m_watchdog.GetTimeout(), [this] { TimeoutFunc(); });
}

MotorSafety& MotorSafety::operator=(MotorSafety&& rhs) {
  ErrorBase::operator=(std::move(rhs));

  m_watchdog = Watchdog(rhs.m_watchdog.GetTimeout(), [this] { TimeoutFunc(); });
  m_enabled = std::move(rhs.m_enabled);

  return *this;
}

void MotorSafety::Feed() {
  std::lock_guard<wpi::mutex> lock(m_thisMutex);
  m_watchdog.Reset();
}

void MotorSafety::SetExpiration(double expirationTime) {
  std::lock_guard<wpi::mutex> lock(m_thisMutex);
  m_watchdog.SetTimeout(expirationTime);
}

double MotorSafety::GetExpiration() const {
  std::lock_guard<wpi::mutex> lock(m_thisMutex);
  return m_watchdog.GetTimeout();
}

bool MotorSafety::IsAlive() const {
  std::lock_guard<wpi::mutex> lock(m_thisMutex);
  return !m_enabled || !m_watchdog.IsExpired();
}

void MotorSafety::SetSafetyEnabled(bool enabled) {
  std::lock_guard<wpi::mutex> lock(m_thisMutex);
  if (enabled) {
    m_watchdog.Enable();
  } else {
    m_watchdog.Disable();
  }
  m_enabled = enabled;
}

bool MotorSafety::IsSafetyEnabled() const {
  std::lock_guard<wpi::mutex> lock(m_thisMutex);
  return m_enabled;
}

void MotorSafety::TimeoutFunc() {
  auto& ds = DriverStation::GetInstance();
  if (ds.IsDisabled() || ds.IsTest()) {
    return;
  }

  wpi::SmallString<128> buf;
  wpi::raw_svector_ostream desc(buf);
  GetDescription(desc);
  desc << "... Output not updated often enough.";
  wpi_setWPIErrorWithContext(Timeout, desc.str());
  StopMotor();
}
