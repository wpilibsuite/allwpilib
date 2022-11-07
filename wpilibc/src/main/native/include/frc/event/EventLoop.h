// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <functional>
#include <vector>

#include <wpi/FunctionExtras.h>

namespace frc {
/** The loop polling BooleanEvent objects and executing the actions bound to
 * them. */
class EventLoop {
 public:
  EventLoop();

  EventLoop(const EventLoop&) = delete;
  EventLoop& operator=(const EventLoop&) = delete;

  /**
   * Bind a new action to run whenever the condition is true.
   *
   * @param condition the condition to listen to.
   * @param action the action to run.
   */
  void Bind(std::function<bool()> condition,
            wpi::unique_function<void()> action);

  /**
   * Poll all bindings.
   */
  void Poll();

  /**
   * Clear all bindings.
   */
  void Clear();

 private:
  struct Binding {
    std::function<bool()> condition;
    wpi::unique_function<void()> action;

    void Poll();
  };
  std::vector<Binding> m_bindings;
};
}  // namespace frc
