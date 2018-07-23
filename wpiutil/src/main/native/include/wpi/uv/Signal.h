/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#ifndef WPIUTIL_WPI_UV_SIGNAL_H_
#define WPIUTIL_WPI_UV_SIGNAL_H_

#include <uv.h>

#include <memory>

#include "wpi/Signal.h"
#include "wpi/uv/Handle.h"

namespace wpi {
namespace uv {

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

}  // namespace uv
}  // namespace wpi

#endif  // WPIUTIL_WPI_UV_SIGNAL_H_
