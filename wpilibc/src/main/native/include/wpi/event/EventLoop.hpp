// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <functional>
#include <list>

#include "wpi/util/FunctionExtras.hpp"

namespace wpi {
/** A declarative way to bind a set of actions to a loop and execute them when
 * the loop is polled. */
class EventLoop {
 public:
  EventLoop();

  EventLoop(const EventLoop&) = delete;
  EventLoop& operator=(const EventLoop&) = delete;

  /**
   * Bind a new action to run when the loop is polled.
   *
   * @param action the action to run.
   * @returns A reference to the bound action, so that it may be later unbound.
   */
  wpi::util::unique_function<void()>& Bind(
      wpi::util::unique_function<void()>&& action);

  /**
   * Unbinds an action so that is no longer ran when the loop is polled.
   * This must be provided the same reference that was returned from `Bind`.
   * The reference will no longer be valid after this is called.
   *
   * @param action the action to unbind.
   */
  void Unbind(wpi::util::unique_function<void()>& action);

  /**
   * Poll all bindings.
   */
  void Poll();

  /**
   * Clear all bindings.
   */
  void Clear();

 private:
  std::list<wpi::util::unique_function<void()>> m_bindings;
  bool m_running{false};
};
}  // namespace wpi
