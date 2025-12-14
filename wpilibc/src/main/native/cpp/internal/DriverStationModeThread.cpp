// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/internal/DriverStationModeThread.hpp"

#include "wpi/driverstation/DriverStation.hpp"
#include "wpi/hal/DriverStation.h"
#include "wpi/util/Synchronization.h"

using namespace wpi::internal;

DriverStationModeThread::DriverStationModeThread(wpi::hal::ControlWord word)
    : m_userControlWord{word.GetValue().value} {
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
    wpi::util::WaitForObject(m_event.GetHandle(), 0.1, &timedOut);
    if (!m_keepAlive) {
      break;
    }
    wpi::DriverStation::RefreshData();
    HAL_ObserveUserProgram({.value = m_userControlWord});
  }
}
