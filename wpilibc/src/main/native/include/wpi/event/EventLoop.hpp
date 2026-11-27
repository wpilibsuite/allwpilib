// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <functional>
#include <vector>

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
   * @returns An id for the bound action, so that it may be later unbound. Only
   * valid within this event loop.
   */
  size_t Bind(wpi::util::unique_function<void()>&& action);

  /**
   * Unbinds an action so that is no longer ran when the loop is polled.
   * This may change the call order of the remaining events.
   *
   * @param actionId the id of the action to unbind.
   */
  void Unbind(size_t actionId);

  /**
   * Poll all bindings.
   */
  void Poll();

  /**
   * Clear all bindings.
   */
  void Clear();

 private:
  size_t nextId = 1;  // Skip id 0 in case it is needed for null later
  struct Binding {
    size_t id;
    wpi::util::unique_function<void()> handler;
  };

  std::vector<Binding> m_bindings;
  bool m_running{false};
};
}  // namespace wpi
