// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#ifndef WPIUTIL_WPI_UV_IDLE_H_
#define WPIUTIL_WPI_UV_IDLE_H_

#include <uv.h>

#include <memory>

#include "wpi/Signal.h"
#include "wpi/uv/Handle.h"

namespace wpi::uv {

class Loop;

/**
 * Idle handle.
 *
 * Idle handles will generate a signal once per loop iteration, right
 * before the Prepare handles.
 *
 * The notable difference with Prepare handles is that when there are active
 * idle handles, the loop will perform a zero timeout poll instead of blocking
 * for I/O.
 *
 * @warning Despite the name, idle handles will signal every loop iteration,
 * not when the loop is actually "idle".  This also means they can easly become
 * CPU hogs.
 */
class Idle final : public HandleImpl<Idle, uv_idle_t> {
  struct private_init {};

 public:
  explicit Idle(const private_init&) {}
  ~Idle() noexcept override = default;

  /**
   * Create an idle handle.
   *
   * @param loop Loop object where this handle runs.
   */
  static std::shared_ptr<Idle> Create(Loop& loop);

  /**
   * Create an idle handle.
   *
   * @param loop Loop object where this handle runs.
   */
  static std::shared_ptr<Idle> Create(const std::shared_ptr<Loop>& loop) {
    return Create(*loop);
  }

  /**
   * Start the handle.
   */
  void Start();

  /**
   * Stop the handle.  The signal will no longer be generated.
   */
  void Stop() { Invoke(&uv_idle_stop, GetRaw()); }

  /**
   * Signal generated once per loop iteration prior to Prepare signals.
   */
  sig::Signal<> idle;
};

}  // namespace wpi::uv

#endif  // WPIUTIL_WPI_UV_IDLE_H_
