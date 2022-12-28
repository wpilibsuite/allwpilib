// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/event/EventLoop.h"

using namespace frc;

EventLoop::EventLoop() {}

void EventLoop::Bind(wpi::unique_function<void()> action) {
  m_bindings.emplace_back(std::move(action));
}

void EventLoop::Poll() {
  for (wpi::unique_function<void()>& action : m_bindings) {
    action();
  }
}

void EventLoop::Clear() {
  m_bindings.clear();
}
