// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <frc/SynchronousInterrupt.h>

#include <atomic>
#include <functional>
#include <memory>
#include <thread>
#include <utility>

namespace frc {
/**
 * Class for handling asynchronous interrupts.
 *
 * <p> By default, interrupts will occur on rising edge. Callbacks are disabled
 * by default, and Enable() must be called before they will occur.
 *
 * <p> Both rising and falling edges can be indicated in one callback if both a
 * rising and falling edge occurred since the previous callback.
 *
 * <p>Synchronous interrupts are handled by the SynchronousInterrupt class.
 */
class AsynchronousInterrupt {
 public:
  /**
   * Construct an Asynchronous Interrupt from a Digital Source.
   *
   * <p> At construction, the interrupt will trigger on the rising edge.
   *
   * <p> The first bool in the callback is rising, the second is falling.
   */
  AsynchronousInterrupt(DigitalSource& source,
                        std::function<void(bool, bool)> callback);

  /**
   * Construct an Asynchronous Interrupt from a Digital Source.
   *
   * <p> At construction, the interrupt will trigger on the rising edge.
   *
   * <p> The first bool in the callback is rising, the 2nd is falling.
   */
  AsynchronousInterrupt(DigitalSource* source,
                        std::function<void(bool, bool)> callback);

  /**
   * Construct an Asynchronous Interrupt from a Digital Source.
   *
   * <p> At construction, the interrupt will trigger on the rising edge.
   *
   * <p> The first bool in the callback is rising, the 2nd is falling.
   */
  AsynchronousInterrupt(std::shared_ptr<DigitalSource> source,
                        std::function<void(bool, bool)> callback);

  /**
   * Construct an Asynchronous Interrupt from a Digital Source.
   *
   * <p> At construction, the interrupt will trigger on the rising edge.
   *
   * <p> The first bool in the callback is rising, the 2nd is falling.
   */
  template <typename Callable, typename Arg, typename... Args>
  AsynchronousInterrupt(DigitalSource& source, Callable&& f, Arg&& arg,
                        Args&&... args)
      : AsynchronousInterrupt(
            source, std::bind(std::forward<Callable>(f), std::forward<Arg>(arg),
                              std::forward<Args>(args)...)) {}

  /**
   * Construct an Asynchronous Interrupt from a Digital Source.
   *
   * <p> At construction, the interrupt will trigger on the rising edge.
   *
   * <p> The first bool in the callback is rising, the 2nd is falling.
   */
  template <typename Callable, typename Arg, typename... Args>
  AsynchronousInterrupt(DigitalSource* source, Callable&& f, Arg&& arg,
                        Args&&... args)
      : AsynchronousInterrupt(
            source, std::bind(std::forward<Callable>(f), std::forward<Arg>(arg),
                              std::forward<Args>(args)...)) {}

  /**
   * Construct an Asynchronous Interrupt from a Digital Source.
   *
   * <p> At construction, the interrupt will trigger on the rising edge.
   *
   * <p> The first bool in the callback is rising, the 2nd is falling.
   */
  template <typename Callable, typename Arg, typename... Args>
  AsynchronousInterrupt(std::shared_ptr<DigitalSource> source, Callable&& f,
                        Arg&& arg, Args&&... args)
      : AsynchronousInterrupt(
            source, std::bind(std::forward<Callable>(f), std::forward<Arg>(arg),
                              std::forward<Args>(args)...)) {}

  ~AsynchronousInterrupt();

  /**
   * Enables interrupt callbacks. Before this, callbacks will not occur. Does
   * nothing if already enabled.
   */
  void Enable();

  /**
   * Disables interrupt callbacks. Does nothing if already disabled.
   */
  void Disable();

  /**
   * Set which edges to trigger the interrupt on.
   *
   * @param risingEdge Trigger on rising edge
   * @param fallingEdge Trigger on falling edge
   */
  void SetInterruptEdges(bool risingEdge, bool fallingEdge);

  /**
   * Get the timestamp of the last rising edge.
   *
   * <p>This function does not require the interrupt to be enabled to work.
   *
   * <p>This only works if rising edge was configured using SetInterruptEdges.
   * @return the timestamp in seconds relative to getFPGATime
   */
  units::second_t GetRisingTimestamp();

  /**
   * Get the timestamp of the last falling edge.
   *
   * <p>This function does not require the interrupt to be enabled to work.
   *
   * <p>This only works if falling edge was configured using SetInterruptEdges.
   * @return the timestamp in seconds relative to getFPGATime
   */
  units::second_t GetFallingTimestamp();

 private:
  void ThreadMain();

  std::atomic_bool m_keepRunning{false};
  std::thread m_thread;
  SynchronousInterrupt m_interrupt;
  std::function<void(bool, bool)> m_callback;
};
}  // namespace frc
