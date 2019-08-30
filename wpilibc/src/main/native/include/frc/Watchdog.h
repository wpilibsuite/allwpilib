/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018-2019 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <chrono>
#include <functional>
#include <utility>

#include <hal/cpp/fpga_clock.h>
#include <units/units.h>
#include <wpi/SafeThread.h>
#include <wpi/StringMap.h>
#include <wpi/StringRef.h>
#include <wpi/deprecated.h>

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
  WPI_DEPRECATED("Use unit-safe version instead")
  Watchdog(double timeout, std::function<void()> callback);

  /**
   * Watchdog constructor.
   *
   * @param timeout  The watchdog's timeout in seconds with microsecond
   *                 resolution.
   * @param callback This function is called when the timeout expires.
   */
  Watchdog(units::second_t timeout, std::function<void()> callback);

  template <typename Callable, typename Arg, typename... Args>
  WPI_DEPRECATED("Use unit-safe version instead")
  Watchdog(double timeout, Callable&& f, Arg&& arg, Args&&... args)
      : Watchdog(units::second_t{timeout}, arg, args...) {}

  template <typename Callable, typename Arg, typename... Args>
  Watchdog(units::second_t timeout, Callable&& f, Arg&& arg, Args&&... args)
      : Watchdog(timeout,
                 std::bind(std::forward<Callable>(f), std::forward<Arg>(arg),
                           std::forward<Args>(args)...)) {}

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
  WPI_DEPRECATED("Use unit-safe version instead")
  void SetTimeout(double timeout);

  /**
   * Sets the watchdog's timeout.
   *
   * @param timeout The watchdog's timeout in seconds with microsecond
   *                resolution.
   */
  void SetTimeout(units::second_t timeout);

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

  /**
   * Enable or disable suppression of the generic timeout message.
   *
   * This may be desirable if the user-provided callback already prints a more
   * specific message.
   *
   * @param suppress Whether to suppress generic timeout message.
   */
  void SuppressTimeoutMessage(bool suppress);

 private:
  // Used for timeout print rate-limiting
  static constexpr std::chrono::milliseconds kMinPrintPeriod{1000};

  hal::fpga_clock::time_point m_startTime;
  std::chrono::nanoseconds m_timeout;
  hal::fpga_clock::time_point m_expirationTime;
  std::function<void()> m_callback;
  hal::fpga_clock::time_point m_lastTimeoutPrintTime = hal::fpga_clock::epoch();
  hal::fpga_clock::time_point m_lastEpochsPrintTime = hal::fpga_clock::epoch();

  wpi::StringMap<std::chrono::nanoseconds> m_epochs;
  bool m_isExpired = false;

  bool m_suppressTimeoutMessage = false;

  class Thread;
  wpi::SafeThreadOwner<Thread>* m_owner;

  bool operator>(const Watchdog& rhs);

  static wpi::SafeThreadOwner<Thread>& GetThreadOwner();
};

}  // namespace frc
