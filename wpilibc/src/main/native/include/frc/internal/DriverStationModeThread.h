// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <atomic>
#include <thread>

namespace frc::internal {
class DriverStationModeThread {
 public:
  DriverStationModeThread();
  ~DriverStationModeThread();

  DriverStationModeThread(const DriverStationModeThread& other) = delete;
  DriverStationModeThread(DriverStationModeThread&& other) = delete;
  DriverStationModeThread& operator=(const DriverStationModeThread& other) =
      delete;
  DriverStationModeThread& operator=(DriverStationModeThread&& other) = delete;

  void InAutonomous(bool entering);
  void InDisabled(bool entering);
  void InTeleop(bool entering);
  void InTest(bool entering);

 private:
  std::atomic_bool m_keepAlive{false};
  std::thread m_thread;
  void Run();
  bool m_userInDisabled{false};
  bool m_userInAutonomous{false};
  bool m_userInTeleop{false};
  bool m_userInTest{false};
};
}  // namespace frc::internal
