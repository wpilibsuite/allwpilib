/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <chrono>
#include <functional>

#include <hal/cpp/fpga_clock.h>
#include <wpi/SafeThread.h>
#include <wpi/StringMap.h>
#include <wpi/StringRef.h>

namespace frc {

/**
 * A class that's a wrapper around a watchdog timer.
 *
 * When the timer expires, a message is printed to the console and an optional
 * user-provided callback is invoked.
 *
 * The watchdog is initialized disabled, so the user needs to call Enable()
 * before use.
 */
class Watchdog {
 public:
  /**
   * Watchdog constructor.
   *
   * @param timeout  The watchdog's timeout in seconds with microsecond
   *                 resolution.
   * @param callback This function is called when the timeout expires.
   */
  Watchdog(double timeout, std::function<void()> callback);

  ~Watchdog();

  Watchdog(Watchdog&&) = default;
  Watchdog& operator=(Watchdog&&) = default;

  /**
   * Returns the time in seconds since the watchdog was last fed.
   */
  double GetTime() const;

  /**
   * Sets the watchdog's timeout.
   *
   * @param timeout The watchdog's timeout in seconds with microsecond
   *                resolution.
   */
  void SetTimeout(double timeout);

  /**
   * Returns the watchdog's timeout in seconds.
   */
  double GetTimeout() const;

  /**
   * Returns true if the watchdog timer has expired.
   */
  bool IsExpired() const;

  /**
   * Adds time since last epoch to the list printed by PrintEpochs().
   *
   * Epochs are a way to partition the time elapsed so that when overruns occur,
   * one can determine which parts of an operation consumed the most time.
   *
   * @param epochName The name to associate with the epoch.
   */
  void AddEpoch(wpi::StringRef epochName);

  /**
   * Prints list of epochs added so far and their times.
   */
  void PrintEpochs();

  /**
   * Resets the watchdog timer.
   *
   * This also enables the timer if it was previously disabled.
   */
  void Reset();

  /**
   * Enables the watchdog timer.
   */
  void Enable();

  /**
   * Disables the watchdog timer.
   */
  void Disable();

 private:
  hal::fpga_clock::time_point m_startTime;
  std::chrono::microseconds m_timeout;
  hal::fpga_clock::time_point m_expirationTime;
  std::function<void()> m_callback;

  wpi::StringMap<std::chrono::microseconds> m_epochs;
  bool m_isExpired = false;

  class Thread;
  wpi::SafeThreadOwner<Thread>* m_owner;

  bool operator>(const Watchdog& rhs);

  static wpi::SafeThreadOwner<Thread>& GetThreadOwner();
};

}  // namespace frc
