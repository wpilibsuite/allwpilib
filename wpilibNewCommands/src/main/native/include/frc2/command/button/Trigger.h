// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <functional>
#include <initializer_list>
#include <memory>
#include <span>
#include <utility>

#include <frc/event/BooleanEvent.h>
#include <frc/event/EventLoop.h>
#include <frc/filter/Debouncer.h>
#include <units/time.h>
#include <wpi/deprecated.h>

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
   * Creates a new trigger based on the given condition.
   *
   * <p>Polled by the default scheduler button loop.
   *
   * @param condition the condition represented by this trigger
   */
  explicit Trigger(std::function<bool()> condition)
      : m_event{CommandScheduler::GetInstance().GetDefaultButtonLoop(),
                std::move(condition)} {}

  /**
   * Creates a new trigger based on the given condition.
   *
   * @param loop The loop instance that polls this trigger.
   * @param condition the condition represented by this trigger
   */
  Trigger(frc::EventLoop* loop, std::function<bool()> condition)
      : m_event{loop, std::move(condition)} {}

  /**
   * Create a new trigger that is always `false`.
   */
  Trigger() : Trigger([] { return false; }) {}

  Trigger(const Trigger& other);

  /**
   * Starts the given command whenever the condition changes from `false` to
   * `true`.
   *
   * <p>Takes a raw pointer, and so is non-owning; users are responsible for the
   * lifespan of the command.
   *
   * @param command the command to start
   * @return this trigger, so calls can be chained
   * @see #Rising()
   */
  Trigger OnTrue(Command* command);

  /**
   * Starts the given command whenever the condition changes from `false` to
   * `true`. Moves command ownership to the button scheduler.
   *
   * @param command The command to bind.
   * @return The trigger, for chained calls.
   */
  Trigger OnTrue(CommandPtr&& command);

  /**
   * Starts the given command whenever the condition changes from `true` to
   * `false`.
   *
   * <p>Takes a raw pointer, and so is non-owning; users are responsible for the
   * lifespan of the command.
   *
   * @param command the command to start
   * @return this trigger, so calls can be chained
   * @see #Falling()
   */
  Trigger OnFalse(Command* command);

  /**
   * Starts the given command whenever the condition changes from `true` to
   * `false`.
   *
   * @param command The command to bind.
   * @return The trigger, for chained calls.
   */
  Trigger OnFalse(CommandPtr&& command);

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
   * @return this trigger, so calls can be chained
   */
  Trigger WhileTrue(Command* command);

  /**
   * Starts the given command when the condition changes to `true` and cancels
   * it when the condition changes to `false`. Moves command ownership to the
   * button scheduler.
   *
   * <p>Doesn't re-start the command if it ends while the condition is still
   * `true`. If the command should restart, see RepeatCommand.
   *
   * @param command The command to bind.
   * @return The trigger, for chained calls.
   */
  Trigger WhileTrue(CommandPtr&& command);

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
   * @return this trigger, so calls can be chained
   */
  Trigger WhileFalse(Command* command);

  /**
   * Starts the given command when the condition changes to `false` and cancels
   * it when the condition changes to `true`. Moves command ownership to the
   * button scheduler.
   *
   * <p>Doesn't re-start the command if it ends while the condition is still
   * `false`. If the command should restart, see RepeatCommand.
   *
   * @param command The command to bind.
   * @return The trigger, for chained calls.
   */
  Trigger WhileFalse(CommandPtr&& command);

  /**
   * Toggles a command when the condition changes from `false` to `true`.
   *
   * <p>Takes a raw pointer, and so is non-owning; users are responsible for the
   * lifespan of the command.
   *
   * @param command the command to toggle
   * @return this trigger, so calls can be chained
   */
  Trigger ToggleOnTrue(Command* command);

  /**
   * Toggles a command when the condition changes from `false` to `true`.
   *
   * <p>Takes a raw pointer, and so is non-owning; users are responsible for the
   * lifespan of the command.
   *
   * @param command the command to toggle
   * @return this trigger, so calls can be chained
   */
  Trigger ToggleOnTrue(CommandPtr&& command);

  /**
   * Toggles a command when the condition changes from `true` to the low
   * state.
   *
   * <p>Takes a raw pointer, and so is non-owning; users are responsible for the
   * lifespan of the command.
   *
   * @param command the command to toggle
   * @return this trigger, so calls can be chained
   */
  Trigger ToggleOnFalse(Command* command);

  /**
   * Toggles a command when the condition changes from `true` to the low
   * state.
   *
   * <p>Takes a raw pointer, and so is non-owning; users are responsible for the
   * lifespan of the command.
   *
   * @param command the command to toggle
   * @return this trigger, so calls can be chained
   */
  Trigger ToggleOnFalse(CommandPtr&& command);

  /**
   * Binds a command to start when the trigger becomes active. Takes a
   * raw pointer, and so is non-owning; users are responsible for the lifespan
   * of the command.
   *
   * @param command The command to bind.
   * @return The trigger, for chained calls.
   * @deprecated Use OnTrue(Command) instead
   */
  WPI_DEPRECATED("Use OnTrue(Command) instead")
  Trigger WhenActive(Command* command);

  /**
   * Binds a command to start when the trigger becomes active. Transfers
   * command ownership to the button scheduler, so the user does not have to
   * worry about lifespan - rvalue refs will be *moved*, lvalue refs will be
   * *copied.*
   *
   * @param command The command to bind.
   * @return The trigger, for chained calls.
   * @deprecated Use OnTrue(Command) instead
   */
  template <class T, typename = std::enable_if_t<std::is_base_of_v<
                         Command, std::remove_reference_t<T>>>>
  WPI_DEPRECATED("Use OnTrue(Command) instead")
  Trigger WhenActive(T&& command) {
    m_event.Rising().IfHigh(
        [command = std::make_unique<std::remove_reference_t<T>>(
             std::forward<T>(command))] { command->Schedule(); });

    return *this;
  }

  /**
   * Binds a runnable to execute when the trigger becomes active.
   *
   * @param toRun the runnable to execute.
   * @param requirements the required subsystems.
   * @deprecated Use OnTrue(Command) instead and construct the InstantCommand
   * manually
   */
  WPI_DEPRECATED(
      "Use OnTrue(Command) instead and construct the InstantCommand manually")
  Trigger WhenActive(std::function<void()> toRun,
                     std::initializer_list<Subsystem*> requirements);

  /**
   * Binds a runnable to execute when the trigger becomes active.
   *
   * @param toRun the runnable to execute.
   * @param requirements the required subsystems.
   * @deprecated Use OnTrue(Command) instead and construct the InstantCommand
   * manually
   */
  WPI_DEPRECATED(
      "Use OnTrue(Command) instead and construct the InstantCommand manually")
  Trigger WhenActive(std::function<void()> toRun,
                     std::span<Subsystem* const> requirements = {});

  /**
   * Binds a command to be started repeatedly while the trigger is active, and
   * canceled when it becomes inactive.  Takes a raw pointer, and so is
   * non-owning; users are responsible for the lifespan of the command.
   *
   * @param command The command to bind.
   * @return The trigger, for chained calls.
   * @deprecated Use WhileTrue(Command) with RepeatCommand, or bind
   command::Schedule with IfHigh(std::function<void()>).
   */
  WPI_DEPRECATED(
      "Use WhileTrue(Command) with RepeatCommand, or bind command::Schedule "
      "with IfHigh(std::function<void()>).")
  Trigger WhileActiveContinous(Command* command);

  /**
   * Binds a command to be started repeatedly while the trigger is active, and
   * canceled when it becomes inactive.  Transfers command ownership to the
   * button scheduler, so the user does not have to worry about lifespan -
   * rvalue refs will be *moved*, lvalue refs will be *copied.*
   *
   * @param command The command to bind.
   * @return The trigger, for chained calls.
   * @deprecated Use WhileTrue(Command) with RepeatCommand, or bind
   command::Schedule with IfHigh(std::function<void()>).
   */
  template <class T, typename = std::enable_if_t<std::is_base_of_v<
                         Command, std::remove_reference_t<T>>>>
  WPI_DEPRECATED(
      "Use WhileTrue(Command) with RepeatCommand, or bind command::Schedule "
      "with IfHigh(std::function<void()>).")
  Trigger WhileActiveContinous(T&& command) {
    std::shared_ptr<T> ptr =
        std::make_shared<std::remove_reference_t<T>>(std::forward<T>(command));
    m_event.IfHigh([ptr] { ptr->Schedule(); });
    m_event.Falling().IfHigh([ptr] { ptr->Cancel(); });

    return *this;
  }

  /**
   * Binds a runnable to execute repeatedly while the trigger is active.
   *
   * @param toRun the runnable to execute.
   * @param requirements the required subsystems.
   * @deprecated Use WhileTrue(Command) and construct a RunCommand manually
   */
  WPI_DEPRECATED("Use WhileTrue(Command) and construct a RunCommand manually")
  Trigger WhileActiveContinous(std::function<void()> toRun,
                               std::initializer_list<Subsystem*> requirements);

  /**
   * Binds a runnable to execute repeatedly while the trigger is active.
   *
   * @param toRun the runnable to execute.
   * @param requirements the required subsystems.
   * @deprecated Use WhileTrue(Command) and construct a RunCommand manually
   */
  WPI_DEPRECATED("Use WhileTrue(Command) and construct a RunCommand manually")
  Trigger WhileActiveContinous(std::function<void()> toRun,
                               std::span<Subsystem* const> requirements = {});

  /**
   * Binds a command to be started when the trigger becomes active, and
   * canceled when it becomes inactive.  Takes a raw pointer, and so is
   * non-owning; users are responsible for the lifespan of the command.
   *
   * @param command The command to bind.
   * @return The trigger, for chained calls.
   * @deprecated Use WhileTrue(Command) instead.
   */
  WPI_DEPRECATED("Use WhileTrue(Command) instead.")
  Trigger WhileActiveOnce(Command* command);

  /**
   * Binds a command to be started when the trigger becomes active, and
   * canceled when it becomes inactive. Transfers command ownership to the
   * button scheduler, so the user does not have to worry about lifespan -
   * rvalue refs will be *moved*, lvalue refs will be *copied.*
   *
   * @param command The command to bind.
   * @return The trigger, for chained calls.
   * @deprecated Use WhileTrue(Command) instead.
   */
  template <class T, typename = std::enable_if_t<std::is_base_of_v<
                         Command, std::remove_reference_t<T>>>>
  WPI_DEPRECATED("Use WhileTrue(Command) instead.")
  Trigger WhileActiveOnce(T&& command) {
    std::shared_ptr<T> ptr =
        std::make_shared<std::remove_reference_t<T>>(std::forward<T>(command));

    m_event.Rising().IfHigh([ptr] { ptr->Schedule(); });
    m_event.Falling().IfHigh([ptr] { ptr->Cancel(); });

    return *this;
  }

  /**
   * Binds a command to start when the trigger becomes inactive.  Takes a
   * raw pointer, and so is non-owning; users are responsible for the lifespan
   * of the command.
   *
   * @param command The command to bind.
   * @return The trigger, for chained calls.
   * @deprecated Use OnFalse(Command) instead.
   */
  WPI_DEPRECATED("Use OnFalse(Command) instead.")
  Trigger WhenInactive(Command* command);

  /**
   * Binds a command to start when the trigger becomes inactive.  Transfers
   * command ownership to the button scheduler, so the user does not have to
   * worry about lifespan - rvalue refs will be *moved*, lvalue refs will be
   * *copied.*
   *
   * @param command The command to bind.
   * @return The trigger, for chained calls.
   * @deprecated Use OnFalse(Command) instead.
   */
  template <class T, typename = std::enable_if_t<std::is_base_of_v<
                         Command, std::remove_reference_t<T>>>>
  WPI_DEPRECATED("Use OnFalse(Command) instead.")
  Trigger WhenInactive(T&& command) {
    m_event.Falling().IfHigh(
        [command = std::make_unique<std::remove_reference_t<T>>(
             std::forward<T>(command))] { command->Schedule(); });

    return *this;
  }

  /**
   * Binds a runnable to execute when the trigger becomes inactive.
   *
   * @param toRun the runnable to execute.
   * @param requirements the required subsystems.
   * @deprecated Use OnFalse(Command) instead and construct the InstantCommand
   * manually
   */
  WPI_DEPRECATED(
      "Use OnFalse(Command) instead and construct the InstantCommand manually")
  Trigger WhenInactive(std::function<void()> toRun,
                       std::initializer_list<Subsystem*> requirements);

  /**
   * Binds a runnable to execute when the trigger becomes inactive.
   *
   * @param toRun the runnable to execute.
   * @param requirements the required subsystems.
   * @deprecated Use OnFalse(Command) instead and construct the InstantCommand
   * manually
   */
  WPI_DEPRECATED(
      "Use OnFalse(Command) instead and construct the InstantCommand manually")
  Trigger WhenInactive(std::function<void()> toRun,
                       std::span<Subsystem* const> requirements = {});

  /**
   * Binds a command to start when the trigger becomes active, and be canceled
   * when it again becomes active. Takes a raw pointer, and so is non-owning;
   * users are responsible for the lifespan of the command.
   *
   * @param command The command to bind.
   * @return The trigger, for chained calls.
   * @deprecated Use ToggleOnTrue(Command) instead.
   */
  WPI_DEPRECATED("Use ToggleOnTrue(Command) instead.")
  Trigger ToggleWhenActive(Command* command);

  /**
   * Binds a command to start when the trigger becomes active, and be canceled
   * when it again becomes active.  Transfers command ownership to the button
   * scheduler, so the user does not have to worry about lifespan - rvalue refs
   * will be *moved*, lvalue refs will be *copied.*
   *
   * @param command The command to bind.
   * @return The trigger, for chained calls.
   * @deprecated Use ToggleOnTrue(Command) instead.
   */
  template <class T, typename = std::enable_if_t<std::is_base_of_v<
                         Command, std::remove_reference_t<T>>>>
  WPI_DEPRECATED("Use ToggleOnTrue(Command) instead.")
  Trigger ToggleWhenActive(T&& command) {
    m_event.Rising().IfHigh(
        [command = std::make_unique<std::remove_reference_t<T>>(
             std::forward<T>(command))] {
          if (!command->IsScheduled()) {
            command->Schedule();
          } else {
            command->Cancel();
          }
        });

    return *this;
  }

  /**
   * Binds a command to be canceled when the trigger becomes active.  Takes a
   * raw pointer, and so is non-owning; users are responsible for the lifespan
   *  and scheduling of the command.
   *
   * @param command The command to bind.
   * @return The trigger, for chained calls.
   * @deprecated Use Rising() as a command end condition with Until() instead.
   */
  WPI_DEPRECATED(
      "Use Rising() as a command end condition with Until() instead.")
  Trigger CancelWhenActive(Command* command);

  /**
   * Get a new event that events only when this one newly changes to true.
   *
   * @return a new event representing when this one newly changes to true.
   */
  Trigger Rising() { return m_event.Rising().CastTo<Trigger>(); }

  /**
   * Get a new event that triggers only when this one newly changes to false.
   *
   * @return a new event representing when this one newly changes to false.
   */
  Trigger Falling() { return m_event.Falling().CastTo<Trigger>(); }

  /**
   * Composes two triggers with logical AND.
   *
   * @return A trigger which is active when both component triggers are active.
   */
  Trigger operator&&(std::function<bool()> rhs) {
    return m_event.operator&&(rhs).CastTo<Trigger>();
  }

  /**
   * Composes two triggers with logical OR.
   *
   * @return A trigger which is active when either component trigger is active.
   */
  Trigger operator||(std::function<bool()> rhs) {
    return m_event.operator||(rhs).CastTo<Trigger>();
  }

  /**
   * Composes a trigger with logical NOT.
   *
   * @return A trigger which is active when the component trigger is inactive,
   * and vice-versa.
   */
  Trigger operator!() { return m_event.operator!().CastTo<Trigger>(); }

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
                       frc::Debouncer::DebounceType::kRising) {
    return m_event.Debounce(debounceTime, type).CastTo<Trigger>();
  }

  /**
   * Get the wrapped BooleanEvent instance.
   */
  frc::BooleanEvent GetEvent() const;

 private:
  frc::BooleanEvent m_event;
};
}  // namespace frc2
