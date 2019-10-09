/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <frc2/command/Command.h>
#include <frc2/command/CommandScheduler.h>

#include <atomic>
#include <memory>
#include <utility>

namespace frc2 {
class Command;
/**
 * A class used to bind command scheduling to events.  The
 * Trigger class is a base for all command-event-binding classes, and so the
 * methods are named fairly abstractly; for purpose-specific wrappers, see
 * Button.
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
  explicit Trigger(std::function<bool()> isActive)
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
   * Returns whether the trigger is active.  Can be overridden by a subclass.
   *
   * @return Whether the trigger is active.
   */
  virtual bool Get() const { return m_isActive(); }

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
        [pressedLast = Get(), *this,
         command = std::make_unique<std::remove_reference_t<T>>(
             std::forward<T>(command)),
         interruptible]() mutable {
          bool pressed = Get();

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
   */
  Trigger WhenActive(std::function<void()> toRun);

  /**
   * Binds a command to be started repeatedly while the trigger is active, and
   * cancelled when it becomes inactive.  Takes a raw pointer, and so is
   * non-owning; users are responsible for the lifespan of the command.
   *
   * @param command The command to bind.
   * @param interruptible Whether the command should be interruptible.
   * @return The trigger, for chained calls.
   */
  Trigger WhileActiveContinous(Command* command, bool interruptible = true);

  /**
   * Binds a command to be started repeatedly while the trigger is active, and
   * cancelled when it becomes inactive.  Transfers command ownership to the
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
        [pressedLast = Get(), *this,
         command = std::make_unique<std::remove_reference_t<T>>(
             std::forward<T>(command)),
         interruptible]() mutable {
          bool pressed = Get();

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
   */
  Trigger WhileActiveContinous(std::function<void()> toRun);

  /**
   * Binds a command to be started when the trigger becomes active, and
   * cancelled when it becomes inactive.  Takes a raw pointer, and so is
   * non-owning; users are responsible for the lifespan of the command.
   *
   * @param command The command to bind.
   * @param interruptible Whether the command should be interruptible.
   * @return The trigger, for chained calls.
   */
  Trigger WhileActiveOnce(Command* command, bool interruptible = true);

  /**
   * Binds a command to be started when the trigger becomes active, and
   * cancelled when it becomes inactive.  Transfers command ownership to the
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
        [pressedLast = Get(), *this,
         command = std::make_unique<std::remove_reference_t<T>>(
             std::forward<T>(command)),
         interruptible]() mutable {
          bool pressed = Get();

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
        [pressedLast = Get(), *this,
         command = std::make_unique<std::remove_reference_t<T>>(
             std::forward<T>(command)),
         interruptible]() mutable {
          bool pressed = Get();

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
   */
  Trigger WhenInactive(std::function<void()> toRun);

  /**
   * Binds a command to start when the trigger becomes active, and be cancelled
   * when it again becomes active.  Takes a raw pointer, and so is non-owning;
   * users are responsible for the lifespan of the command.
   *
   * @param command The command to bind.
   * @param interruptible Whether the command should be interruptible.
   * @return The trigger, for chained calls.
   */
  Trigger ToggleWhenActive(Command* command, bool interruptible = true);

  /**
   * Binds a command to start when the trigger becomes active, and be cancelled
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
        [pressedLast = Get(), *this,
         command = std::make_unique<std::remove_reference_t<T>>(
             std::forward<T>(command)),
         interruptible]() mutable {
          bool pressed = Get();

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
   * Binds a command to be cancelled when the trigger becomes active.  Takes a
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
    return Trigger([*this, rhs] { return Get() && rhs.Get(); });
  }

  /**
   * Composes two triggers with logical OR.
   *
   * @return A trigger which is active when either component trigger is active.
   */
  Trigger operator||(Trigger rhs) {
    return Trigger([*this, rhs] { return Get() || rhs.Get(); });
  }

  /**
   * Composes a trigger with logical NOT.
   *
   * @return A trigger which is active when the component trigger is inactive,
   * and vice-versa.
   */
  Trigger operator!() {
    return Trigger([*this] { return !Get(); });
  }

 private:
  std::function<bool()> m_isActive;
};
}  // namespace frc2
