// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/internal/DriverStationModeThread.h"

#include <hal/DriverStation.h>
#include <wpi/Synchronization.h>

#include "frc/DriverStation.h"

using namespace frc::internal;

DriverStationModeThread::DriverStationModeThread(HAL_ControlWord word)
    : m_userControlWord{word.value} {
  HAL_ProvideNewDataEventHandle(m_event.GetHandle());
  m_keepAlive = true;
  m_thread = std::thread{[this] { Run(); }};
}

DriverStationModeThread::~DriverStationModeThread() {
  HAL_RemoveNewDataEventHandle(m_event.GetHandle());
  m_keepAlive = false;
  m_event.Set();
  if (m_thread.joinable()) {
    m_thread.join();
  }
}

void DriverStationModeThread::Run() {
  for (;;) {
    bool timedOut = false;
    wpi::WaitForObject(m_event.GetHandle(), 0.1, &timedOut);
    if (!m_keepAlive) {
      break;
    }
    frc::DriverStation::RefreshData();
    HAL_ObserveUserProgram({.value = m_userControlWord});
  }
}
