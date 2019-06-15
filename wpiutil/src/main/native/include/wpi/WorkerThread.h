/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018-2019 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#ifndef WPIUTIL_WPI_WORKERTHREAD_H_
#define WPIUTIL_WPI_WORKERTHREAD_H_

#include <memory>
#include <utility>

#include "wpi/SafeThread.h"
#include "wpi/ThreadedExecutorBase.h"

namespace wpi {

class WorkerThreadExecutor;

namespace detail {

class WorkerThreadThread : public SafeThread,
                           public ThreadedExecutorBaseHandle {
  friend class ThreadedExecutorBase<WorkerThreadExecutor, WorkerThreadThread>;

 public:
  void Main() override;

 private:
  void Signal() { m_cond.notify_one(); }
  std::thread::id GetThreadId() { return m_threadId; }
};

}  // namespace detail

class WorkerThread final {
 public:
  WorkerThread() { m_owner.Start(); }

  WorkerThreadExecutor GetExecutor();

 private:
  SafeThreadOwner<detail::WorkerThreadThread> m_owner;
};

class WorkerThreadExecutor final
    : public ThreadedExecutorBase<WorkerThreadExecutor,
                                  detail::WorkerThreadThread> {
 public:
  WorkerThreadExecutor() = default;
  explicit WorkerThreadExecutor(
      std::shared_ptr<detail::WorkerThreadThread> handle)
      : ThreadedExecutorBase(std::move(handle)) {}
};

inline WorkerThreadExecutor WorkerThread::GetExecutor() {
  return WorkerThreadExecutor{m_owner.GetThreadSharedPtr()};
}

}  // namespace wpi

#endif  // WPIUTIL_WPI_WORKERTHREAD_H_
