// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <atomic>
#include <thread>

#include "wpi/hal/DriverStationTypes.hpp"
#include "wpi/util/Synchronization.h"

namespace wpi::internal {
/**
 * For internal use only.
 */
class DriverStationModeThread {
 public:
  /**
   * For internal use only.
   *
   * @param word initial control word
   */
  explicit DriverStationModeThread(wpi::hal::ControlWord word);

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
   * @param word control word
   */
  void InControl(wpi::hal::ControlWord word) {
    m_userControlWord = word.GetValue().value;
  }

 private:
  std::atomic_bool m_keepAlive{false};
  wpi::util::Event m_event{false, false};
  std::thread m_thread;
  void Run();
  std::atomic<int64_t> m_userControlWord;
};
}  // namespace wpi::internal
