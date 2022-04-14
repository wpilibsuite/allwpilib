// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#ifndef WPIUTIL_WPI_EVENTLOOPRUNNER_H_
#define WPIUTIL_WPI_EVENTLOOPRUNNER_H_

#include <functional>
#include <memory>

#include "wpi/SafeThread.h"
#include "wpi/uv/Loop.h"

namespace wpi {

/**
 * Executes an event loop on a separate thread.
 */
class EventLoopRunner {
 public:
  using LoopFunc = std::function<void(uv::Loop&)>;

  EventLoopRunner();
  virtual ~EventLoopRunner();

  /**
   * Stop the loop.  Once the loop is stopped it cannot be restarted.
   * This function does not return until the loop has exited.
   */
  void Stop();

  /**
   * Run a function asynchronously (once) on the loop.
   * This is safe to call from any thread, but is NOT safe to call from the
   * provided function (it will deadlock).
   * @param func function to execute on the loop
   */
  void ExecAsync(LoopFunc func);

  /**
   * Run a function synchronously (once) on the loop.
   * This is safe to call from any thread, but is NOT safe to call from the
   * provided function (it will deadlock).
   * This does not return until the function finishes executing.
   * @param func function to execute on the loop
   */
  void ExecSync(LoopFunc func);

  /**
   * Get the loop.  If the loop thread is not running, returns nullptr.
   * @return The loop
   */
  std::shared_ptr<uv::Loop> GetLoop();

 private:
  class Thread;
  SafeThreadOwner<Thread> m_owner;
};

}  // namespace wpi

#endif  // WPIUTIL_WPI_EVENTLOOPRUNNER_H_
