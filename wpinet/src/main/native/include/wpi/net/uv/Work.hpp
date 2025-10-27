// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#ifndef WPINET_UV_WORK_H_
#define WPINET_UV_WORK_H_

#include <uv.h>

#include <functional>
#include <memory>
#include <utility>

#include <wpi/Signal.h>

#include "wpinet/uv/Request.h"

namespace wpi::uv {

class Loop;

/**
 * Work request.
 * For use with `QueueWork()` function family.
 */
class WorkReq : public RequestImpl<WorkReq, uv_work_t> {
 public:
  WorkReq();

  Loop& GetLoop() const { return *static_cast<Loop*>(GetRaw()->loop->data); }

  /**
   * Function(s) that will be run on the thread pool.
   */
  sig::Signal<> work;

  /**
   * Function(s) that will be run on the loop thread after the work on the
   * thread pool has been completed by the work callback.
   */
  sig::Signal<> afterWork;
};

/**
 * Initializes a work request which will run on the thread pool.
 *
 * @param loop Event loop
 * @param req request
 */
void QueueWork(Loop& loop, const std::shared_ptr<WorkReq>& req);

/**
 * Initializes a work request which will run on the thread pool.
 *
 * @param loop Event loop
 * @param req request
 */
inline void QueueWork(const std::shared_ptr<Loop>& loop,
                      const std::shared_ptr<WorkReq>& req) {
  QueueWork(*loop, req);
}

/**
 * Initializes a work request which will run on the thread pool.  The work
 * callback will be run on a thread from the thread pool, and the afterWork
 * callback will be called on the loop thread after the work completes.  Any
 * errors are forwarded to the loop.
 *
 * @param loop Event loop
 * @param work Work callback (called from separate thread)
 * @param afterWork After work callback (called on loop thread)
 */
void QueueWork(Loop& loop, std::function<void()> work,
               std::function<void()> afterWork);

/**
 * Initializes a work request which will run on the thread pool.  The work
 * callback will be run on a thread from the thread pool, and the afterWork
 * callback will be called on the loop thread after the work completes.  Any
 * errors are forwarded to the loop.
 *
 * @param loop Event loop
 * @param work Work callback (called from separate thread)
 * @param afterWork After work callback (called on loop thread)
 */
inline void QueueWork(const std::shared_ptr<Loop>& loop,
                      std::function<void()> work,
                      std::function<void()> afterWork) {
  QueueWork(*loop, std::move(work), std::move(afterWork));
}

}  // namespace wpi::uv

#endif  // WPINET_UV_WORK_H_
