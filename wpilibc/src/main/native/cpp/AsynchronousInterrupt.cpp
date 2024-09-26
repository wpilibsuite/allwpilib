// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/AsynchronousInterrupt.h"

#include <frc/DigitalSource.h>

#include <memory>
#include <utility>

using namespace frc;

AsynchronousInterrupt::AsynchronousInterrupt(
    DigitalSource& source, std::function<void(bool, bool)> callback)
    : m_interrupt{source}, m_callback{std::move(callback)} {}
AsynchronousInterrupt::AsynchronousInterrupt(
    DigitalSource* source, std::function<void(bool, bool)> callback)
    : m_interrupt{source}, m_callback{std::move(callback)} {}
AsynchronousInterrupt::AsynchronousInterrupt(
    std::shared_ptr<DigitalSource> source,
    std::function<void(bool, bool)> callback)
    : m_interrupt{source}, m_callback{std::move(callback)} {}

AsynchronousInterrupt::~AsynchronousInterrupt() {
  Disable();
}

void AsynchronousInterrupt::ThreadMain() {
  while (m_keepRunning) {
    auto result = m_interrupt.WaitForInterrupt(10_s, false);
    if (!m_keepRunning) {
      break;
    }
    if (result == SynchronousInterrupt::WaitResult::kTimeout) {
      continue;
    }
    m_callback((result & SynchronousInterrupt::WaitResult::kRisingEdge) != 0,
               (result & SynchronousInterrupt::WaitResult::kFallingEdge) != 0);
  }
}

void AsynchronousInterrupt::Enable() {
  if (m_keepRunning) {
    return;
  }

  m_keepRunning = true;
  m_thread = std::thread([this] { this->ThreadMain(); });
}

void AsynchronousInterrupt::Disable() {
  m_keepRunning = false;
  m_interrupt.WakeupWaitingInterrupt();
  if (m_thread.joinable()) {
    m_thread.join();
  }
}

void AsynchronousInterrupt::SetInterruptEdges(bool risingEdge,
                                              bool fallingEdge) {
  m_interrupt.SetInterruptEdges(risingEdge, fallingEdge);
}

units::second_t AsynchronousInterrupt::GetRisingTimestamp() {
  return m_interrupt.GetRisingTimestamp();
}
units::second_t AsynchronousInterrupt::GetFallingTimestamp() {
  return m_interrupt.GetFallingTimestamp();
}
