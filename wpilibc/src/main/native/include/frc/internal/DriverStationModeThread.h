// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <atomic>
#include <thread>

namespace frc::internal {
/**
 * For internal use only.
 */
class DriverStationModeThread {
 public:
  /**
   * For internal use only.
   */
  DriverStationModeThread();

  ~DriverStationModeThread();

  DriverStationModeThread(const DriverStationModeThread& other) = delete;
  DriverStationModeThread(DriverStationModeThread&& other) = delete;
  DriverStationModeThread& operator=(const DriverStationModeThread& other) =
      delete;
  DriverStationModeThread& operator=(DriverStationModeThread&& other) = delete;

  /**
   * Only to be used to tell the Driver Station what code you claim to be
   * executing for diagnostic purposes only.
   *
   * @param id opmode UID
   * @param enabled If true, running enabled code; if false, running disabled
   * code
   */
  void InOpMode(int64_t id, bool enabled);

 private:
  std::atomic_bool m_keepAlive{false};
  std::thread m_thread;
  void Run();
  std::atomic<int64_t> m_userOpMode{0};
  std::atomic<bool> m_userEnabled{false};
};
}  // namespace frc::internal
