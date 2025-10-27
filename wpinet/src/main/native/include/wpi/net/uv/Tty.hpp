// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#ifndef WPINET_UV_TTY_H_
#define WPINET_UV_TTY_H_

#include <uv.h>

#include <memory>
#include <utility>

#include "wpinet/uv/Stream.h"

namespace wpi::uv {

class Loop;
class Tty;

/**
 * TTY handle.
 * TTY handles represent a stream for the console.
 */
class Tty final : public StreamImpl<Tty, uv_tty_t> {
  struct private_init {};

 public:
  explicit Tty(const private_init&) {}
  ~Tty() noexcept override = default;

  /**
   * Create a TTY handle.
   *
   * @param loop Loop object where this handle runs.
   * @param fd File descriptor, usually 0=stdin, 1=stdout, 2=stderr
   * @param readable Specifies if you plan on calling StartRead().  stdin is
   *                 readable, stdout is not.
   */
  static std::shared_ptr<Tty> Create(Loop& loop, uv_file fd, bool readable);

  /**
   * Create a TTY handle.
   *
   * @param loop Loop object where this handle runs.
   * @param fd File descriptor, usually 0=stdin, 1=stdout, 2=stderr
   * @param readable Specifies if you plan on calling StartRead().  stdin is
   *                 readable, stdout is not.
   */
  static std::shared_ptr<Tty> Create(const std::shared_ptr<Loop>& loop,
                                     uv_file fd, bool readable) {
    return Create(*loop, fd, readable);
  }

  /**
   * Set the TTY using the specified terminal mode.
   *
   * @param mode terminal mode
   */
  void SetMode(uv_tty_mode_t mode) {
    int err = uv_tty_set_mode(GetRaw(), mode);
    if (err < 0) {
      ReportError(err);
    }
  }

  /**
   * Reset TTY settings to default values for the next process to take over.
   * Typically called when the program exits.
   */
  void ResetMode() { Invoke(&uv_tty_reset_mode); }

  /**
   * Gets the current window size.
   * @return Window size (pair of width and height).
   */
  std::pair<int, int> GetWindowSize() {
    int width = 0, height = 0;
    Invoke(&uv_tty_get_winsize, GetRaw(), &width, &height);
    return std::pair{width, height};
  }
};

}  // namespace wpi::uv

#endif  // WPINET_UV_TTY_H_
