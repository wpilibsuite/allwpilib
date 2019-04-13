/*----------------------------------------------------------------------------*/
/* Copyright (c) 2008-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "frc/MotorSafety.h"

#include <algorithm>
#include <utility>

#include <wpi/SmallPtrSet.h>
#include <wpi/SmallString.h>
#include <wpi/raw_ostream.h>

#include "frc/DriverStation.h"
#include "frc/WPIErrors.h"

using namespace frc;

static wpi::SmallPtrSet<MotorSafety*, 32> instanceList;
static wpi::mutex listMutex;

MotorSafety::MotorSafety() {
  std::lock_guard<wpi::mutex> lock(listMutex);
  instanceList.insert(this);
}

MotorSafety::~MotorSafety() {
  std::lock_guard<wpi::mutex> lock(listMutex);
  instanceList.erase(this);
}

MotorSafety::MotorSafety(MotorSafety&& rhs)
    : ErrorBase(std::move(rhs)),
      m_expiration(std::move(rhs.m_expiration)),
      m_enabled(std::move(rhs.m_enabled)),
      m_stopTime(std::move(rhs.m_stopTime)) {}

MotorSafety& MotorSafety::operator=(MotorSafety&& rhs) {
  ErrorBase::operator=(std::move(rhs));

  m_expiration = std::move(rhs.m_expiration);
  m_enabled = std::move(rhs.m_enabled);
  m_stopTime = std::move(rhs.m_stopTime);

  return *this;
}

void MotorSafety::Feed() {
  std::lock_guard<wpi::mutex> lock(m_thisMutex);
  m_stopTime = Timer::GetFPGATimestamp() + m_expiration;
}

void MotorSafety::SetExpiration(double expirationTime) {
  std::lock_guard<wpi::mutex> lock(m_thisMutex);
  m_expiration = expirationTime;
}

double MotorSafety::GetExpiration() const {
  std::lock_guard<wpi::mutex> lock(m_thisMutex);
  return m_expiration;
}

bool MotorSafety::IsAlive() const {
  std::lock_guard<wpi::mutex> lock(m_thisMutex);
  return !m_enabled || m_stopTime > Timer::GetFPGATimestamp();
}

void MotorSafety::SetSafetyEnabled(bool enabled) {
  std::lock_guard<wpi::mutex> lock(m_thisMutex);
  m_enabled = enabled;
}

bool MotorSafety::IsSafetyEnabled() const {
  std::lock_guard<wpi::mutex> lock(m_thisMutex);
  return m_enabled;
}

void MotorSafety::Check() {
  bool enabled;
  double stopTime;

  {
    std::lock_guard<wpi::mutex> lock(m_thisMutex);
    enabled = m_enabled;
    stopTime = m_stopTime;
  }

  DriverStation& ds = DriverStation::GetInstance();
  if (!enabled || ds.IsDisabled() || ds.IsTest()) {
    return;
  }

  if (stopTime < Timer::GetFPGATimestamp()) {
    wpi::SmallString<128> buf;
    wpi::raw_svector_ostream desc(buf);
    GetDescription(desc);
    desc << "... Output not updated often enough.";
    wpi_setWPIErrorWithContext(Timeout, desc.str());
    StopMotor();
  }
}

void MotorSafety::CheckMotors() {
  std::lock_guard<wpi::mutex> lock(listMutex);
  for (auto elem : instanceList) {
    elem->Check();
  }
}
