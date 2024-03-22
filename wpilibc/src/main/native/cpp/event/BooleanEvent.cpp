// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/event/BooleanEvent.h"

using namespace frc;

BooleanEvent::BooleanEvent(EventLoop* loop, std::function<bool()> condition)
    : m_loop(loop), m_condition(std::move(condition)) {}

BooleanEvent::operator std::function<bool()>() {
  return m_condition;
}

bool BooleanEvent::GetAsBoolean() const {
  return m_condition();
}

void BooleanEvent::IfHigh(std::function<void()> action) {
  m_loop->Bind([condition = m_condition, action = std::move(action)] {
    if (condition()) {
      action();
    }
  });
}

BooleanEvent BooleanEvent::operator!() {
  return BooleanEvent(this->m_loop, [lhs = m_condition] { return !lhs(); });
}

BooleanEvent BooleanEvent::operator&&(std::function<bool()> rhs) {
  return BooleanEvent(this->m_loop,
                      [lhs = m_condition, rhs] { return lhs() && rhs(); });
}

BooleanEvent BooleanEvent::operator||(std::function<bool()> rhs) {
  return BooleanEvent(this->m_loop,
                      [lhs = m_condition, rhs] { return lhs() || rhs(); });
}

BooleanEvent BooleanEvent::Rising() {
  return BooleanEvent(
      this->m_loop, [lhs = m_condition, m_previous = m_condition()]() mutable {
        bool present = lhs();
        bool past = m_previous;
        m_previous = present;
        return !past && present;
      });
}

BooleanEvent BooleanEvent::Falling() {
  return BooleanEvent(
      this->m_loop, [lhs = m_condition, m_previous = m_condition()]() mutable {
        bool present = lhs();
        bool past = m_previous;
        m_previous = present;
        return past && !present;
      });
}

BooleanEvent BooleanEvent::Debounce(units::second_t debounceTime,
                                    frc::Debouncer::DebounceType type) {
  return BooleanEvent(
      this->m_loop,
      [debouncer = frc::Debouncer(debounceTime, type),
       lhs = m_condition]() mutable { return debouncer.Calculate(lhs()); });
}
