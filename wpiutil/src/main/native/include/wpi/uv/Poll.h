/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#ifndef WPIUTIL_WPI_UV_POLL_H_
#define WPIUTIL_WPI_UV_POLL_H_

#include <uv.h>

#include <memory>

#include "wpi/Signal.h"
#include "wpi/uv/Handle.h"

namespace wpi {
namespace uv {

class Loop;

/**
 * Poll handle.
 */
class Poll final : public HandleImpl<Poll, uv_poll_t> {
  struct private_init {};

 public:
  explicit Poll(const private_init&) {}
  ~Poll() noexcept override = default;

  /**
   * Create a poll handle using a file descriptor.
   *
   * @param loop Loop object where this handle runs.
   * @param fd File descriptor.
   */
  static std::shared_ptr<Poll> Create(Loop& loop, int fd);

  /**
   * Create a poll handle using a file descriptor.
   *
   * @param loop Loop object where this handle runs.
   * @param fd File descriptor.
   */
  static std::shared_ptr<Poll> Create(const std::shared_ptr<Loop>& loop,
                                      int fd) {
    return Create(*loop, fd);
  }

  /**
   * Create a poll handle using a socket descriptor.
   *
   * @param loop Loop object where this handle runs.
   * @param sock Socket descriptor.
   */
  static std::shared_ptr<Poll> CreateSocket(Loop& loop, uv_os_sock_t sock);

  /**
   * Create a poll handle using a socket descriptor.
   *
   * @param loop Loop object where this handle runs.
   * @param sock Socket descriptor.
   */
  static std::shared_ptr<Poll> CreateSocket(const std::shared_ptr<Loop>& loop,
                                            uv_os_sock_t sock) {
    return CreateSocket(*loop, sock);
  }

  /**
   * Start polling the file descriptor.
   *
   * @param events Bitmask of events (UV_READABLE, UV_WRITEABLE, UV_PRIORITIZED,
   *               and UV_DISCONNECT).
   */
  void Start(int events);

  /**
   * Stop polling the file descriptor.
   */
  void Stop() { Invoke(&uv_poll_stop, GetRaw()); }

  /**
   * Signal generated when a poll event occurs.
   */
  sig::Signal<int> pollEvent;
};

}  // namespace uv
}  // namespace wpi

#endif  // WPIUTIL_WPI_UV_POLL_H_
