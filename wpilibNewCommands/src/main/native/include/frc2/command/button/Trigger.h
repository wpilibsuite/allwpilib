// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <functional>
#include <initializer_list>
#include <memory>
#include <utility>

#include <frc/filter/Debouncer.h>
#include <units/time.h>
#include <wpi/span.h>

#include "frc2/command/Command.h"
#include "frc2/command/CommandScheduler.h"

namespace frc2 {
class Command;
/**
 * A class used to bind command scheduling to events.  The
 * Trigger class is a base for all command-event-binding classes, and so the
 * methods are named fairly abstractly; for purpose-specific wrappers, see
 * Button.
 *
 * This class is provided by the NewCommands VendorDep
 *
 * @see Button
 */
class Trigger {
 public:
  /**
   * Create a new trigger that is active when the given condition is true.
   *
   * @param isActive Whether the trigger is active.
   */
  Trigger(std::function<bool()> isActive)  // NOLINT
      : m_isActive{std::move(isActive)} {}

  /**
   * Create a new trigger that is never active (default constructor) - activity
   *  can be further determined by subclass code.
   */
  Trigger() {
    m_isActive = [] { return false; };
  }

  Trigger(const Trigger& other);

  /**
   * Binds a command to start when the trigger becomes active.  Takes a
   * raw pointer, and so is non-owning; users are responsible for the lifespan
   * of the command.
   *
   * @param command The command to bind.
   * @param interruptible Whether the command should be interruptible.
   * @return The trigger, for chained calls.
   */
  Trigger WhenActive(Command* command, bool interruptible = true);

  /**
   * Binds a command to start when the trigger becomes active.  Transfers
   * command ownership to the button scheduler, so the user does not have to
   * worry about lifespan - rvalue refs will be *moved*, lvalue refs will be
   * *copied.*
   *
   * @param command The command to bind.
   * @param interruptible Whether the command should be interruptible.
   * @return The trigger, for chained calls.
   */
  template <class T, typename = std::enable_if_t<std::is_base_of_v<
                         Command, std::remove_reference_t<T>>>>
  Trigger WhenActive(T&& command, bool interruptible = true) {
    CommandScheduler::GetInstance().AddButton(
        [pressedLast = m_isActive(), *this,
         command = std::make_unique<std::remove_reference_t<T>>(
             std::forward<T>(command)),
         interruptible]() mutable {
          bool pressed = m_isActive();

          if (!pressedLast && pressed) {
            command->Schedule(interruptible);
          }

          pressedLast = pressed;
        });

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
   * @param interruptible Whether the command should be interruptible.
   * @return The trigger, for chained calls.
   */
  Trigger WhileActiveContinous(Command* command, bool interruptible = true);

  /**
   * Binds a command to be started repeatedly while the trigger is active, and
   * canceled when it becomes inactive.  Transfers command ownership to the
   * button scheduler, so the user does not have to worry about lifespan -
   * rvalue refs will be *moved*, lvalue refs will be *copied.*
   *
   * @param command The command to bind.
   * @param interruptible Whether the command should be interruptible.
   * @return The trigger, for chained calls.
   */
  template <class T, typename = std::enable_if_t<std::is_base_of_v<
                         Command, std::remove_reference_t<T>>>>
  Trigger WhileActiveContinous(T&& command, bool interruptible = true) {
    CommandScheduler::GetInstance().AddButton(
        [pressedLast = m_isActive(), *this,
         command = std::make_unique<std::remove_reference_t<T>>(
             std::forward<T>(command)),
         interruptible]() mutable {
          bool pressed = m_isActive();

          if (pressed) {
            command->Schedule(interruptible);
          } else if (pressedLast && !pressed) {
            command->Cancel();
          }

          pressedLast = pressed;
        });
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
   * @param interruptible Whether the command should be interruptible.
   * @return The trigger, for chained calls.
   */
  Trigger WhileActiveOnce(Command* command, bool interruptible = true);

  /**
   * Binds a command to be started when the trigger becomes active, and
   * canceled when it becomes inactive.  Transfers command ownership to the
   * button scheduler, so the user does not have to worry about lifespan -
   * rvalue refs will be *moved*, lvalue refs will be *copied.*
   *
   * @param command The command to bind.
   * @param interruptible Whether the command should be interruptible.
   * @return The trigger, for chained calls.
   */
  template <class T, typename = std::enable_if_t<std::is_base_of_v<
                         Command, std::remove_reference_t<T>>>>
  Trigger WhileActiveOnce(T&& command, bool interruptible = true) {
    CommandScheduler::GetInstance().AddButton(
        [pressedLast = m_isActive(), *this,
         command = std::make_unique<std::remove_reference_t<T>>(
             std::forward<T>(command)),
         interruptible]() mutable {
          bool pressed = m_isActive();

          if (!pressedLast && pressed) {
            command->Schedule(interruptible);
          } else if (pressedLast && !pressed) {
            command->Cancel();
          }

          pressedLast = pressed;
        });
    return *this;
  }

  /**
   * Binds a command to start when the trigger becomes inactive.  Takes a
   * raw pointer, and so is non-owning; users are responsible for the lifespan
   * of the command.
   *
   * @param command The command to bind.
   * @param interruptible Whether the command should be interruptible.
   * @return The trigger, for chained calls.
   */
  Trigger WhenInactive(Command* command, bool interruptible = true);

  /**
   * Binds a command to start when the trigger becomes inactive.  Transfers
   * command ownership to the button scheduler, so the user does not have to
   * worry about lifespan - rvalue refs will be *moved*, lvalue refs will be
   * *copied.*
   *
   * @param command The command to bind.
   * @param interruptible Whether the command should be interruptible.
   * @return The trigger, for chained calls.
   */
  template <class T, typename = std::enable_if_t<std::is_base_of_v<
                         Command, std::remove_reference_t<T>>>>
  Trigger WhenInactive(T&& command, bool interruptible = true) {
    CommandScheduler::GetInstance().AddButton(
        [pressedLast = m_isActive(), *this,
         command = std::make_unique<std::remove_reference_t<T>>(
             std::forward<T>(command)),
         interruptible]() mutable {
          bool pressed = m_isActive();

          if (pressedLast && !pressed) {
            command->Schedule(interruptible);
          }

          pressedLast = pressed;
        });
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
   * when it again becomes active.  Takes a raw pointer, and so is non-owning;
   * users are responsible for the lifespan of the command.
   *
   * @param command The command to bind.
   * @param interruptible Whether the command should be interruptible.
   * @return The trigger, for chained calls.
   */
  Trigger ToggleWhenActive(Command* command, bool interruptible = true);

  /**
   * Binds a command to start when the trigger becomes active, and be canceled
   * when it again becomes active.  Transfers command ownership to the button
   * scheduler, so the user does not have to worry about lifespan - rvalue refs
   * will be *moved*, lvalue refs will be *copied.*
   *
   * @param command The command to bind.
   * @param interruptible Whether the command should be interruptible.
   * @return The trigger, for chained calls.
   */
  template <class T, typename = std::enable_if_t<std::is_base_of_v<
                         Command, std::remove_reference_t<T>>>>
  Trigger ToggleWhenActive(T&& command, bool interruptible = true) {
    CommandScheduler::GetInstance().AddButton(
        [pressedLast = m_isActive(), *this,
         command = std::make_unique<std::remove_reference_t<T>>(
             std::forward<T>(command)),
         interruptible]() mutable {
          bool pressed = m_isActive();

          if (!pressedLast && pressed) {
            if (command->IsScheduled()) {
              command->Cancel();
            } else {
              command->Schedule(interruptible);
            }
          }

          pressedLast = pressed;
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
   * Composes two triggers with logical AND.
   *
   * @return A trigger which is active when both component triggers are active.
   */
  Trigger operator&&(Trigger rhs) {
    return Trigger([*this, rhs] { return m_isActive() && rhs.m_isActive(); });
  }

  /**
   * Composes two triggers with logical OR.
   *
   * @return A trigger which is active when either component trigger is active.
   */
  Trigger operator||(Trigger rhs) {
    return Trigger([*this, rhs] { return m_isActive() || rhs.m_isActive(); });
  }

  /**
   * Composes a trigger with logical NOT.
   *
   * @return A trigger which is active when the component trigger is inactive,
   * and vice-versa.
   */
  Trigger operator!() {
    return Trigger([*this] { return !m_isActive(); });
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

 private:
  std::function<bool()> m_isActive;
};
}  // namespace frc2
