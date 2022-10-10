// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <functional>
#include <initializer_list>
#include <memory>
#include <utility>

#include <frc/event/BooleanEvent.h>
#include <frc/event/EventLoop.h>
#include <frc/filter/Debouncer.h>
#include <units/time.h>
#include <wpi/span.h>

#include "frc2/command/Command.h"
#include "frc2/command/CommandScheduler.h"

namespace frc2 {
class Command;
/**
 * This class is a command-based wrapper around {@link BooleanEvent}, providing
 * an easy way to link commands to inputs.
 *
 * This class is provided by the NewCommands VendorDep
 *
 * @see Button
 */
class Trigger : public frc::BooleanEvent {
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
      : BooleanEvent{CommandScheduler::GetInstance().GetDefaultButtonLoop(),
                     std::move(isActive)} {}

  /**
   * Create a new trigger that is active when the given condition is true.
   *
   * @param loop The loop instance that polls this trigger.
   * @param isActive Whether the trigger is active.
   */
  Trigger(frc::EventLoop* loop, std::function<bool()> isActive)
      : BooleanEvent{loop, std::move(isActive)} {}

  /**
   * Create a new trigger that is never active (default constructor) - activity
   *  can be further determined by subclass code.
   */
  Trigger() : Trigger([] { return false; }) {}

  Trigger(const Trigger& other);

  /**
   * Binds a command to start when the trigger becomes active. Takes a
   * raw pointer, and so is non-owning; users are responsible for the lifespan
   * of the command.
   *
   * @param command The command to bind.
   * @return The trigger, for chained calls.
   */
  Trigger WhenActive(Command* command);

  /**
   * Binds a command to start when the trigger becomes active. Moves
   * command ownership to the button scheduler.
   *
   * @param command The command to bind.
   * @return The trigger, for chained calls.
   */
  Trigger WhenActive(CommandPtr&& command);

  /**
   * Binds a command to start when the trigger becomes active. Transfers
   * command ownership to the button scheduler, so the user does not have to
   * worry about lifespan - rvalue refs will be *moved*, lvalue refs will be
   * *copied.*
   *
   * @param command The command to bind.
   * @return The trigger, for chained calls.
   */
  template <class T, typename = std::enable_if_t<std::is_base_of_v<
                         Command, std::remove_reference_t<T>>>>
  Trigger WhenActive(T&& command) {
    this->Rising().IfHigh(
        [command = std::make_unique<std::remove_reference_t<T>>(
             std::forward<T>(command))] { command->Schedule(); });

    return *this;
  }

  /**
   * Binds a runnable to execute when the trigger becomes active.
   *
   * @param toRun the runnable to execute.
   * @param requirements the required subsystems.
   */
  Trigger WhenActive(std::function<void()> toRun,
                     std::initializer_list<Subsystem*> requirements);

  /**
   * Binds a runnable to execute when the trigger becomes active.
   *
   * @param toRun the runnable to execute.
   * @param requirements the required subsystems.
   */
  Trigger WhenActive(std::function<void()> toRun,
                     wpi::span<Subsystem* const> requirements = {});

  /**
   * Binds a command to be started repeatedly while the trigger is active, and
   * canceled when it becomes inactive.  Takes a raw pointer, and so is
   * non-owning; users are responsible for the lifespan of the command.
   *
   * @param command The command to bind.
   * @return The trigger, for chained calls.
   */
  Trigger WhileActiveContinous(Command* command);

  /**
   * Binds a command to be started repeatedly while the trigger is active, and
   * canceled when it becomes inactive. Moves command ownership to the button
   * scheduler.
   *
   * @param command The command to bind.
   * @return The trigger, for chained calls.
   */
  Trigger WhileActiveContinous(CommandPtr&& command);

  /**
   * Binds a command to be started repeatedly while the trigger is active, and
   * canceled when it becomes inactive.  Transfers command ownership to the
   * button scheduler, so the user does not have to worry about lifespan -
   * rvalue refs will be *moved*, lvalue refs will be *copied.*
   *
   * @param command The command to bind.
   * @return The trigger, for chained calls.
   */
  template <class T, typename = std::enable_if_t<std::is_base_of_v<
                         Command, std::remove_reference_t<T>>>>
  Trigger WhileActiveContinous(T&& command) {
    std::shared_ptr<T> ptr =
        std::make_shared<std::remove_reference_t<T>>(std::forward<T>(command));
    this->IfHigh([ptr] { ptr->Schedule(); });
    this->Falling().IfHigh([ptr] { ptr->Cancel(); });

    return *this;
  }

  /**
   * Binds a runnable to execute repeatedly while the trigger is active.
   *
   * @param toRun the runnable to execute.
   * @param requirements the required subsystems.
   */
  Trigger WhileActiveContinous(std::function<void()> toRun,
                               std::initializer_list<Subsystem*> requirements);

  /**
   * Binds a runnable to execute repeatedly while the trigger is active.
   *
   * @param toRun the runnable to execute.
   * @param requirements the required subsystems.
   */
  Trigger WhileActiveContinous(std::function<void()> toRun,
                               wpi::span<Subsystem* const> requirements = {});

  /**
   * Binds a command to be started when the trigger becomes active, and
   * canceled when it becomes inactive.  Takes a raw pointer, and so is
   * non-owning; users are responsible for the lifespan of the command.
   *
   * @param command The command to bind.
   * @return The trigger, for chained calls.
   */
  Trigger WhileActiveOnce(Command* command);

  /**
   * Binds a command to be started when the trigger becomes active, and
   * canceled when it becomes inactive. Moves command ownership to the button
   * scheduler.
   *
   * @param command The command to bind.
   * @return The trigger, for chained calls.
   */
  Trigger WhileActiveOnce(CommandPtr&& command);

  /**
   * Binds a command to be started when the trigger becomes active, and
   * canceled when it becomes inactive. Transfers command ownership to the
   * button scheduler, so the user does not have to worry about lifespan -
   * rvalue refs will be *moved*, lvalue refs will be *copied.*
   *
   * @param command The command to bind.
   * @return The trigger, for chained calls.
   */
  template <class T, typename = std::enable_if_t<std::is_base_of_v<
                         Command, std::remove_reference_t<T>>>>
  Trigger WhileActiveOnce(T&& command) {
    std::shared_ptr<T> ptr =
        std::make_shared<std::remove_reference_t<T>>(std::forward<T>(command));

    this->Rising().IfHigh([ptr] { ptr->Schedule(); });
    this->Falling().IfHigh([ptr] { ptr->Cancel(); });

    return *this;
  }

  /**
   * Binds a command to start when the trigger becomes inactive.  Takes a
   * raw pointer, and so is non-owning; users are responsible for the lifespan
   * of the command.
   *
   * @param command The command to bind.
   * @return The trigger, for chained calls.
   */
  Trigger WhenInactive(Command* command);

  /**
   * Binds a command to start when the trigger becomes inactive. Moves
   * command ownership to the button scheduler.
   *
   * @param command The command to bind.
   * @return The trigger, for chained calls.
   */
  Trigger WhenInactive(CommandPtr&& command);

  /**
   * Binds a command to start when the trigger becomes inactive.  Transfers
   * command ownership to the button scheduler, so the user does not have to
   * worry about lifespan - rvalue refs will be *moved*, lvalue refs will be
   * *copied.*
   *
   * @param command The command to bind.
   * @return The trigger, for chained calls.
   */
  template <class T, typename = std::enable_if_t<std::is_base_of_v<
                         Command, std::remove_reference_t<T>>>>
  Trigger WhenInactive(T&& command) {
    this->Falling().IfHigh(
        [command = std::make_unique<std::remove_reference_t<T>>(
             std::forward<T>(command))] { command->Schedule(); });

    return *this;
  }

  /**
   * Binds a runnable to execute when the trigger becomes inactive.
   *
   * @param toRun the runnable to execute.
   * @param requirements the required subsystems.
   */
  Trigger WhenInactive(std::function<void()> toRun,
                       std::initializer_list<Subsystem*> requirements);

  /**
   * Binds a runnable to execute when the trigger becomes inactive.
   *
   * @param toRun the runnable to execute.
   * @param requirements the required subsystems.
   */
  Trigger WhenInactive(std::function<void()> toRun,
                       wpi::span<Subsystem* const> requirements = {});

  /**
   * Binds a command to start when the trigger becomes active, and be canceled
   * when it again becomes active. Takes a raw pointer, and so is non-owning;
   * users are responsible for the lifespan of the command.
   *
   * @param command The command to bind.
   * @return The trigger, for chained calls.
   */
  Trigger ToggleWhenActive(Command* command);

  /**
   * Binds a command to start when the trigger becomes active, and be canceled
   * when it again becomes active. Moves command ownership to the button
   * scheduler.
   *
   * @param command The command to bind.
   * @return The trigger, for chained calls.
   */
  Trigger ToggleWhenActive(CommandPtr&& command);

  /**
   * Binds a command to start when the trigger becomes active, and be canceled
   * when it again becomes active.  Transfers command ownership to the button
   * scheduler, so the user does not have to worry about lifespan - rvalue refs
   * will be *moved*, lvalue refs will be *copied.*
   *
   * @param command The command to bind.
   * @return The trigger, for chained calls.
   */
  template <class T, typename = std::enable_if_t<std::is_base_of_v<
                         Command, std::remove_reference_t<T>>>>
  Trigger ToggleWhenActive(T&& command) {
    this->Rising().IfHigh(
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
   */
  Trigger CancelWhenActive(Command* command);

  /**
   * Get a new event that events only when this one newly changes to true.
   *
   * @return a new event representing when this one newly changes to true.
   */
  Trigger Rising() { return BooleanEvent::Rising().CastTo<Trigger>(); }

  /**
   * Get a new event that triggers only when this one newly changes to false.
   *
   * @return a new event representing when this one newly changes to false.
   */
  Trigger Falling() { return BooleanEvent::Falling().CastTo<Trigger>(); }

  /**
   * Composes two triggers with logical AND.
   *
   * @return A trigger which is active when both component triggers are active.
   */
  Trigger operator&&(std::function<bool()> rhs) {
    return BooleanEvent::operator&&(rhs).CastTo<Trigger>();
  }

  /**
   * Composes two triggers with logical OR.
   *
   * @return A trigger which is active when either component trigger is active.
   */
  Trigger operator||(std::function<bool()> rhs) {
    return BooleanEvent::operator||(rhs).CastTo<Trigger>();
  }

  /**
   * Composes a trigger with logical NOT.
   *
   * @return A trigger which is active when the component trigger is inactive,
   * and vice-versa.
   */
  Trigger operator!() { return BooleanEvent::operator!().CastTo<Trigger>(); }

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
    return BooleanEvent::Debounce(debounceTime, type).CastTo<Trigger>();
  }
};
}  // namespace frc2
