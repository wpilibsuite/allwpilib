// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <functional>
#include <string_view>
#include <utility>

#include <units/time.h>

#include "frc/Tracer.h"

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
  Watchdog(units::second_t timeout, std::function<void()> callback);

  template <typename Callable, typename Arg, typename... Args>
  Watchdog(units::second_t timeout, Callable&& f, Arg&& arg, Args&&... args)
      : Watchdog(timeout,
                 std::bind(std::forward<Callable>(f), std::forward<Arg>(arg),
                           std::forward<Args>(args)...)) {}

  ~Watchdog();

  Watchdog(Watchdog&& rhs);
  Watchdog& operator=(Watchdog&& rhs);

  /**
   * Returns the time since the watchdog was last fed.
   */
  units::second_t GetTime() const;

  /**
   * Sets the watchdog's timeout.
   *
   * @param timeout The watchdog's timeout in seconds with microsecond
   *                resolution.
   */
  void SetTimeout(units::second_t timeout);

  /**
   * Returns the watchdog's timeout.
   */
  units::second_t GetTimeout() const;

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
  void AddEpoch(std::string_view epochName);

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
  static constexpr auto kMinPrintPeriod = 1_s;

  units::second_t m_startTime = 0_s;
  units::second_t m_timeout;
  units::second_t m_expirationTime = 0_s;
  std::function<void()> m_callback;
  units::second_t m_lastTimeoutPrintTime = 0_s;

  Tracer m_tracer;
  bool m_isExpired = false;

  bool m_suppressTimeoutMessage = false;

  class Impl;
  Impl* m_impl;

  bool operator>(const Watchdog& rhs) const;

  static Impl* GetImpl();
};

}  // namespace frc
