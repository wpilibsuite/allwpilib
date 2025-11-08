// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/event/EventLoop.hpp"

#include <utility>

#include "wpi/system/Errors.hpp"

using namespace wpi;

namespace {
struct RunningSetter {
  bool& m_running;
  explicit RunningSetter(bool& running) noexcept : m_running{running} {
    m_running = true;
  }
  ~RunningSetter() noexcept { m_running = false; }
};
}  // namespace

EventLoop::EventLoop() {}

void EventLoop::Bind(wpi::util::unique_function<void()> action) {
  if (m_running) {
    throw WPILIB_MakeError(err::Error,
                           "Cannot bind EventLoop while it is running");
  }
  m_bindings.emplace_back(std::move(action));
}

void EventLoop::Poll() {
  RunningSetter runSetter{m_running};
  for (wpi::util::unique_function<void()>& action : m_bindings) {
    action();
  }
}

void EventLoop::Clear() {
  if (m_running) {
    throw WPILIB_MakeError(err::Error,
                           "Cannot clear EventLoop while it is running");
  }
  m_bindings.clear();
}
