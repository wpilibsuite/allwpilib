// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj2.command.button;

import static edu.wpi.first.wpilibj.util.ErrorMessages.requireNonNullParam;

import edu.wpi.first.math.filter.Debouncer;
import edu.wpi.first.wpilibj.event.BooleanEvent;
import edu.wpi.first.wpilibj.event.EventLoop;
import edu.wpi.first.wpilibj2.command.Command;
import edu.wpi.first.wpilibj2.command.CommandScheduler;
import edu.wpi.first.wpilibj2.command.InstantCommand;
import edu.wpi.first.wpilibj2.command.Subsystem;
import java.util.function.BiFunction;
import java.util.function.BooleanSupplier;

/**
 * This class is a wrapper around {@link BooleanEvent}, providing an easy way to link commands to
 * digital inputs.
 *
 * <p>This class is provided by the NewCommands VendorDep
 */
public class Trigger extends BooleanEvent {
  /**
   * Creates a new trigger with the given condition/digital signal.
   *
   * @param loop the loop that polls this trigger
   * @param signal the digital signal represented.
   */
  public Trigger(EventLoop loop, BooleanSupplier signal) {
    super(loop, signal);
  }

  /**
   * Copies the BooleanEvent into a Trigger object.
   *
   * @param toCast the BooleanEvent
   * @return a Trigger wrapping the given BooleanEvent
   * @see BooleanEvent#castTo(BiFunction)
   */
  public static Trigger cast(BooleanEvent toCast) {
    return toCast.castTo(Trigger::new);
  }

  /**
   * Creates a new trigger with the given condition/digital signal.
   *
   * <p>Polled by the {@link CommandScheduler#getDefaultButtonLoop() default scheduler button loop}.
   *
   * @param signal the digital signal represented.
   */
  public Trigger(BooleanSupplier signal) {
    this(CommandScheduler.getInstance().getDefaultButtonLoop(), signal);
  }

  /** Creates a new trigger that is always inactive. */
  @Deprecated
  public Trigger() {
    this(() -> false);
  }

  /**
   * Returns whether or not the trigger is active.
   *
   * <p>This method will be called repeatedly a command is linked to the Trigger.
   *
   * <p>Functionally identical to {@link Trigger#getAsBoolean()}.
   *
   * @return whether or not the trigger condition is active.
   * @deprecated use {@link #getAsBoolean()}
   */
  @Deprecated
  public final boolean get() {
    return getAsBoolean();
  }

  /**
   * Starts the given command whenever the trigger just becomes active.
   *
   * @param command the command to start
   * @return this trigger, so calls can be chained
   */
  public Trigger whenActive(final Command command) {
    requireNonNullParam(command, "command", "whenActive");

    this.rising().ifHigh(command::schedule);
    return this;
  }

  /**
   * Runs the given runnable whenever the trigger just becomes active.
   *
   * @param toRun the runnable to run
   * @param requirements the required subsystems
   * @return this trigger, so calls can be chained
   */
  public Trigger whenActive(final Runnable toRun, Subsystem... requirements) {
    return whenActive(new InstantCommand(toRun, requirements));
  }

  /**
   * Constantly starts the given command while the button is held.
   *
   * <p>{@link Command#schedule()} will be called repeatedly while the trigger is active, and will
   * be canceled when the trigger becomes inactive.
   *
   * @param command the command to start
   * @return this trigger, so calls can be chained
   */
  public Trigger whileActiveContinuous(final Command command) {
    requireNonNullParam(command, "command", "whileActiveContinuous");

    this.ifHigh(command::schedule);
    this.falling().ifHigh(command::cancel);

    return this;
  }

  /**
   * Constantly runs the given runnable while the button is held.
   *
   * @param toRun the runnable to run
   * @param requirements the required subsystems
   * @return this trigger, so calls can be chained
   */
  public Trigger whileActiveContinuous(final Runnable toRun, Subsystem... requirements) {
    return whileActiveContinuous(new InstantCommand(toRun, requirements));
  }

  /**
   * Starts the given command when the trigger initially becomes active, and ends it when it becomes
   * inactive, but does not re-start it in-between.
   *
   * @param command the command to start
   * @return this trigger, so calls can be chained
   */
  public Trigger whileActiveOnce(final Command command) {
    requireNonNullParam(command, "command", "whileActiveOnce");

    this.rising().ifHigh(command::schedule);
    this.falling().ifHigh(command::cancel);

    return this;
  }

  /**
   * Starts the command when the trigger becomes inactive.
   *
   * @param command the command to start
   * @return this trigger, so calls can be chained
   */
  public Trigger whenInactive(final Command command) {
    requireNonNullParam(command, "command", "whenInactive");

    this.falling().ifHigh(command::schedule);

    return this;
  }

  /**
   * Runs the given runnable when the trigger becomes inactive.
   *
   * @param toRun the runnable to run
   * @param requirements the required subsystems
   * @return this trigger, so calls can be chained
   */
  public Trigger whenInactive(final Runnable toRun, Subsystem... requirements) {
    return whenInactive(new InstantCommand(toRun, requirements));
  }

  /**
   * Toggles a command when the trigger becomes active.
   *
   * @param command the command to toggle
   * @return this trigger, so calls can be chained
   */
  public Trigger toggleWhenActive(final Command command) {
    requireNonNullParam(command, "command", "toggleWhenActive");

    this.rising()
        .ifHigh(
            () -> {
              if (command.isScheduled()) {
                command.cancel();
              } else {
                command.schedule();
              }
            });

    return this;
  }

  /**
   * Cancels a command when the trigger becomes active.
   *
   * @param command the command to cancel
   * @return this trigger, so calls can be chained
   */
  public Trigger cancelWhenActive(final Command command) {
    requireNonNullParam(command, "command", "cancelWhenActive");

    this.rising().ifHigh(command::cancel);

    return this;
  }

  /* ----------- Super method type redeclarations ----------------- */

  @Override
  public Trigger and(BooleanSupplier trigger) {
    return cast(super.and(trigger));
  }

  @Override
  public Trigger or(BooleanSupplier trigger) {
    return cast(super.or(trigger));
  }

  @Override
  public Trigger negate() {
    return cast(super.negate());
  }

  @Override
  public Trigger debounce(double seconds) {
    return debounce(seconds, Debouncer.DebounceType.kRising);
  }

  @Override
  public Trigger debounce(double seconds, Debouncer.DebounceType type) {
    return cast(super.debounce(seconds, type));
  }

  @Override
  public Trigger rising() {
    return cast(super.rising());
  }

  @Override
  public Trigger falling() {
    return cast(super.falling());
  }
}
