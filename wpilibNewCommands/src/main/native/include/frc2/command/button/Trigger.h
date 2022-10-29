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
 * This class is a command-based wrapper around {@link frc::BooleanEvent},
 * providing an easy way to link commands to inputs.
 *
 * This class is provided by the NewCommands VendorDep
 *
 * @see Button
 */
class Trigger {
 public:
  /**
   * Creates a new trigger with the given condition determining whether it is
   * active.
   *
   * <p>Polled by the default scheduler button loop.
   *
   * @param isActive returns whether or not the trigger should be active
   */
  explicit Trigger(std::function<bool()> isActive)
      : m_event{CommandScheduler::GetInstance().GetDefaultButtonLoop(),
                std::move(isActive)} {}

  /**
   * Create a new trigger that is active when the given condition is true.
   *
   * @param loop The loop instance that polls this trigger.
   * @param isActive Whether the trigger is active.
   */
  Trigger(frc::EventLoop* loop, std::function<bool()> isActive)
      : m_event{loop, std::move(isActive)} {}

  /**
   * Create a new trigger that is never active (default constructor) - activity
   *  can be further determined by subclass code.
   */
  Trigger() : Trigger([] { return false; }) {}

  Trigger(const Trigger& other);

  /**
   * Starts the given command whenever the signal rises from `false` to `true`.
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
   * Starts the given command whenever the signal rises from `false` to `true`.
   * Moves command ownership to the button scheduler.
   *
   * @param command The command to bind.
   * @return The trigger, for chained calls.
   */
  Trigger OnTrue(CommandPtr&& command);

  /**
   * Starts the given command whenever the signal falls from `true` to `false`.
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
   * Starts the given command whenever the signal falls from `true` to `false`.
   *
   * @param command The command to bind.
   * @return The trigger, for chained calls.
   */
  Trigger OnFalse(CommandPtr&& command);

  /**
   * Starts the given command when the signal rises to `true` and cancels it
   * when the signal falls to `false`.
   *
   * <p>Doesn't re-start the command in-between.
   *
   * <p>Takes a raw pointer, and so is non-owning; users are responsible for the
   * lifespan of the command.
   *
   * @param command the command to start
   * @return this trigger, so calls can be chained
   */
  Trigger WhileTrue(Command* command);

  /**
   * Starts the given command when the signal rises to `true` and cancels it
   * when the signal falls to `false`. Moves command ownership to the button
   * scheduler.
   *
   * @param command The command to bind.
   * @return The trigger, for chained calls.
   */
  Trigger WhileTrue(CommandPtr&& command);

  /**
   * Starts the given command when the signal falls to `false` and cancels
   * it when the signal rises.
   *
   * <p>Doesn't re-start the command in-between.
   *
   * <p>Takes a raw pointer, and so is non-owning; users are responsible for the
   * lifespan of the command.
   *
   * @param command the command to start
   * @return this trigger, so calls can be chained
   */
  Trigger WhileFalse(Command* command);

  /**
   * Starts the given command when the signal falls to `false` and cancels
   * it when the signal rises. Moves command ownership to the button
   * scheduler.
   *
   * @param command The command to bind.
   * @return The trigger, for chained calls.
   */
  Trigger WhileFalse(CommandPtr&& command);

  /**
   * Toggles a command when the signal rises from `false` to the high
   * state.
   *
   * <p>Takes a raw pointer, and so is non-owning; users are responsible for the
   * lifespan of the command.
   *
   * @param command the command to toggle
   * @return this trigger, so calls can be chained
   */
  Trigger ToggleOnTrue(Command* command);

  /**
   * Toggles a command when the signal rises from `false` to the high
   * state.
   *
   * <p>Takes a raw pointer, and so is non-owning; users are responsible for the
   * lifespan of the command.
   *
   * @param command the command to toggle
   * @return this trigger, so calls can be chained
   */
  Trigger ToggleOnTrue(CommandPtr&& command);

  /**
   * Toggles a command when the signal falls from `true` to the low
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
   * Toggles a command when the signal falls from `true` to the low
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
