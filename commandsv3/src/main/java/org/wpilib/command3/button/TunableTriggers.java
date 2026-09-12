// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.command3.button;

import static org.wpilib.util.ErrorMessages.requireNonNullParam;

import java.util.function.BooleanSupplier;
import org.wpilib.command3.Scheduler;
import org.wpilib.command3.Trigger;
import org.wpilib.event.EventLoop;
import org.wpilib.tunable.TunableBase;

/**
 * Static {@link Trigger} factories for reacting to tunable changes.
 *
 * <p>Each call to {@link #tuned(TunableBase)}, {@link #tuned(EventLoop, TunableBase)}, {@link
 * #tuned(Scheduler, TunableBase)}, or {@link #tuned(Scheduler, EventLoop, TunableBase)} creates an
 * independent observer of the supplied tunable. The returned trigger reports whether that observer
 * detected a tuning revision change during the current event-loop poll. It initially reports {@code
 * false}, including when the tunable was tuned before observation began. Multiple backend-applied
 * tunes between polls coalesce into one {@code true} poll, and local writes do not activate the
 * trigger.
 *
 * <p>Use {@link Trigger#ifTrue(org.wpilib.command3.Command)} to react to every poll containing a
 * tune. {@link Trigger#onTrue(org.wpilib.command3.Command)} keeps ordinary rising-edge semantics
 * and merges consecutive {@code true} polls.
 *
 * <pre>{@code
 * TunableTriggers.tuned(kP).ifTrue(applyGainsCommand);
 *
 * TunableTriggers.tuned(kP)
 *     .or(TunableTriggers.tuned(kI))
 *     .or(TunableTriggers.tuned(kD))
 *     .ifTrue(applyGainsCommand);
 *
 * Scheduler calibrationScheduler = Scheduler.createIndependentScheduler();
 * TunableTriggers.tuned(calibrationScheduler, kP).ifTrue(applyGainsCommand);
 * }</pre>
 *
 * <p>Triggers created with an explicit {@link EventLoop} are only updated when that loop is polled
 * by its owner. Command scheduling still follows the selected {@link Scheduler scheduler's} rules;
 * an explicit loop without an explicit scheduler uses {@link Scheduler#getDefault()} for command
 * scheduling.
 */
public final class TunableTriggers {
  private TunableTriggers() {
    throw new UnsupportedOperationException("This is a utility class!");
  }

  /**
   * Creates a trigger for detecting backend-applied tunes to a tunable.
   *
   * <p>The trigger uses {@link Scheduler#getDefault()} for command scheduling and is polled by that
   * scheduler's {@link Scheduler#getDefaultEventLoop()}.
   *
   * @param tunable the tunable to observe; the returned trigger does not copy it
   * @return a trigger that is true for polls where this observer detected a new tuning revision
   */
  public static Trigger tuned(TunableBase tunable) {
    return tuned(Scheduler.getDefault(), tunable);
  }

  /**
   * Creates a trigger for detecting backend-applied tunes to a tunable.
   *
   * <p>The trigger uses {@link Scheduler#getDefault()} for command scheduling. The supplied event
   * loop must be polled by its owner.
   *
   * @param loop the event loop that polls this trigger
   * @param tunable the tunable to observe; the returned trigger does not copy it
   * @return a trigger that is true for polls where this observer detected a new tuning revision
   */
  public static Trigger tuned(EventLoop loop, TunableBase tunable) {
    return tuned(Scheduler.getDefault(), loop, tunable);
  }

  /**
   * Creates a trigger for detecting backend-applied tunes to a tunable.
   *
   * <p>The trigger is polled by the supplied scheduler's {@link Scheduler#getDefaultEventLoop()}.
   *
   * @param scheduler the scheduler that should execute triggered commands
   * @param tunable the tunable to observe; the returned trigger does not copy it
   * @return a trigger that is true for polls where this observer detected a new tuning revision
   */
  public static Trigger tuned(Scheduler scheduler, TunableBase tunable) {
    Scheduler checkedScheduler = requireNonNullParam(scheduler, "scheduler", "tuned");
    return tuned(checkedScheduler, checkedScheduler.getDefaultEventLoop(), tunable);
  }

  /**
   * Creates a trigger for detecting backend-applied tunes to a tunable.
   *
   * <p>The supplied event loop must be polled by its owner. The returned trigger observes the
   * supplied object directly, so the tunable must outlive the returned trigger and any bindings
   * made from it.
   *
   * @param scheduler the scheduler that should execute triggered commands
   * @param loop the event loop that polls this trigger
   * @param tunable the tunable to observe; the returned trigger does not copy it
   * @return a trigger that is true for polls where this observer detected a new tuning revision
   */
  public static Trigger tuned(Scheduler scheduler, EventLoop loop, TunableBase tunable) {
    Scheduler checkedScheduler = requireNonNullParam(scheduler, "scheduler", "tuned");
    EventLoop checkedLoop = requireNonNullParam(loop, "loop", "tuned");
    TunableBase checkedTunable = requireNonNullParam(tunable, "tunable", "tuned");
    return new Trigger(
        checkedScheduler,
        checkedLoop,
        new BooleanSupplier() {
          private long m_lastSeen = checkedTunable.getTuneRevision();

          @Override
          public boolean getAsBoolean() {
            long current = checkedTunable.getTuneRevision();
            boolean tuned = current != m_lastSeen;
            m_lastSeen = current;
            return tuned;
          }
        });
  }
}
