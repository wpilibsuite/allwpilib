// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#ifndef WPINET_WPINET_SRC_MAIN_NATIVE_INCLUDE_WPI_NET_UV_CHECK_HPP_
#define WPINET_WPINET_SRC_MAIN_NATIVE_INCLUDE_WPI_NET_UV_CHECK_HPP_

#include <uv.h>

#include <memory>

#include "wpi/net/uv/Handle.hpp"
#include "wpi/util/Signal.h"

namespace wpi::net::uv {

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
  wpi::util::sig::Signal<> check;
};

}  // namespace wpi::net::uv

#endif  // WPINET_WPINET_SRC_MAIN_NATIVE_INCLUDE_WPI_NET_UV_CHECK_HPP_
