// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj2.command.button;

import edu.wpi.first.wpilibj2.command.Command;
import edu.wpi.first.wpilibj2.command.Subsystem;
import java.util.function.BooleanSupplier;

/**
 * This class provides an easy way to link commands to OI inputs.
 *
 * <p>It is very easy to link a button to a command. For instance, you could link the trigger button
 * of a joystick to a "score" command.
 *
 * <p>This class represents a subclass of Trigger that is specifically aimed at buttons on an
 * operator interface as a common use case of the more generalized Trigger objects. This is a simple
 * wrapper around Trigger with the method names renamed to fit the Button object use.
 *
 * @deprecated Replace with {@link Trigger}.
 */
@Deprecated
public class Button extends Trigger {
  /**
   * Default constructor; creates a button that is never pressed.
   *
   * @deprecated Replace with {@code new Button(() -> false) }.
   */
  @Deprecated(since = "2023")
  public Button() {}

  /**
   * Creates a new button with the given condition determining whether it is pressed.
   *
   * @param isPressed returns whether or not the trigger should be active
   * @deprecated Replace with Trigger.
   */
  @Deprecated
  public Button(BooleanSupplier isPressed) {
    super(isPressed);
  }

  /**
   * Starts the given command whenever the button is newly pressed.
   *
   * @param command the command to start
   * @return this button, so calls can be chained
   * @deprecated Replace with {@link Trigger#onTrue(Command)}
   */
  @Deprecated
  public Button whenPressed(final Command command) {
    whenActive(command);
    return this;
  }

  /**
   * Runs the given runnable whenever the button is newly pressed.
   *
   * @param toRun the runnable to run
   * @param requirements the required subsystems
   * @return this button, so calls can be chained
   * @deprecated Replace with {@link #onTrue(Command)}, creating the InstantCommand manually
   */
  @Deprecated
  public Button whenPressed(final Runnable toRun, Subsystem... requirements) {
    whenActive(toRun, requirements);
    return this;
  }

  /**
   * Constantly starts the given command while the button is held.
   *
   * <p>{@link Command#schedule()} will be called repeatedly while the button is held, and will be
   * canceled when the button is released.
   *
   * @param command the command to start
   * @return this button, so calls can be chained
   * @deprecated Use {@link #whileTrue(Command)} with {@link
   *     edu.wpi.first.wpilibj2.command.RepeatCommand RepeatCommand}.
   */
  @Deprecated
  public Button whileHeld(final Command command) {
    whileActiveContinuous(command);
    return this;
  }

  /**
   * Constantly runs the given runnable while the button is held.
   *
   * @param toRun the runnable to run
   * @param requirements the required subsystems
   * @return this button, so calls can be chained
   * @deprecated Use {@link #whileTrue(Command)} and construct a RunCommand manually
   */
  @Deprecated
  public Button whileHeld(final Runnable toRun, Subsystem... requirements) {
    whileActiveContinuous(toRun, requirements);
    return this;
  }

  /**
   * Starts the given command when the button is first pressed, and cancels it when it is released,
   * but does not start it again if it ends or is otherwise interrupted.
   *
   * @param command the command to start
   * @return this button, so calls can be chained
   * @deprecated Replace with {@link Trigger#whileTrue(Command)}
   */
  @Deprecated
  public Button whenHeld(final Command command) {
    whileActiveOnce(command);
    return this;
  }

  /**
   * Starts the command when the button is released. The command is set to be interruptible.
   *
   * @param command the command to start
   * @return this button, so calls can be chained
   * @deprecated Replace with {@link Trigger#onFalse(Command)}
   */
  @Deprecated
  public Button whenReleased(final Command command) {
    whenInactive(command);
    return this;
  }

  /**
   * Runs the given runnable when the button is released.
   *
   * @param toRun the runnable to run
   * @param requirements the required subsystems
   * @return this button, so calls can be chained
   * @deprecated Replace with {@link Trigger#onFalse(Command)}, creating the InstantCommand manually
   */
  @Deprecated
  public Button whenReleased(final Runnable toRun, Subsystem... requirements) {
    whenInactive(toRun, requirements);
    return this;
  }

  /**
   * Toggles the command whenever the button is pressed (on then off then on). The command is set to
   * be interruptible.
   *
   * @param command the command to start
   * @return this button, so calls can be chained
   * @deprecated Replace with {@link Trigger#toggleOnTrue(Command)}
   */
  @Deprecated
  public Button toggleWhenPressed(final Command command) {
    toggleWhenActive(command);
    return this;
  }

  /**
   * Cancels the command when the button is pressed.
   *
   * @param command the command to start
   * @return this button, so calls can be chained
   * @deprecated Instead, pass this as an end condition to {@link Command#until(BooleanSupplier)}.
   */
  @Deprecated
  public Button cancelWhenPressed(final Command command) {
    cancelWhenActive(command);
    return this;
  }
}
