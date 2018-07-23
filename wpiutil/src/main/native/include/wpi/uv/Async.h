/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#ifndef WPIUTIL_WPI_UV_ASYNC_H_
#define WPIUTIL_WPI_UV_ASYNC_H_

#include <uv.h>

#include <memory>

#include "wpi/Signal.h"
#include "wpi/uv/Handle.h"

namespace wpi {
namespace uv {

class Loop;

/**
 * Async handle.
 * Async handles allow the user to "wakeup" the event loop and have a signal
 * generated from another thread.
 */
class Async final : public HandleImpl<Async, uv_async_t> {
  struct private_init {};

 public:
  explicit Async(const private_init&) {}
  ~Async() noexcept override = default;

  /**
   * Create an async handle.
   *
   * @param loop Loop object where this handle runs.
   */
  static std::shared_ptr<Async> Create(Loop& loop);

  /**
   * Create an async handle.
   *
   * @param loop Loop object where this handle runs.
   */
  static std::shared_ptr<Async> Create(const std::shared_ptr<Loop>& loop) {
    return Create(*loop);
  }

  /**
   * Wakeup the event loop and emit the event.
   *
   * It’s safe to call this function from any thread.
   * An async event will be emitted on the loop thread.
   */
  void Send() { Invoke(&uv_async_send, GetRaw()); }

  /**
   * Signal generated (on event loop thread) when the async event occurs.
   */
  sig::Signal<> wakeup;
};

}  // namespace uv
}  // namespace wpi

#endif  // WPIUTIL_WPI_UV_ASYNC_H_
