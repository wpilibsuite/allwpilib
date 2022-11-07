// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/event/EventLoop.h"

using namespace frc;

EventLoop::EventLoop() {}

void EventLoop::Binding::Poll() {
  if (condition()) {
    action();
  }
}

void EventLoop::Bind(std::function<bool()> condition,
                     wpi::unique_function<void()> action) {
  m_bindings.emplace_back(Binding{condition, std::move(action)});
}

void EventLoop::Poll() {
  for (Binding& binding : m_bindings) {
    binding.Poll();
  }
}

void EventLoop::Clear() {
  m_bindings.clear();
}
