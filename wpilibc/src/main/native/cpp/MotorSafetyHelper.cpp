/*----------------------------------------------------------------------------*/
/* Copyright (c) 2008-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "frc/MotorSafetyHelper.h"

#include <wpi/SmallPtrSet.h>
#include <wpi/SmallString.h>
#include <wpi/raw_ostream.h>

#include "frc/DriverStation.h"
#include "frc/MotorSafety.h"
#include "frc/Timer.h"
#include "frc/WPIErrors.h"

using namespace frc;

static wpi::SmallPtrSet<MotorSafetyHelper*, 32> helperList;
static wpi::mutex listMutex;

MotorSafetyHelper::MotorSafetyHelper(MotorSafety* safeObject)
    : m_safeObject(safeObject) {
  m_enabled = false;
  m_expiration = DEFAULT_SAFETY_EXPIRATION;
  m_stopTime = Timer::GetFPGATimestamp();

  std::lock_guard<wpi::mutex> lock(listMutex);
  helperList.insert(this);
}

MotorSafetyHelper::~MotorSafetyHelper() {
  std::lock_guard<wpi::mutex> lock(listMutex);
  helperList.erase(this);
}

void MotorSafetyHelper::Feed() {
  std::lock_guard<wpi::mutex> lock(m_thisMutex);
  m_stopTime = Timer::GetFPGATimestamp() + m_expiration;
}

void MotorSafetyHelper::SetExpiration(double expirationTime) {
  std::lock_guard<wpi::mutex> lock(m_thisMutex);
  m_expiration = expirationTime;
}

double MotorSafetyHelper::GetExpiration() const {
  std::lock_guard<wpi::mutex> lock(m_thisMutex);
  return m_expiration;
}

bool MotorSafetyHelper::IsAlive() const {
  std::lock_guard<wpi::mutex> lock(m_thisMutex);
  return !m_enabled || m_stopTime > Timer::GetFPGATimestamp();
}

void MotorSafetyHelper::Check() {
  bool enabled;
  double stopTime;

  {
    std::lock_guard<wpi::mutex> lock(m_thisMutex);
    enabled = m_enabled;
    stopTime = m_stopTime;
  }

  DriverStation& ds = DriverStation::GetInstance();
  if (!enabled || ds.IsDisabled() || ds.IsTest()) return;

  if (stopTime < Timer::GetFPGATimestamp()) {
    wpi::SmallString<128> buf;
    wpi::raw_svector_ostream desc(buf);
    m_safeObject->GetDescription(desc);
    desc << "... Output not updated often enough.";
    wpi_setWPIErrorWithContext(Timeout, desc.str());
    m_safeObject->StopMotor();
  }
}

void MotorSafetyHelper::SetSafetyEnabled(bool enabled) {
  std::lock_guard<wpi::mutex> lock(m_thisMutex);
  m_enabled = enabled;
}

bool MotorSafetyHelper::IsSafetyEnabled() const {
  std::lock_guard<wpi::mutex> lock(m_thisMutex);
  return m_enabled;
}

void MotorSafetyHelper::CheckMotors() {
  std::lock_guard<wpi::mutex> lock(listMutex);
  for (auto elem : helperList) {
    elem->Check();
  }
}
