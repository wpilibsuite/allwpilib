/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#ifndef WPIUTIL_WPI_UV_CHECK_H_
#define WPIUTIL_WPI_UV_CHECK_H_

#include <uv.h>

#include <memory>

#include "wpi/Signal.h"
#include "wpi/uv/Handle.h"

namespace wpi {
namespace uv {

class Loop;

/**
 * Check handle.
 * Check handles will generate a signal once per loop iteration, right
 * after polling for I/O.
 */
class Check final : public HandleImpl<Check, uv_check_t> {
  struct private_init {};

 public:
  explicit Check(const private_init&) {}
  ~Check() noexcept override = default;

  /**
   * Create a check handle.
   *
   * @param loop Loop object where this handle runs.
   */
  static std::shared_ptr<Check> Create(Loop& loop);

  /**
   * Create a check handle.
   *
   * @param loop Loop object where this handle runs.
   */
  static std::shared_ptr<Check> Create(const std::shared_ptr<Loop>& loop) {
    return Create(*loop);
  }

  /**
   * Start the handle.
   */
  void Start();

  /**
   * Stop the handle.  The signal will no longer be generated.
   */
  void Stop() { Invoke(&uv_check_stop, GetRaw()); }

  /**
   * Signal generated once per loop iteration after polling for I/O.
   */
  sig::Signal<> check;
};

}  // namespace uv
}  // namespace wpi

#endif  // WPIUTIL_WPI_UV_CHECK_H_
