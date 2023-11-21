// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <frc/filter/Debouncer.h>

#include <functional>
#include <memory>

#include <units/time.h>
#include <wpi/FunctionExtras.h>

#include "EventLoop.h"

namespace frc {

/**
 * This class provides an easy way to link actions to inputs. Each object
 * represents a boolean condition to which callback actions can be bound using
 * {@link #IfHigh(std::function<void()>)}.
 *
 * <p>These events can easily be composed using factories such as {@link
 * #operator!},
 * {@link #operator||}, {@link #operator&&} etc.
 *
 * <p>To get an event that activates only when this one changes, see {@link
 * #Falling()} and {@link #Rising()}.
 */
class BooleanEvent {
 public:
  /**
   * Creates a new event with the given condition determining whether it is
   * active.
   *
   * @param loop the loop that polls this event
   * @param condition returns whether or not the event should be active
   */
  BooleanEvent(EventLoop* loop, std::function<bool()> condition);

  /**
   * Check whether this event is active or not as of the last loop poll.
   *
   * @return true if active, false if not active. If the event was never polled,
   * it returns the state at event construction.
   */
  bool GetAsBoolean() const;

  /**
   * Bind an action to this event.
   *
   * @param action the action to run if this event is active.
   */
  void IfHigh(std::function<void()> action);

  operator std::function<bool()>();  // NOLINT

  /**
   * A method to "downcast" a BooleanEvent instance to a subclass (for example,
   * to a command-based version of this class).
   *
   * @param ctor a method reference to the constructor of the subclass that
   * accepts the loop as the first parameter and the condition/signal as the
   * second.
   * @return an instance of the subclass.
   */
  template <class T>
  T CastTo(std::function<T(EventLoop*, std::function<bool()>)> ctor =
               [](EventLoop* loop, std::function<bool()> condition) {
                 return T(loop, condition);
               }) {
    return ctor(m_loop, [state = m_state] { return *state; });
  }

  /**
   * Creates a new event that is active when this event is inactive, i.e. that
   * acts as the negation of this event.
   *
   * @return the negated event
   */
  BooleanEvent operator!();

  /**
   * Composes this event with another event, returning a new event that is
   * active when both events are active.
   *
   * <p>The events must use the same event loop. If the events use different
   * event loops, the composed signal won't update until both loops are polled.
   *
   * @param rhs the event to compose with
   * @return the event that is active when both events are active
   */
  BooleanEvent operator&&(std::function<bool()> rhs);

  /**
   * Composes this event with another event, returning a new event that is
   * active when either event is active.
   *
   * <p>The events must use the same event loop. If the events use different
   * event loops, the composed signal won't update until both loops are polled.
   *
   * @param rhs the event to compose with
   * @return the event that is active when either event is active
   */
  BooleanEvent operator||(std::function<bool()> rhs);

  /**
   * Get a new event that events only when this one newly changes to true.
   *
   * @return a new event representing when this one newly changes to true.
   */
  BooleanEvent Rising();

  /**
   * Get a new event that triggers only when this one newly changes to false.
   *
   * @return a new event representing when this one newly changes to false.
   */
  BooleanEvent Falling();

  /**
   * Creates a new debounced event from this event - it will become active when
   * this event has been active for longer than the specified period.
   *
   * @param debounceTime The debounce period.
   * @param type The debounce type.
   * @return The debounced event.
   */
  BooleanEvent Debounce(units::second_t debounceTime,
                        frc::Debouncer::DebounceType type =
                            frc::Debouncer::DebounceType::kRising);

 private:
  EventLoop* m_loop;
  std::function<bool()> m_condition;
  std::shared_ptr<bool> m_state;  // A programmer's worst nightmare.
};
}  // namespace frc
