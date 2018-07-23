/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "frc/Watchdog.h"

#include <wpi/raw_ostream.h>

#include "frc/Timer.h"

using namespace frc;

Watchdog::Watchdog(double timeout, std::function<void()> callback)
    : m_timeout(timeout),
      m_callback(callback),
      m_notifier(&Watchdog::TimeoutFunc, this) {
  Enable();
}

double Watchdog::GetTime() const {
  return Timer::GetFPGATimestamp() - m_startTime;
}

bool Watchdog::IsExpired() const { return m_isExpired; }

void Watchdog::AddEpoch(wpi::StringRef epochName) {
  double currentTime = Timer::GetFPGATimestamp();
  m_epochs[epochName] = currentTime - m_startTime;
  m_startTime = currentTime;
}

void Watchdog::PrintEpochs() {
  for (const auto& epoch : m_epochs) {
    wpi::outs() << "\t" << epoch.getKey() << ": " << epoch.getValue() << "s\n";
  }
}

void Watchdog::Reset() { Enable(); }

void Watchdog::Enable() {
  m_startTime = Timer::GetFPGATimestamp();
  m_isExpired = false;
  m_epochs.clear();
  m_notifier.StartPeriodic(m_timeout);
}

void Watchdog::Disable() { m_notifier.Stop(); }

void Watchdog::TimeoutFunc() {
  if (!m_isExpired) {
    wpi::outs() << "Watchdog not fed after " << m_timeout << "s\n";
    m_callback();
    m_isExpired = true;
    Disable();
  }
}
