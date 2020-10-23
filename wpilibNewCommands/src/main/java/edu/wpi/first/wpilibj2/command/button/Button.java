/*----------------------------------------------------------------------------*/
/* Copyright (c) 2008-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj2.command.button;

import java.util.function.BooleanSupplier;

import edu.wpi.first.wpilibj2.command.Command;
import edu.wpi.first.wpilibj2.command.Subsystem;

/**
 * This class provides an easy way to link commands to OI inputs.
 *
 * <p>It is very easy to link a button to a command. For instance, you could link the trigger
 * button of a joystick to a "score" command.
 *
 * <p>This class represents a subclass of Trigger that is specifically aimed at buttons on an
 * operator interface as a common use case of the more generalized Trigger objects. This is a simple
 * wrapper around Trigger with the method names renamed to fit the Button object use.
 */
public class Button extends Trigger {
  /**
   * Default constructor; creates a button that is never pressed (unless {@link Button#get()} is
   * overridden).
   */
  public Button() {
  }

  /**
   * Creates a new button with the given condition determining whether it is pressed.
   *
   * @param isPressed returns whether or not the trigger should be active
   */
  public Button(BooleanSupplier isPressed) {
    super(isPressed);
  }

  /**
   * Starts the given command whenever the button is newly pressed.
   *
   * @param command       the command to start
   * @param interruptible whether the command is interruptible
   * @return this button, so calls can be chained
   */
  public Button whenPressed(final Command command, boolean interruptible) {
    whenActive(command, interruptible);
    return this;
  }

  /**
   * Starts the given command whenever the button is newly pressed. The command is set to be
   * interruptible.
   *
   * @param command the command to start
   * @return this button, so calls can be chained
   */
  public Button whenPressed(final Command command) {
    whenActive(command);
    return this;
  }

  /**
   * Runs the given runnable whenever the button is newly pressed.
   *
   * @param toRun        the runnable to run
   * @param requirements the required subsystems
   * @return this button, so calls can be chained
   */
  public Button whenPressed(final Runnable toRun, Subsystem... requirements) {
    whenActive(toRun, requirements);
    return this;
  }

  /**
   * Constantly starts the given command while the button is held.
   *
   * {@link Command#schedule(boolean)} will be called repeatedly while the button is held, and will
   * be canceled when the button is released.
   *
   * @param command       the command to start
   * @param interruptible whether the command is interruptible
   * @return this button, so calls can be chained
   */
  public Button whileHeld(final Command command, boolean interruptible) {
    whileActiveContinuous(command, interruptible);
    return this;
  }

  /**
   * Constantly starts the given command while the button is held.
   *
   * {@link Command#schedule(boolean)} will be called repeatedly while the button is held, and will
   * be canceled when the button is released.  The command is set to be interruptible.
   *
   * @param command the command to start
   * @return this button, so calls can be chained
   */
  public Button whileHeld(final Command command) {
    whileActiveContinuous(command);
    return this;
  }

  /**
   * Constantly runs the given runnable while the button is held.
   *
   * @param toRun        the runnable to run
   * @param requirements the required subsystems
   * @return this button, so calls can be chained
   */
  public Button whileHeld(final Runnable toRun, Subsystem... requirements) {
    whileActiveContinuous(toRun, requirements);
    return this;
  }

  /**
   * Starts the given command when the button is first pressed, and cancels it when it is released,
   * but does not start it again if it ends or is otherwise interrupted.
   *
   * @param command       the command to start
   * @param interruptible whether the command is interruptible
   * @return this button, so calls can be chained
   */
  public Button whenHeld(final Command command, boolean interruptible) {
    whileActiveOnce(command, interruptible);
    return this;
  }

  /**
   * Starts the given command when the button is first pressed, and cancels it when it is released,
   * but does not start it again if it ends or is otherwise interrupted.  The command is set to be
   * interruptible.
   *
   * @param command the command to start
   * @return this button, so calls can be chained
   */
  public Button whenHeld(final Command command) {
    whileActiveOnce(command, true);
    return this;
  }


  /**
   * Starts the command when the button is released.
   *
   * @param command       the command to start
   * @param interruptible whether the command is interruptible
   * @return this button, so calls can be chained
   */
  public Button whenReleased(final Command command, boolean interruptible) {
    whenInactive(command, interruptible);
    return this;
  }

  /**
   * Starts the command when the button is released.  The command is set to be interruptible.
   *
   * @param command the command to start
   * @return this button, so calls can be chained
   */
  public Button whenReleased(final Command command) {
    whenInactive(command);
    return this;
  }

  /**
   * Runs the given runnable when the button is released.
   *
   * @param toRun        the runnable to run
   * @param requirements the required subsystems
   * @return this button, so calls can be chained
   */
  public Button whenReleased(final Runnable toRun, Subsystem... requirements) {
    whenInactive(toRun, requirements);
    return this;
  }

  /**
   * Toggles the command whenever the button is pressed (on then off then on).
   *
   * @param command       the command to start
   * @param interruptible whether the command is interruptible
   */
  public Button toggleWhenPressed(final Command command, boolean interruptible) {
    toggleWhenActive(command, interruptible);
    return this;
  }

  /**
   * Toggles the command whenever the button is pressed (on then off then on).  The command is set
   * to be interruptible.
   *
   * @param command the command to start
   * @return this button, so calls can be chained
   */
  public Button toggleWhenPressed(final Command command) {
    toggleWhenActive(command);
    return this;
  }

  /**
   * Cancels the command when the button is pressed.
   *
   * @param command the command to start
   * @return this button, so calls can be chained
   */
  public Button cancelWhenPressed(final Command command) {
    cancelWhenActive(command);
    return this;
  }
}
