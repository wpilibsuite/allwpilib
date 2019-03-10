/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <wpi/SafeThread.h>

namespace frc {

class Fiber;

/**
 * A class for that schedules Fibers to run on an internal thread.
 */
class FiberScheduler {
 public:
  /**
   * FiberScheduler constructor.
   *
   * Constructs a non-real-time thread on which to run scheduled fibers.
   */
  FiberScheduler();

  /**
   * FiberScheduler constructor.
   *
   * Constructs a real-time thread on which to run scheduled fibers.
   *
   * @param priority The priority at which to run the internal thread (1..99
   *                 where 1 is highest priority).
   */
  explicit FiberScheduler(int priority);

  ~FiberScheduler();

  FiberScheduler(FiberScheduler&&) = default;
  FiberScheduler& operator=(FiberScheduler&&) = default;

  /**
   * Add a fiber to the scheduler.
   *
   * The fiber will automaticaly start running after its period has elapsed.
   *
   * @param fiber The fiber to add.
   */
  void Add(Fiber& fiber);

  /**
   * Remove a fiber from the scheduler.
   *
   * This will wait until the current fiber has stopped running, then remove
   * the given fiber from the scheduler.
   *
   * @param fiber The fiber to remove.
   */
  void Remove(Fiber& fiber);

 private:
  class Thread;
  wpi::SafeThreadOwner<Thread> m_owner;
};

}  // namespace frc
