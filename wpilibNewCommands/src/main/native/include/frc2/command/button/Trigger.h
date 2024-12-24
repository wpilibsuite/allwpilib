// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <functional>
#include <utility>

#include <frc/event/BooleanEvent.h>
#include <frc/event/EventLoop.h>
#include <frc/filter/Debouncer.h>
#include <units/time.h>
#include <wpi/FunctionExtras.h>

#include "frc2/command/Command.h"
#include "frc2/command/CommandScheduler.h"

namespace frc2 {
class Command;
/**
 * This class provides an easy way to link commands to conditions.
 *
 * <p>It is very easy to link a button to a command. For instance, you could
 * link the trigger button of a joystick to a "score" command.
 *
 * <p>Triggers can easily be composed for advanced functionality using the
 * {@link #operator!}, {@link #operator||}, {@link #operator&&} operators.
 *
 * <p>This class is provided by the NewCommands VendorDep
 */
class Trigger {
 public:
  /**
   * Enum specifying the initial state to use for a binding. This impacts
   * whether or not the binding will be triggered immediately.
   */
  enum struct InitialState {
    /**
     * Indicates the binding should use false as the initial value. This causes
     * a rising edge at the start if and only if the condition starts true.
     */
    kFalse,
    /**
     * Indicates the binding should use true as the initial value. This causes a
     * falling edge at the start if and only if the condition starts false.
     */
    kTrue,
    /**
     * Indicates the binding should use the trigger's condition as the initial
     * value. This never causes an edge at the start. This is the default
     * behavior.
     */
    kCondition,
    /**
     * Indicates the binding should use the negated trigger's condition as the
     * initial value. This always causes an edge at the start. Rising or falling
     * depends on if the condition starts true or false, respectively.
     */
    kNegCondition
  };

  /**
   * Creates a new trigger based on the given condition.
   *
   * <p>Polled by the default scheduler button loop.
   *
   * @param condition the condition represented by this trigger
   */
  explicit Trigger(std::function<bool()> condition)
      : Trigger{CommandScheduler::GetInstance().GetDefaultButtonLoop(),
                std::move(condition)} {}

  /**
   * Creates a new trigger based on the given condition.
   *
   * @param loop The loop instance that polls this trigger.
   * @param condition the condition represented by this trigger
   */
  Trigger(frc::EventLoop* loop, std::function<bool()> condition)
      : m_loop{loop}, m_condition{std::move(condition)} {}

  /**
   * Create a new trigger that is always `false`.
   */
  Trigger() : Trigger([] { return false; }) {}

  Trigger(const Trigger& other);

  /**
   * Starts the command when the condition changes.
   *
   * @param command the command to start
   * @param initialState the initial state to use, kCondition (no initial edge)
   * by default
   * @return this trigger, so calls can be chained
   */
  Trigger OnChange(Command* command,
                   InitialState initialState = InitialState::kCondition);

  /**
   * Starts the command when the condition changes. Moves command ownership to
   * the button scheduler.
   *
   * @param command the command to start
   * @param initialState the initial state to use, kCondition (no initial edge)
   * by default
   * @return this trigger, so calls can be chained
   */
  Trigger OnChange(CommandPtr&& command,
                   InitialState initialState = InitialState::kCondition);

  /**
   * Starts the given command whenever the condition changes from `false` to
   * `true`.
   *
   * <p>Takes a raw pointer, and so is non-owning; users are responsible for the
   * lifespan of the command.
   *
   * @param command the command to start
   * @param initialState the initial state to use, kCondition (no initial edge)
   * by default
   * @return this trigger, so calls can be chained
   */
  Trigger OnTrue(Command* command,
                 InitialState initialState = InitialState::kCondition);

  /**
   * Starts the given command whenever the condition changes from `false` to
   * `true`. Moves command ownership to the button scheduler.
   *
   * @param command The command to bind.
   * @param initialState the initial state to use, kCondition (no initial edge)
   * by default
   * @return The trigger, for chained calls.
   */
  Trigger OnTrue(CommandPtr&& command,
                 InitialState initialState = InitialState::kCondition);

  /**
   * Starts the given command whenever the condition changes from `true` to
   * `false`.
   *
   * <p>Takes a raw pointer, and so is non-owning; users are responsible for the
   * lifespan of the command.
   *
   * @param command the command to start
   * @param initialState the initial state to use, kCondition (no initial edge)
   * by default
   * @return this trigger, so calls can be chained
   */
  Trigger OnFalse(Command* command,
                  InitialState initialState = InitialState::kCondition);

  /**
   * Starts the given command whenever the condition changes from `true` to
   * `false`.
   *
   * @param command The command to bind.
   * @param initialState the initial state to use, kCondition (no initial edge)
   * by default
   * @return The trigger, for chained calls.
   */
  Trigger OnFalse(CommandPtr&& command,
                  InitialState initialState = InitialState::kCondition);

  /**
   * Starts the given command when the condition changes to `true` and cancels
   * it when the condition changes to `false`.
   *
   * <p>Doesn't re-start the command if it ends while the condition is still
   * `true`. If the command should restart, see RepeatCommand.
   *
   * <p>Takes a raw pointer, and so is non-owning; users are responsible for the
   * lifespan of the command.
   *
   * @param command the command to start
   * @param initialState the initial state to use, kCondition (no initial edge)
   * by default
   * @return this trigger, so calls can be chained
   */
  Trigger WhileTrue(Command* command,
                    InitialState initialState = InitialState::kCondition);

  /**
   * Starts the given command when the condition changes to `true` and cancels
   * it when the condition changes to `false`. Moves command ownership to the
   * button scheduler.
   *
   * <p>Doesn't re-start the command if it ends while the condition is still
   * `true`. If the command should restart, see RepeatCommand.
   *
   * @param command The command to bind.
   * @param initialState the initial state to use, kCondition (no initial edge)
   * by default
   * @return The trigger, for chained calls.
   */
  Trigger WhileTrue(CommandPtr&& command,
                    InitialState initialState = InitialState::kCondition);

  /**
   * Starts the given command when the condition changes to `false` and cancels
   * it when the condition changes to `true`.
   *
   * <p>Doesn't re-start the command if it ends while the condition is still
   * `true`. If the command should restart, see RepeatCommand.
   *
   * <p>Takes a raw pointer, and so is non-owning; users are responsible for the
   * lifespan of the command.
   *
   * @param command the command to start
   * @param initialState the initial state to use, kCondition (no initial edge)
   * by default
   * @return this trigger, so calls can be chained
   */
  Trigger WhileFalse(Command* command,
                     InitialState initialState = InitialState::kCondition);

  /**
   * Starts the given command when the condition changes to `false` and cancels
   * it when the condition changes to `true`. Moves command ownership to the
   * button scheduler.
   *
   * <p>Doesn't re-start the command if it ends while the condition is still
   * `false`. If the command should restart, see RepeatCommand.
   *
   * @param command The command to bind.
   * @param initialState the initial state to use, kCondition (no initial edge)
   * by default
   * @return The trigger, for chained calls.
   */
  Trigger WhileFalse(CommandPtr&& command,
                     InitialState initialState = InitialState::kCondition);

  /**
   * Toggles a command when the condition changes from `false` to `true`.
   *
   * <p>Takes a raw pointer, and so is non-owning; users are responsible for the
   * lifespan of the command.
   *
   * @param command the command to toggle
   * @param initialState the initial state to use, kCondition (no initial edge)
   * by default
   * @return this trigger, so calls can be chained
   */
  Trigger ToggleOnTrue(Command* command,
                       InitialState initialState = InitialState::kCondition);

  /**
   * Toggles a command when the condition changes from `false` to `true`.
   *
   * <p>Takes a raw pointer, and so is non-owning; users are responsible for the
   * lifespan of the command.
   *
   * @param command the command to toggle
   * @param initialState the initial state to use, kCondition (no initial edge)
   * by default
   * @return this trigger, so calls can be chained
   */
  Trigger ToggleOnTrue(CommandPtr&& command,
                       InitialState initialState = InitialState::kCondition);

  /**
   * Toggles a command when the condition changes from `true` to the low
   * state.
   *
   * <p>Takes a raw pointer, and so is non-owning; users are responsible for the
   * lifespan of the command.
   *
   * @param command the command to toggle
   * @param initialState the initial state to use, kCondition (no initial edge)
   * by default
   * @return this trigger, so calls can be chained
   */
  Trigger ToggleOnFalse(Command* command,
                        InitialState initialState = InitialState::kCondition);

  /**
   * Toggles a command when the condition changes from `true` to `false`.
   *
   * <p>Takes a raw pointer, and so is non-owning; users are responsible for the
   * lifespan of the command.
   *
   * @param command the command to toggle
   * @param initialState the initial state to use, kCondition (no initial edge)
   * by default
   * @return this trigger, so calls can be chained
   */
  Trigger ToggleOnFalse(CommandPtr&& command,
                        InitialState initialState = InitialState::kCondition);

  /**
   * Composes two triggers with logical AND.
   *
   * @return A trigger which is active when both component triggers are active.
   */
  Trigger operator&&(std::function<bool()> rhs) {
    return Trigger(m_loop, [condition = m_condition, rhs = std::move(rhs)] {
      return condition() && rhs();
    });
  }

  /**
   * Composes two triggers with logical AND.
   *
   * @return A trigger which is active when both component triggers are active.
   */
  Trigger operator&&(Trigger rhs) {
    return Trigger(m_loop, [condition = m_condition, rhs] {
      return condition() && rhs.m_condition();
    });
  }

  /**
   * Composes two triggers with logical OR.
   *
   * @return A trigger which is active when either component trigger is active.
   */
  Trigger operator||(std::function<bool()> rhs) {
    return Trigger(m_loop, [condition = m_condition, rhs = std::move(rhs)] {
      return condition() || rhs();
    });
  }

  /**
   * Composes two triggers with logical OR.
   *
   * @return A trigger which is active when either component trigger is active.
   */
  Trigger operator||(Trigger rhs) {
    return Trigger(m_loop, [condition = m_condition, rhs] {
      return condition() || rhs.m_condition();
    });
  }

  /**
   * Composes a trigger with logical NOT.
   *
   * @return A trigger which is active when the component trigger is inactive,
   * and vice-versa.
   */
  Trigger operator!() {
    return Trigger(m_loop, [condition = m_condition] { return !condition(); });
  }

  /**
   * Creates a new debounced trigger from this trigger - it will become active
   * when this trigger has been active for longer than the specified period.
   *
   * @param debounceTime The debounce period.
   * @param type The debounce type.
   * @return The debounced trigger.
   */
  Trigger Debounce(units::second_t debounceTime,
                   frc::Debouncer::DebounceType type =
                       frc::Debouncer::DebounceType::kRising);

  /**
   * Returns the current state of this trigger.
   *
   * @return A bool representing the current state of the trigger.
   */
  bool Get() const;

 private:
  /**
   * Gets the initial state for a binding based on an initial state policy.
   *
   * @param initialState Initial state policy.
   * @return The initial state to use.
   */
  bool GetInitialState(InitialState initialState) const {
    switch (initialState) {
      case InitialState::kFalse:
        return false;
      case InitialState::kTrue:
        return true;
      case InitialState::kCondition:
        return m_condition();
      case InitialState::kNegCondition:
        return !m_condition();
    }
    return false;
  }

  /**
   * Adds a binding to the EventLoop.
   *
   * @param body The body of the binding to add.
   */
  void AddBinding(wpi::unique_function<void(bool, bool)>&& body,
                  InitialState initialState);

  frc::EventLoop* m_loop;
  std::function<bool()> m_condition;
};
}  // namespace frc2
