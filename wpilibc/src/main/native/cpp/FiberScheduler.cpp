/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "frc/FiberScheduler.h"

#include <hal/HALBase.h>
#include <hal/Threads.h>
#include <wpi/PriorityQueue.h>

#include "frc/ErrorBase.h"
#include "frc/Fiber.h"

using namespace frc;

class FiberControlBlock {
 public:
  std::chrono::microseconds m_period;
  hal::fpga_clock::time_point m_expirationTime;
};

class FiberScheduler::Thread : public wpi::SafeThread {
 public:
  template <typename T>
  struct DerefGreater {
    constexpr bool operator()(const T& lhs, const T& rhs) const {
      return *lhs > *rhs;
    }
  };

  wpi::PriorityQueue<Fiber*, std::vector<Fiber*>, DerefGreater<Fiber*>>
      m_fibers;

 private:
  void Main() override;
};

void FiberScheduler::Thread::Main() {
  std::unique_lock<wpi::mutex> lock(m_mutex);

  while (m_active) {
    if (m_fibers.size() > 0) {
      if (m_cond.wait_until(lock, m_fibers.top()->m_expirationTime) ==
          std::cv_status::timeout) {
        if (m_fibers.size() == 0 ||
            m_fibers.top()->m_expirationTime > hal::fpga_clock::now()) {
          continue;
        }

        // If the condition variable timed out, a Fiber is ready to run
        auto& fiber = *m_fibers.top();
        m_fibers.pop();

        lock.unlock();
        fiber.m_callback();
        lock.lock();

        // Reschedule Fiber
        fiber.m_expirationTime = hal::fpga_clock::now() + fiber.m_period;
        m_fibers.emplace(&fiber);
      }
      // Otherwise, a Fiber removed itself from the queue (it notifies the
      // scheduler of this) or a spurious wakeup occurred, so just rewait with
      // the soonest watchdog timeout.
    } else {
      m_cond.wait(lock, [&] { return m_fibers.size() > 0 || !m_active; });
    }
  }
}

FiberScheduler::FiberScheduler() { m_owner.Start(); }

FiberScheduler::FiberScheduler(int priority) : FiberScheduler() {
  auto native = m_owner.GetNativeThreadHandle();
  int32_t status = 0;
  HAL_SetThreadPriority(&native, true, priority, &status);
  wpi_setGlobalErrorWithContext(status, HAL_GetErrorMessage(status));
}

FiberScheduler::~FiberScheduler() {
  {
    // Locks mutex
    auto thr = m_owner.GetThread();
    if (!thr) return;

    while (!thr->m_fibers.empty()) {
      thr->m_fibers.pop();
    }
  }
  m_owner.Join();
}

void FiberScheduler::Add(Fiber& fiber) {
  auto startTime = hal::fpga_clock::now();

  // Locks mutex
  auto thr = m_owner.GetThread();
  if (!thr) return;

  thr->m_fibers.remove(&fiber);
  fiber.m_expirationTime = startTime + fiber.m_period;
  thr->m_fibers.emplace(&fiber);
  thr->m_cond.notify_all();
}

void FiberScheduler::Remove(Fiber& fiber) {
  // Locks mutex
  auto thr = m_owner.GetThread();
  if (!thr) return;

  thr->m_fibers.remove(&fiber);
  thr->m_cond.notify_all();
}
