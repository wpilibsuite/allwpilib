// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#ifndef WPIUTIL_WPI_UV_PREPARE_H_
#define WPIUTIL_WPI_UV_PREPARE_H_

#include <uv.h>

#include <memory>

#include "wpi/Signal.h"
#include "wpi/uv/Handle.h"

namespace wpi::uv {

class Loop;

/**
 * Prepare handle.
 * Prepare handles will generate a signal once per loop iteration, right
 * before polling for I/O.
 */
class Prepare final : public HandleImpl<Prepare, uv_prepare_t> {
  struct private_init {};

 public:
  explicit Prepare(const private_init&) {}
  ~Prepare() noexcept override = default;

  /**
   * Create a prepare handle.
   *
   * @param loop Loop object where this handle runs.
   */
  static std::shared_ptr<Prepare> Create(Loop& loop);

  /**
   * Create a prepare handle.
   *
   * @param loop Loop object where this handle runs.
   */
  static std::shared_ptr<Prepare> Create(const std::shared_ptr<Loop>& loop) {
    return Create(*loop);
  }

  /**
   * Start the handle.
   */
  void Start();

  /**
   * Stop the handle.  The signal will no longer be generated.
   */
  void Stop() { Invoke(&uv_prepare_stop, GetRaw()); }

  /**
   * Signal generated once per loop iteration prior to polling for I/O.
   */
  sig::Signal<> prepare;
};

}  // namespace wpi::uv

#endif  // WPIUTIL_WPI_UV_PREPARE_H_
