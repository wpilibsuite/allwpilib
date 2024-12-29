// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/event/BooleanEvent.h"

#include <memory>
#include <utility>

using namespace frc;

BooleanEvent::BooleanEvent(EventLoop* loop, std::function<bool()> condition)
    : m_loop(loop), m_signal(std::move(condition)) {
  m_state = std::make_shared<bool>(m_signal());
  m_loop->Bind(
      // NOLINTNEXTLINE(clang-analyzer-cplusplus.NewDeleteLeaks)
      [condition = m_signal, state = m_state] { *state = condition(); });
}

bool BooleanEvent::GetAsBoolean() const {
  return *m_state;
}

BooleanEvent::operator std::function<bool()>() {
  return [state = m_state] { return *state; };
}

void BooleanEvent::IfHigh(std::function<void()> action) {
  m_loop->Bind([state = m_state, action = std::move(action)] {
    if (*state) {
      action();
    }
  });
}

BooleanEvent BooleanEvent::operator!() {
  return BooleanEvent(this->m_loop, [state = m_state] { return !*state; });
}

BooleanEvent BooleanEvent::operator&&(std::function<bool()> rhs) {
  return BooleanEvent(this->m_loop,
                      [state = m_state, rhs] { return *state && rhs(); });
}

BooleanEvent BooleanEvent::operator||(std::function<bool()> rhs) {
  return BooleanEvent(this->m_loop,
                      [state = m_state, rhs] { return *state || rhs(); });
}

BooleanEvent BooleanEvent::Rising() {
  return BooleanEvent(this->m_loop,
                      [state = m_state, m_previous = *m_state]() mutable {
                        bool present = *state;
                        bool past = m_previous;
                        m_previous = present;
                        return !past && present;
                      });
}

BooleanEvent BooleanEvent::Falling() {
  return BooleanEvent(this->m_loop,
                      [state = m_state, m_previous = *m_state]() mutable {
                        bool present = *state;
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
       state = m_state]() mutable { return debouncer.Calculate(*state); });
}
