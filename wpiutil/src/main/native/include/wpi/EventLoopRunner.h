/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#ifndef WPIUTIL_WPI_EVENTLOOPRUNNER_H_
#define WPIUTIL_WPI_EVENTLOOPRUNNER_H_

#include <functional>

#include "wpi/SafeThread.h"
#include "wpi/uv/Loop.h"

namespace wpi {

/**
 * Executes an event loop on a separate thread.
 */
class EventLoopRunner {
 public:
  using LoopFunc = std::function<void(wpi::uv::Loop&)>;

  EventLoopRunner();
  virtual ~EventLoopRunner();

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

 private:
  class Thread;
  wpi::SafeThreadOwner<Thread> m_owner;
};

}  // namespace wpi

#endif  // WPIUTIL_WPI_EVENTLOOPRUNNER_H_
