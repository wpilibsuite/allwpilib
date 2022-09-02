// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/MotorSafety.h"

#include <algorithm>
#include <utility>

#include <wpi/SmallPtrSet.h>
#include <wpi/SafeThread.h>

#include "frc/DriverStation.h"
#include "frc/Errors.h"

using namespace frc;

static wpi::SmallPtrSet<MotorSafety*, 32> instanceList;
static wpi::mutex listMutex;
static bool threadStarted = false;

#ifndef __FRC_ROBORIO__
namespace frc::impl {
void ResetMotorSafety() {
  std::scoped_lock lock(listMutex);
  instanceList.clear();
}
}  // namespace frc::impl
#endif

MotorSafety::MotorSafety() {
  std::scoped_lock lock(listMutex);
  instanceList.insert(this);
  if (!threadStarted) {
    threadStarted = true;
    // TODO Threads
  }
}

MotorSafety::~MotorSafety() {
  std::scoped_lock lock(listMutex);
  instanceList.erase(this);
}

MotorSafety::MotorSafety(MotorSafety&& rhs)
    : m_expiration(std::move(rhs.m_expiration)),
      m_enabled(std::move(rhs.m_enabled)),
      m_stopTime(std::move(rhs.m_stopTime)) {}

MotorSafety& MotorSafety::operator=(MotorSafety&& rhs) {
  std::scoped_lock lock(m_thisMutex, rhs.m_thisMutex);

  m_expiration = std::move(rhs.m_expiration);
  m_enabled = std::move(rhs.m_enabled);
  m_stopTime = std::move(rhs.m_stopTime);

  return *this;
}

void MotorSafety::Feed() {
  std::scoped_lock lock(m_thisMutex);
  m_stopTime = Timer::GetFPGATimestamp() + m_expiration;
}

void MotorSafety::SetExpiration(units::second_t expirationTime) {
  std::scoped_lock lock(m_thisMutex);
  m_expiration = expirationTime;
}

units::second_t MotorSafety::GetExpiration() const {
  std::scoped_lock lock(m_thisMutex);
  return m_expiration;
}

bool MotorSafety::IsAlive() const {
  std::scoped_lock lock(m_thisMutex);
  return !m_enabled || m_stopTime > Timer::GetFPGATimestamp();
}

void MotorSafety::SetSafetyEnabled(bool enabled) {
  std::scoped_lock lock(m_thisMutex);
  m_enabled = enabled;
}

bool MotorSafety::IsSafetyEnabled() const {
  std::scoped_lock lock(m_thisMutex);
  return m_enabled;
}

void MotorSafety::Check() {
  bool enabled;
  units::second_t stopTime;

  {
    std::scoped_lock lock(m_thisMutex);
    enabled = m_enabled;
    stopTime = m_stopTime;
  }

  if (!enabled || DriverStation::IsDisabled() || DriverStation::IsTest()) {
    return;
  }

  if (stopTime < Timer::GetFPGATimestamp()) {
    FRC_ReportError(err::Timeout, "{}... Output not updated often enough",
                    GetDescription());

    try {
      StopMotor();
    } catch (frc::RuntimeError& e) {
      e.Report();
    } catch (std::exception& e) {
      FRC_ReportError(err::Error, "{} StopMotor threw unexpected exception: {}",
                      GetDescription(), e.what());
    }
  }
}

void MotorSafety::CheckMotors() {
  std::scoped_lock lock(listMutex);
  for (auto elem : instanceList) {
    elem->Check();
  }
}
