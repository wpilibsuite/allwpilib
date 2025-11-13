// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/internal/DriverStationModeThread.hpp"

#include "wpi/driverstation/DriverStation.hpp"
#include "wpi/hal/DriverStation.h"
#include "wpi/util/Synchronization.h"

using namespace wpi::internal;

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

void DriverStationModeThread::InDisabled(bool entering) {
  m_userInDisabled = entering;
}

void DriverStationModeThread::InAutonomous(bool entering) {
  m_userInAutonomous = entering;
}

void DriverStationModeThread::InTeleop(bool entering) {
  m_userInTeleop = entering;
}

void DriverStationModeThread::InTest(bool entering) {
  m_userInTest = entering;
}

void DriverStationModeThread::Run() {
  wpi::util::Event event{false, false};
  HAL_ProvideNewDataEventHandle(event.GetHandle());

  while (m_keepAlive.load()) {
    bool timedOut = false;
    wpi::util::WaitForObject(event.GetHandle(), 0.1, &timedOut);
    wpi::DriverStation::RefreshData();
    if (m_userInDisabled) {
      HAL_ObserveUserProgramDisabled();
    }
    if (m_userInAutonomous) {
      HAL_ObserveUserProgramAutonomous();
    }
    if (m_userInTeleop) {
      HAL_ObserveUserProgramTeleop();
    }
    if (m_userInTest) {
      HAL_ObserveUserProgramTest();
    }
  }

  HAL_RemoveNewDataEventHandle(event.GetHandle());
}
