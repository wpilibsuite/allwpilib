// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/event/EventLoop.h"

#include <utility>

#include "frc/Errors.h"

using namespace frc;

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

void EventLoop::Bind(wpi::unique_function<void()> action) {
  if (m_running) {
    throw FRC_MakeError(err::Error,
                        "Cannot bind EventLoop while it is running");
  }
  m_bindings.emplace_back(std::move(action));
}

void EventLoop::Poll() {
  RunningSetter runSetter{m_running};
  for (wpi::unique_function<void()>& action : m_bindings) {
    action();
  }
}

void EventLoop::Clear() {
  if (m_running) {
    throw FRC_MakeError(err::Error,
                        "Cannot clear EventLoop while it is running");
  }
  m_bindings.clear();
}
