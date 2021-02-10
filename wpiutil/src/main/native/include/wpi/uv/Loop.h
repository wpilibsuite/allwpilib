// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#ifndef WPIUTIL_WPI_UV_LOOP_H_
#define WPIUTIL_WPI_UV_LOOP_H_

#include <uv.h>

#include <atomic>
#include <chrono>
#include <functional>
#include <memory>
#include <thread>
#include <utility>

#include "wpi/Signal.h"
#include "wpi/uv/Error.h"

namespace wpi::uv {

class Handle;

/**
 * Event loop.
 *
 * The event loop is the central part of uv functionality.  It takes care of
 * polling for I/O and scheduling signals to be generated based on different
 * sources of events.
 *
 * The event loop is not moveable, copyable, or directly constructible.  Use
 * Create() to create an event loop, or GetDefault() to get the default loop
 * if you know your program will only have a single one.
 */
class Loop final : public std::enable_shared_from_this<Loop> {
  struct private_init {};

 public:
  using Time = std::chrono::duration<uint64_t, std::milli>;

  enum Mode {
    kDefault = UV_RUN_DEFAULT,
    kOnce = UV_RUN_ONCE,
    kNoWait = UV_RUN_NOWAIT
  };

  explicit Loop(const private_init&) noexcept;

  Loop(const Loop&) = delete;
  Loop& operator=(const Loop&) = delete;
  Loop(Loop&& oth) = delete;
  Loop& operator=(Loop&& oth) = delete;

  ~Loop() noexcept;

  /**
   * Create a new event loop.  The created loop is not the default event loop.
   *
   * @return The newly created loop.  May return nullptr if a failure occurs.
   */
  static std::shared_ptr<Loop> Create();

  /**
   * Create the default event loop.  Only use this event loop if a single loop
   * is needed for the entire application.
   *
   * @return The newly created loop.  May return nullptr if a failure occurs.
   */
  static std::shared_ptr<Loop> GetDefault();

  /**
   * Release all internal loop resources.
   *
   * Call this function only when the loop has finished executing and all open
   * handles and requests have been closed, or the loop will emit an error.
   *
   * error() will be emitted in case of errors.
   */
  void Close();

  /**
   * Run the event loop.
   *
   * Available modes are:
   *
   * * `Loop::kDefault`: Run the event loop until there are no
   *                     active and referenced handles or requests.
   * * `Loop::kOnce`: Run a single event loop iteration. Note that this
   *                  function blocksif there are no pending callbacks.
   * * `Loop::kNoWait`: Run a single event loop iteration, but don't block
   *                    if there are no pending callbacks.
   *
   * @return True when done, false in all other cases.
   */
  bool Run(Mode mode = kDefault) {
    m_tid = std::this_thread::get_id();
    int rv = uv_run(m_loop, static_cast<uv_run_mode>(static_cast<int>(mode)));
    m_tid = std::thread::id{};
    return rv == 0;
  }

  /**
   * Check if there are active resources.
   *
   * @return True if there are active resources in the loop.
   */
  bool IsAlive() const noexcept { return uv_loop_alive(m_loop) != 0; }

  /**
   * Stop the event loop.
   *
   * This will cause Run() to end as soon as possible.
   * This will happen not sooner than the next loop iteration.
   * If this function was called before blocking for I/O, the loop won’t block
   * for I/O on this iteration.
   */
  void Stop() noexcept { uv_stop(m_loop); }

  /**
   * Get backend file descriptor.
   *
   * Only kqueue, epoll and event ports are supported.
   * This can be used in conjunction with `run(Loop::kNoWait)` to poll
   * in one thread and run the event loop’s callbacks in another.
   *
   * @return The backend file descriptor.
   */
  int GetDescriptor() const noexcept { return uv_backend_fd(m_loop); }

  /**
   * Get the poll timeout.
   *
   * @return A `std::pair` composed of a boolean value that is true in case of
   * valid timeout, false otherwise, and the timeout
   * (`std::chrono::duration<uint64_t, std::milli>`).
   */
  std::pair<bool, Time> GetTimeout() const noexcept {
    auto to = uv_backend_timeout(m_loop);
    return std::make_pair(to == -1, Time{to});
  }

  /**
   * Return the current timestamp in milliseconds.
   *
   * The timestamp is cached at the start of the event loop tick.
   * The timestamp increases monotonically from some arbitrary point in
   * time.
   * Don’t make assumptions about the starting point, you will only get
   * disappointed.
   *
   * @return The current timestamp in milliseconds (actual type is
   * `std::chrono::duration<uint64_t, std::milli>`).
   */
  Time Now() const noexcept { return Time{uv_now(m_loop)}; }

  /**
   * Update the event loop’s concept of _now_.
   *
   * The current time is cached at the start of the event loop tick in order
   * to reduce the number of time-related system calls.
   * You won’t normally need to call this function unless you have callbacks
   * that block the event loop for longer periods of time, where _longer_ is
   * somewhat subjective but probably on the order of a millisecond or more.
   */
  void UpdateTime() noexcept { uv_update_time(m_loop); }

  /**
   * Walk the list of handles.
   *
   * The callback will be executed once for each handle that is still active.
   *
   * @param callback A function to be invoked once for each active handle.
   */
  void Walk(std::function<void(Handle&)> callback);

  /**
   * Reinitialize any kernel state necessary in the child process after
   * a fork(2) system call.
   *
   * Previously started watchers will continue to be started in the child
   * process.
   *
   * It is necessary to explicitly call this function on every event loop
   * created in the parent process that you plan to continue to use in the
   * child, including the default loop (even if you don’t continue to use it
   * in the parent). This function must be called before calling any API
   * function using the loop in the child. Failure to do so will result in
   * undefined behaviour, possibly including duplicate events delivered to
   * both parent and child or aborting the child process.
   *
   * When possible, it is preferred to create a new loop in the child process
   * instead of reusing a loop created in the parent. New loops created in the
   * child process after the fork should not use this function.
   *
   * Note that this function is not implemented on Windows.
   * Note also that this function is experimental in `libuv`. It may contain
   * bugs, and is subject to change or removal. API and ABI stability is not
   * guaranteed.
   *
   * error() will be emitted in case of errors.
   */
  void Fork();

  /**
   * Get the underlying event loop data structure.
   *
   * @return The underlying event loop data structure.
   */
  uv_loop_t* GetRaw() const noexcept { return m_loop; }

  /**
   * Gets user-defined data.
   * @return User-defined data if any, nullptr otherwise.
   */
  template <typename T = void>
  std::shared_ptr<T> GetData() const {
    return std::static_pointer_cast<T>(m_data);
  }

  /**
   * Sets user-defined data.
   * @param data User-defined arbitrary data.
   */
  void SetData(std::shared_ptr<void> data) { m_data = std::move(data); }

  /**
   * Get the thread id of the loop thread.  If the loop is not currently
   * running, returns default-constructed thread id.
   */
  std::thread::id GetThreadId() const { return m_tid; }

  /**
   * Error signal
   */
  sig::Signal<Error> error;

  /**
   * Reports error.
   * @param err Error code
   */
  void ReportError(int err) { error(Error(err)); }

 private:
  std::shared_ptr<void> m_data;
  uv_loop_t* m_loop;
  uv_loop_t m_loopStruct;
  std::atomic<std::thread::id> m_tid;
};

}  // namespace wpi::uv

#endif  // WPIUTIL_WPI_UV_LOOP_H_
