// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <functional>
#include <initializer_list>
#include <utility>

#include <wpi/span.h>

#include "Trigger.h"

namespace frc {
class Command;
/**
 * A class used to bind command scheduling to button presses.  Can be composed
 * with other buttons with the operators in Trigger.
 *
 * This class is provided by the NewCommands VendorDep
 *
 * @see Trigger
 */
class Button : public Trigger {
 public:
  /**
   * Create a new button that is pressed when the given condition is true.
   *
   * @param isPressed Whether the button is pressed.
   */
  explicit Button(std::function<bool()> isPressed);

  /**
   * Create a new button that is pressed active (default constructor) - activity
   *  can be further determined by subclass code.
   */
  Button() = default;

  /**
   * Binds a command to start when the button is pressed.  Takes a
   * raw pointer, and so is non-owning; users are responsible for the lifespan
   * of the command.
   *
   * @param command The command to bind.
   * @return The trigger, for chained calls.
   */
  Button WhenPressed(Command* command);

  /**
   * Binds a command to start when the button is pressed.  Transfers
   * command ownership to the button scheduler, so the user does not have to
   * worry about lifespan - rvalue refs will be *moved*, lvalue refs will be
   * *copied.*
   *
   * @param command The command to bind.
   * @return The trigger, for chained calls.
   */
  template <class T, typename = std::enable_if_t<std::is_base_of_v<
                         Command, std::remove_reference_t<T>>>>
  Button WhenPressed(T&& command) {
    WhenActive(std::forward<T>(command));
    return *this;
  }

  /**
   * Binds a runnable to execute when the button is pressed.
   *
   * @param toRun the runnable to execute.
   * @param requirements the required subsystems.
   */
  Button WhenPressed(std::function<void()> toRun,
                     std::initializer_list<Subsystem*> requirements);

  /**
   * Binds a runnable to execute when the button is pressed.
   *
   * @param toRun the runnable to execute.
   * @param requirements the required subsystems.
   */
  Button WhenPressed(std::function<void()> toRun,
                     wpi::span<Subsystem* const> requirements = {});

  /**
   * Binds a command to be started repeatedly while the button is pressed, and
   * canceled when it is released.  Takes a raw pointer, and so is non-owning;
   * users are responsible for the lifespan of the command.
   *
   * @param command The command to bind.
   * @return The button, for chained calls.
   */
  Button WhileHeld(Command* command);

  /**
   * Binds a command to be started repeatedly while the button is pressed, and
   * canceled when it is released.  Transfers command ownership to the button
   * scheduler, so the user does not have to worry about lifespan - rvalue refs
   * will be *moved*, lvalue refs will be *copied.*
   *
   * @param command The command to bind.
   * @return The button, for chained calls.
   */
  template <class T, typename = std::enable_if_t<std::is_base_of_v<
                         Command, std::remove_reference_t<T>>>>
  Button WhileHeld(T&& command) {
    WhileActiveContinous(std::forward<T>(command));
    return *this;
  }

  /**
   * Binds a runnable to execute repeatedly while the button is pressed.
   *
   * @param toRun the runnable to execute.
   * @param requirements the required subsystems.
   */
  Button WhileHeld(std::function<void()> toRun,
                   std::initializer_list<Subsystem*> requirements);

  /**
   * Binds a runnable to execute repeatedly while the button is pressed.
   *
   * @param toRun the runnable to execute.
   * @param requirements the required subsystems.
   */
  Button WhileHeld(std::function<void()> toRun,
                   wpi::span<Subsystem* const> requirements = {});

  /**
   * Binds a command to be started when the button is pressed, and canceled
   * when it is released.  Takes a raw pointer, and so is non-owning; users are
   * responsible for the lifespan of the command.
   *
   * @param command The command to bind.
   * @return The button, for chained calls.
   */
  Button WhenHeld(Command* command);

  /**
   * Binds a command to be started when the button is pressed, and canceled
   * when it is released.  Transfers command ownership to the button scheduler,
   * so the user does not have to worry about lifespan - rvalue refs will be
   * *moved*, lvalue refs will be *copied.*
   *
   * @param command The command to bind.
   * @return The button, for chained calls.
   */
  template <class T, typename = std::enable_if_t<std::is_base_of_v<
                         Command, std::remove_reference_t<T>>>>
  Button WhenHeld(T&& command) {
    WhileActiveOnce(std::forward<T>(command));
    return *this;
  }

  /**
   * Binds a command to start when the button is released.  Takes a
   * raw pointer, and so is non-owning; users are responsible for the lifespan
   * of the command.
   *
   * @param command The command to bind.
   * @return The button, for chained calls.
   */
  Button WhenReleased(Command* command);

  /**
   * Binds a command to start when the button is pressed.  Transfers
   * command ownership to the button scheduler, so the user does not have to
   * worry about lifespan - rvalue refs will be *moved*, lvalue refs will be
   * *copied.*
   *
   * @param command The command to bind.
   * @return The button, for chained calls.
   */
  template <class T, typename = std::enable_if_t<std::is_base_of_v<
                         Command, std::remove_reference_t<T>>>>
  Button WhenReleased(T&& command) {
    WhenInactive(std::forward<T>(command));
    return *this;
  }

  /**
   * Binds a runnable to execute when the button is released.
   *
   * @param toRun the runnable to execute.
   * @param requirements the required subsystems.
   */
  Button WhenReleased(std::function<void()> toRun,
                      std::initializer_list<Subsystem*> requirements);

  /**
   * Binds a runnable to execute when the button is released.
   *
   * @param toRun the runnable to execute.
   * @param requirements the required subsystems.
   */
  Button WhenReleased(std::function<void()> toRun,
                      wpi::span<Subsystem* const> requirements = {});

  /**
   * Binds a command to start when the button is pressed, and be canceled when
   * it is pressed again.  Takes a raw pointer, and so is non-owning; users are
   * responsible for the lifespan of the command.
   *
   * @param command The command to bind.
   * @return The button, for chained calls.
   */
  Button ToggleWhenPressed(Command* command);

  /**
   * Binds a command to start when the button is pressed, and be canceled when
   * it is pessed again.  Transfers command ownership to the button scheduler,
   * so the user does not have to worry about lifespan - rvalue refs will be
   * *moved*, lvalue refs will be *copied.*
   *
   * @param command The command to bind.
   * @return The button, for chained calls.
   */
  template <class T, typename = std::enable_if_t<std::is_base_of_v<
                         Command, std::remove_reference_t<T>>>>
  Button ToggleWhenPressed(T&& command) {
    ToggleWhenActive(std::forward<T>(command));
    return *this;
  }

  /**
   * Binds a command to be canceled when the button is pressed.  Takes a
   * raw pointer, and so is non-owning; users are responsible for the lifespan
   *  and scheduling of the command.
   *
   * @param command The command to bind.
   * @return The button, for chained calls.
   */
  Button CancelWhenPressed(Command* command);
};
}  // namespace frc
