/*----------------------------------------------------------------------------*/
/* Copyright (c) 2020 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <frc/SynchronousInterrupt.h>

#include <atomic>
#include <memory>
#include <thread>
#include <utility>

namespace frc {
/**
 * Class for handling sychronous interrupts.
 *
 * <p>Asynchronous interrupts are handled by the AsynchronousInterrupt class.
 */
class AsynchronousInterrupt {
  explicit AsynchronousInterrupt(DigitalSource& source,
                                 std::function<void(bool, bool)> callback);
  explicit AsynchronousInterrupt(DigitalSource* source,
                                 std::function<void(bool, bool)> callback);
  explicit AsynchronousInterrupt(std::shared_ptr<DigitalSource> source,
                                 std::function<void(bool, bool)> callback);

  template <typename Callable, typename Arg, typename... Args>
  AsynchronousInterrupt(DigitalSource& source, Callable&& f, Arg&& arg,
                        Args&&... args)
      : AsynchronousInterrupt(
            source, std::bind(std::forward<Callable>(f), std::forward<Arg>(arg),
                              std::forward<Args>(args)...)) {}

  template <typename Callable, typename Arg, typename... Args>
  AsynchronousInterrupt(DigitalSource* source, Callable&& f, Arg&& arg,
                        Args&&... args)
      : AsynchronousInterrupt(
            source, std::bind(std::forward<Callable>(f), std::forward<Arg>(arg),
                              std::forward<Args>(args)...)) {}

  template <typename Callable, typename Arg, typename... Args>
  AsynchronousInterrupt(std::shared_ptr<DigitalSource> source, Callable&& f,
                        Arg&& arg, Args&&... args)
      : AsynchronousInterrupt(
            source, std::bind(std::forward<Callable>(f), std::forward<Arg>(arg),
                              std::forward<Args>(args)...)) {}

  ~AsynchronousInterrupt();

  AsynchronousInterrupt(AsynchronousInterrupt&&) = default;
  AsynchronousInterrupt& operator=(AsynchronousInterrupt&&) = default;

  void Enable();
  void Disable();

  void SetInterruptEdges(bool risingEdge, bool fallingEdge);

  units::second_t GetRisingTimestamp();
  units::second_t GetFallingTimestamp();

 private:
  void ThreadMain();

  std::atomic_bool m_keepRunning{false};
  std::thread m_thread;
  SynchronousInterrupt m_interrupt;
  std::function<void(bool, bool)> m_callback;
};
}  // namespace frc
