// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#ifndef WPIUTIL_WPI_UV_SIGNAL_H_
#define WPIUTIL_WPI_UV_SIGNAL_H_

#include <uv.h>

#include <memory>

#include "wpi/Signal.h"
#include "wpi/uv/Handle.h"

namespace wpi::uv {

class Loop;

/**
 * Signal handle.
 */
class Signal final : public HandleImpl<Signal, uv_signal_t> {
  struct private_init {};

 public:
  explicit Signal(const private_init&) {}
  ~Signal() noexcept override = default;

  /**
   * Create a signal handle.
   *
   * @param loop Loop object where this handle runs.
   */
  static std::shared_ptr<Signal> Create(Loop& loop);

  /**
   * Create a signal handle.
   *
   * @param loop Loop object where this handle runs.
   */
  static std::shared_ptr<Signal> Create(const std::shared_ptr<Loop>& loop) {
    return Create(*loop);
  }

  /**
   * Start watching for the given signal.
   *
   * @param signum Signal to watch for.
   */
  void Start(int signum);

  /**
   * Start watching for the given signal.  Same as Start() but the signal
   * handler is reset the moment the signal is received.
   *
   * @param signum Signal to watch for.
   */
  void StartOneshot(int signum);

  /**
   * Stop watching for the signal.
   */
  void Stop() { Invoke(&uv_signal_stop, GetRaw()); }

  /**
   * Get the signal being monitored.
   * @return Signal number.
   */
  int GetSignal() const { return GetRaw()->signum; }

  /**
   * Signal generated when a signal occurs.
   */
  sig::Signal<int> signal;
};

}  // namespace wpi::uv

#endif  // WPIUTIL_WPI_UV_SIGNAL_H_
