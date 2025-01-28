// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/MotorSafety.h"

#include <utility>

#include <hal/DriverStation.h>
#include <wpi/ManagedStatic.h>
#include <wpi/SafeThread.h>
#include <wpi/SmallPtrSet.h>

#include "frc/DriverStation.h"
#include "frc/Errors.h"

using namespace frc;

namespace {
class Thread : public wpi::SafeThread {
 public:
  Thread() {}
  void Main() override;

  wpi::SmallPtrSet<MotorSafety*, 32> m_instanceList;
};

void Thread::Main() {
  wpi::Event event{false, false};
  HAL_ProvideNewDataEventHandle(event.GetHandle());

  int safetyCounter = 0;
  while (m_active) {
    bool timedOut = false;
    bool signaled = wpi::WaitForObject(event.GetHandle(), 0.1, &timedOut);
    if (signaled) {
      HAL_ControlWord controlWord;
      std::memset(&controlWord, 0, sizeof(controlWord));
      HAL_GetControlWord(&controlWord);
      if (!(controlWord.enabled && controlWord.dsAttached)) {
        safetyCounter = 0;
      }
      if (++safetyCounter >= 4) {
        std::scoped_lock lock(m_mutex);
        for (auto elem : m_instanceList) {
          elem->Check();
        }
        safetyCounter = 0;
      }
    } else {
      safetyCounter = 0;
    }
  }

  HAL_RemoveNewDataEventHandle(event.GetHandle());
}
}  // namespace

namespace {
struct MotorSafetyManager {
  wpi::SafeThreadOwner<Thread> thread;
};
}  // namespace

static wpi::ManagedStatic<MotorSafetyManager> gManager;

static inline MotorSafetyManager& GetManager() {
  return *gManager;
}

MotorSafety::MotorSafety() {
  auto& manager = GetManager();
  if (!manager.thread) {
    manager.thread.Start();
  }
  if (auto thr = manager.thread.GetThread()) {
    thr->m_instanceList.insert(this);
  }
}

MotorSafety::~MotorSafety() {
  if (auto thr = GetManager().thread.GetThread()) {
    thr->m_instanceList.erase(this);
  }
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
    FRC_ReportError(err::Timeout,
                    "{}... Output not updated often enough. See "
                    "https://docs.wpilib.org/motorsafety for more information.",
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
  if (auto thr = GetManager().thread.GetThread()) {
    for (auto elem : thr->m_instanceList) {
      elem->Check();
    }
  }
}
