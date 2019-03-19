/*----------------------------------------------------------------------------*/
/* Copyright (c) 2008-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.experimental.buttons;

import edu.wpi.first.wpilibj.experimental.command.Command;

/**
 * This class provides an easy way to link commands to OI inputs.
 *
 * <p>It is very easy to link a button to a command. For instance, you could link the trigger button
 * of a joystick to a "score" command.
 *
 * <p>This class represents a subclass of Trigger that is specifically aimed at buttons on an
 * operator interface as a common use case of the more generalized Trigger objects. This is a simple
 * wrapper around Trigger with the method names renamed to fit the Button object use.
 */
@SuppressWarnings("PMD.TooManyMethods")
public abstract class Button extends Trigger {

  /**
   * Starts the given command whenever the button is newly pressed.
   *
   * @param command       the command to start
   * @param interruptible whether the command is interruptible
   */
  public void whenPressed(final Command command, boolean interruptible) {
    whenActive(command, interruptible);
  }

  /**
   * Starts the given command whenever the button is newly pressed. The command is set to be
   * interruptible.
   *
   * @param command the command to start
   */
  public void whenPressed(final Command command) {
    whenActive(command);
  }

  /**
   * Runs the given runnable whenever the button is newly pressed.
   *
   * @param toRun the runnable to run
   */
  public void whenPressed(final Runnable toRun) {
    whenActive(toRun);
  }

  /**
   * Constantly starts the given command while the button is held.
   *
   * {@link Command#schedule(boolean)} will be called repeatedly while the button is held, and will
   * be canceled when the button is released.
   *
   * @param command       the command to start
   * @param interruptible whether the command is interruptible
   */
  public void whileHeld(final Command command, boolean interruptible) {
    whileActiveContinuous(command, interruptible);
  }

  /**
   * Constantly starts the given command while the button is held.
   *
   * {@link Command#schedule(boolean)} will be called repeatedly while the button is held, and will
   * be canceled when the button is released.  The command is set to be interruptible.
   *
   * @param command the command to start
   */
  public void whileHeld(final Command command) {
    whileActiveContinuous(command);
  }

  /**
   * Constantly runs the given runnable while the button is held.
   *
   * @param toRun the runnable to run
   */
  public void whileHeld(final Runnable toRun) {
    whileActiveContinuous(toRun);
  }

  /**
   * Starts the given command when the button is first pressed, and cancels it when it is released,
   * but does not start it again if it ends or is otherwise interrupted.
   *
   * @param command       the command to start
   * @param interruptible whether the command is interruptible
   */
  public void whenHeld(final Command command, boolean interruptible) {
    whileActiveOnce(command, interruptible);
  }

  /**
   * Starts the given command when the button is first pressed, and cancels it when it is released,
   * but does not start it again if it ends or is otherwise interrupted.  The command is set
   * to be interruptible.
   *
   * @param command the command to start
   */
  public void whenHeld(final Command command) {
    whileActiveOnce(command, true);
  }


  /**
   * Starts the command when the button is released.
   *
   * @param command       the command to start
   * @param interruptible whether the command is interruptible
   */
  public void whenReleased(final Command command, boolean interruptible) {
    whenInactive(command, interruptible);
  }

  /**
   * Starts the command when the button is released.  The command is set to be interruptible.
   *
   * @param command the command to start
   */
  public void whenReleased(final Command command) {
    whenInactive(command);
  }

  /**
   * Runs the given runnable when the button is released.
   *
   * @param toRun the runnable to run
   */
  public void whenReleased(final Runnable toRun) {
    whenInactive(toRun);
  }

  /**
   * Toggles the command whenever the button is pressed (on then off then on).
   *
   * @param command       the command to start
   * @param interruptible whether the command is interruptible
   */
  public void toggleWhenPressed(final Command command, boolean interruptible) {
    toggleWhenActive(command, interruptible);
  }

  /**
   * Toggles the command whenever the button is pressed (on then off then on).  The command is
   * set to be interruptible.
   *
   * @param command the command to start
   */
  public void toggleWhenPressed(final Command command) {
    toggleWhenActive(command);
  }

  /**
   * Cancel the command when the button is pressed.
   *
   * @param command the command to start
   */
  public void cancelWhenPressed(final Command command) {
    cancelWhenActive(command);
  }
}
