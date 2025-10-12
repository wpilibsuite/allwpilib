// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/internal/DriverStationModeThread.h"

#include <hal/DriverStation.h>
#include <wpi/Synchronization.h>

#include "frc/DriverStation.h"

using namespace frc::internal;

DriverStationModeThread::DriverStationModeThread() {
  m_keepAlive = true;
  m_thread = std::thread{[&] { Run(); }};
}

DriverStationModeThread::~DriverStationModeThread() {
  m_keepAlive = false;
  if (m_thread.joinable()) {
    m_thread.join();
  }
}

void DriverStationModeThread::Run() {
  wpi::Event event{false, false};
  HAL_ProvideNewDataEventHandle(event.GetHandle());

  while (m_keepAlive.load()) {
    bool timedOut = false;
    wpi::WaitForObject(event.GetHandle(), 0.1, &timedOut);
    frc::DriverStation::RefreshData();
    HAL_ObserveUserProgram({.value = m_userControlWord});
  }

  HAL_RemoveNewDataEventHandle(event.GetHandle());
}
