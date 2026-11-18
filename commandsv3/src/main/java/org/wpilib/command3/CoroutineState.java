// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.command3;

import java.util.function.BooleanSupplier;
import org.wpilib.system.RobotController;

/**
 * The possible states a coroutine can be in. When the scheduler attempts to tick a command, it
 * first checks the state of its coroutine. Any coroutine in a {@link #shouldMount() mountable}
 * state will be mounted and ticked; any coroutine not in a mountable state will be skipped and no
 * further action will be taken on it in that cycle.
 */
sealed interface CoroutineState {
  /**
   * Singleton instance of the {@link Live} state. Coroutines start in this state, which is always
   * mountable.
   */
  CoroutineState LIVE = new Live();

  /**
   * Singleton instance of the {@link Frozen} state. Coroutines in this state will never be mounted,
   * but their commands will remain in the scheduler until an external source cancels them.
   */
  CoroutineState FROZEN = new Frozen();

  /**
   * Singleton instance of the {@link Canceled} state. Coroutines in this state will never be
   * mounted, and their commands will be removed from the scheduler in the next scheduler run. This
   * is used only when the scheduler is processing its commands and the currently loaded command
   * requests self-cancellation. The normal {@link org.wpilib.command3.Scheduler#cancel(Command)}
   * flow will immediately cancel the command without needing to move its coroutine to this state.
   */
  CoroutineState CANCELED = new Canceled();

  /**
   * Creates a new {@link ParkedUntil} state with the given target timestamp. The scheduler will
   * only un-park the coroutine when the current time is greater than or equal to the target
   * timestamp.
   *
   * @param targetTimestamp The timestamp at which the coroutine should be un-parked
   * @return The new {@link ParkedUntil} state
   */
  static ParkedUntil parkedUntil(long targetTimestampNanos) {
    return new ParkedUntil(targetTimestampNanos);
  }

  /**
   * Creates a new {@link ParkedOnCondition} state with the given condition. The scheduler will only
   * un-park the coroutine when the condition returns true.
   *
   * @param condition The condition that must be true for the coroutine to be un-parked
   * @return The new {@link ParkedOnCondition} state
   */
  static ParkedOnCondition parkedUntil(BooleanSupplier condition) {
    return new ParkedOnCondition(condition);
  }

  /**
   * Creates a new {@link ParkedWhile} state with the given condition. The scheduler will only
   * un-park the coroutine when the condition returns false.
   *
   * @param condition The condition that must be false for the coroutine to be un-parked
   * @return The new {@link ParkedWhile} state
   */
  static ParkedWhile parkedWhile(BooleanSupplier condition) {
    return new ParkedWhile(condition);
  }

  /**
   * Creates a new {@link ForkFailed} state with the given result. The scheduler will immediately
   * cancel the coroutine and remove it from the scheduler. A coroutine only enters this state if it
   * would {@link Coroutine#setCancelOnForkFailure(boolean) self-cancel on fork failures} (which is
   * the default behavior unless opted out of by user code) and a fork operation failed to fork a
   * command.
   *
   * @param failure The result of the fork operation that failed
   * @return The new {@link ForkFailed} state
   */
  static ForkFailed forkFailed(Coroutine.ForkResult failure) {
    return new ForkFailed(failure);
  }

  /**
   * Checks if a coroutine in this state should be mounted by the scheduler when its turn comes.
   *
   * @return true if the scheduler should mount the coroutine, false if it should be skipped
   */
  boolean shouldMount();

  /** A coroutine in this state is considered live and should always be mounted by the scheduler. */
  record Live() implements CoroutineState {
    @Override
    public boolean shouldMount() {
      return true;
    }

    @Override
    public String toString() {
      return "LIVE";
    }
  }

  /**
   * A coroutine in this state is considered frozen and should never be mounted by the scheduler.
   */
  record Frozen() implements CoroutineState {
    @Override
    public boolean shouldMount() {
      return false;
    }

    @Override
    public String toString() {
      return "FROZEN";
    }
  }

  /**
   * A coroutine in this state is considered parked and should only be mounted by the scheduler when
   * the target timestamp is reached.
   *
   * @param targetTimestampNanos The timestamp at which the coroutine should be un-parked
   */
  record ParkedUntil(long targetTimestampNanos) implements CoroutineState {
    boolean elapsed() {
      return RobotController.getTime() >= targetTimestampNanos;
    }

    @Override
    public boolean shouldMount() {
      return elapsed();
    }

    @Override
    public String toString() {
      return "ParkedUntil[" + targetTimestampNanos + "ns]";
    }
  }

  /**
   * A coroutine in this state is considered parked and should only be mounted by the scheduler when
   * the condition returns true.
   *
   * @param condition The condition that must be true for the coroutine to be un-parked
   */
  record ParkedOnCondition(BooleanSupplier condition) implements CoroutineState {
    public boolean conditionMet() {
      return condition.getAsBoolean();
    }

    @Override
    public boolean shouldMount() {
      return conditionMet();
    }

    @Override
    public String toString() {
      return "ParkedOnCondition[dynamic]";
    }
  }

  /**
   * A coroutine in this state is considered parked and should only be mounted by the scheduler when
   * the condition returns false.
   *
   * @param condition The condition that must be false for the coroutine to be un-parked
   */
  record ParkedWhile(BooleanSupplier condition) implements CoroutineState {
    public boolean conditionMet() {
      return !condition.getAsBoolean();
    }

    @Override
    public boolean shouldMount() {
      return conditionMet();
    }

    @Override
    public String toString() {
      return "ParkedWhile[dynamic]";
    }
  }

  /**
   * A coroutine in this state is considered canceled and should never be mounted by the scheduler.
   */
  record Canceled() implements CoroutineState {
    @Override
    public boolean shouldMount() {
      return false;
    }

    @Override
    public String toString() {
      return "CANCELED";
    }
  }

  /**
   * A coroutine in this state is considered failed and should never be mounted by the scheduler.
   *
   * @param failure The result of the fork operation that failed
   */
  record ForkFailed(Coroutine.ForkResult failure) implements CoroutineState {
    @Override
    public boolean shouldMount() {
      return false;
    }

    @Override
    public String toString() {
      return "FORK_FAILED[" + failure + "]";
    }
  }
}
