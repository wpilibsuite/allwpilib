/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#ifndef WPIUTIL_WPI_UV_TIMER_H_
#define WPIUTIL_WPI_UV_TIMER_H_

#include <uv.h>

#include <chrono>
#include <functional>
#include <memory>

#include "wpi/Signal.h"
#include "wpi/uv/Handle.h"

namespace wpi {
namespace uv {

class Loop;

/**
 * Timer handle.
 * Timer handles are used to schedule signals to be called in the future.
 */
class Timer final : public HandleImpl<Timer, uv_timer_t> {
  struct private_init {};

 public:
  using Time = std::chrono::duration<uint64_t, std::milli>;

  explicit Timer(const private_init&) {}
  ~Timer() noexcept override = default;

  /**
   * Create a timer handle.
   *
   * @param loop Loop object where this handle runs.
   */
  static std::shared_ptr<Timer> Create(Loop& loop);

  /**
   * Create a timer handle.
   *
   * @param loop Loop object where this handle runs.
   */
  static std::shared_ptr<Timer> Create(const std::shared_ptr<Loop>& loop) {
    return Create(*loop);
  }

  /**
   * Create a timer that calls a functor after a given time interval.
   *
   * @param loop Loop object where the timer should run.
   * @param timeout Time interval
   * @param func Functor
   */
  static void SingleShot(Loop& loop, Time timeout, std::function<void()> func);

  /**
   * Create a timer that calls a functor after a given time interval.
   *
   * @param loop Loop object where the timer should run.
   * @param timeout Time interval
   * @param func Functor
   */
  static void SingleShot(const std::shared_ptr<Loop>& loop, Time timeout,
                         std::function<void()> func) {
    return SingleShot(*loop, timeout, func);
  }

  /**
   * Start the timer.
   *
   * If timeout is zero, an event is emitted on the next event loop
   * iteration. If repeat is non-zero, an event is emitted first
   * after timeout milliseconds and then repeatedly after repeat milliseconds.
   *
   * @param timeout Milliseconds before to emit an event (use
   * `std::chrono::duration<uint64_t, std::milli>`).
   * @param repeat Milliseconds between successive events (use
   * `std::chrono::duration<uint64_t, std::milli>`).
   */
  void Start(Time timeout, Time repeat = Time{0});

  /**
   * Stop the timer.
   */
  void Stop() { Invoke(&uv_timer_stop, GetRaw()); }

  /**
   * Stop the timer and restart it if it was repeating.
   *
   * Stop the timer, and if it is repeating restart it using the repeat value
   * as the timeout.
   * If the timer has never been started before it emits sigError.
   */
  void Again() { Invoke(&uv_timer_again, GetRaw()); }

  /**
   * Set the repeat interval value.
   *
   * The timer will be scheduled to run on the given interval and will follow
   * normal timer semantics in the case of a time-slice overrun.
   * For example, if a 50ms repeating timer first runs for 17ms, it will be
   * scheduled to run again 33ms later. If other tasks consume more than the
   * 33ms following the first timer event, then another event will be emitted
   * as soon as possible.
   *
   * If the repeat value is set from a listener bound to an event, it does
   * not immediately take effect. If the timer was non-repeating before, it
   * will have been stopped. If it was repeating, then the old repeat value
   * will have been used to schedule the next timeout.
   *
   * @param repeat Repeat interval in milliseconds (use
   * `std::chrono::duration<uint64_t, std::milli>`).
   */
  void SetRepeat(Time repeat) { uv_timer_set_repeat(GetRaw(), repeat.count()); }

  /**
   * Get the timer repeat value.
   * @return Timer repeat value in milliseconds (as a
   * `std::chrono::duration<uint64_t, std::milli>`).
   */
  Time GetRepeat() const { return Time{uv_timer_get_repeat(GetRaw())}; }

  /**
   * Signal generated when the timeout event occurs.
   */
  sig::Signal<> timeout;
};

}  // namespace uv
}  // namespace wpi

#endif  // WPIUTIL_WPI_UV_TIMER_H_
