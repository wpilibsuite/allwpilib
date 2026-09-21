// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.command3;

import static org.wpilib.util.ErrorMessages.requireNonNullParam;

import java.util.Collection;
import org.wpilib.annotation.NoDiscard;

/**
 * Tracks the running status of a set of commands using primitive run IDs and bitwise operations.
 * This approach avoids boxing and heap allocations in hot loops.
 *
 * <p>The active state of tracked commands is represented using a bitmask (or array of 64-bit words
 * for large sets), where each bit position corresponds to an index in the list of tracked commands:
 *
 * <ul>
 *   <li><b>Bit value of {@code 1}:</b> The command at that index is currently running and active.
 *   <li><b>Bit value of {@code 0}:</b> The command at that index has completed, was not running, or
 *       has been pruned from further evaluation cycles.
 * </ul>
 *
 * <p>For {@code N <= 64} commands, tracking uses a single 64-bit {@code long} bitmask where bit
 * {@code i} represents {@code m_commands[i]}. For {@code N > 64} commands, tracking uses an array
 * of 64-bit {@code long} words where bit {@code (i & 63)} of word {@code (i >>> 6)} represents
 * {@code m_commands[i]}.
 */
abstract sealed class CommandRunTracker {
  protected final Scheduler m_scheduler;
  protected final Command[] m_commands;
  protected final int[] m_expectedRunIds;
  protected int m_remainingCount;

  @SuppressWarnings("PMD.ArrayIsStoredDirectly")
  protected CommandRunTracker(Scheduler scheduler, Command[] commands, int[] expectedRunIds) {
    m_scheduler = scheduler;
    m_commands = commands;
    m_expectedRunIds = expectedRunIds;
  }

  /**
   * Checks if all tracked commands are currently running.
   *
   * @return true if all tracked commands are running, false if any command has completed or was not
   *     running
   */
  public abstract boolean areAllRunning();

  /**
   * Checks if any of the tracked commands are currently running.
   *
   * @return true if at least one tracked command is running, false if all commands have completed
   */
  public abstract boolean isAnyRunning();

  /**
   * Creates a tracker for the given collection of commands.
   *
   * @param scheduler the scheduler to query command run IDs from
   * @param commands the commands to track
   * @return a command run tracker
   */
  @NoDiscard
  public static CommandRunTracker of(Scheduler scheduler, Collection<? extends Command> commands) {
    requireNonNullParam(scheduler, "scheduler", "CommandRunTracker.of");
    requireNonNullParam(commands, "commands", "CommandRunTracker.of");
    Command[] array = commands.toArray(Command[]::new);
    return createRunTracker(scheduler, array);
  }

  /**
   * Creates a tracker for the given array of commands.
   *
   * @param scheduler the scheduler to query command run IDs from
   * @param commands the commands to track
   * @return a command run tracker
   */
  @NoDiscard
  public static CommandRunTracker of(Scheduler scheduler, Command... commands) {
    requireNonNullParam(scheduler, "scheduler", "CommandRunTracker.of");
    requireNonNullParam(commands, "commands", "CommandRunTracker.of");
    Command[] array = commands.clone();
    return createRunTracker(scheduler, array);
  }

  @NoDiscard
  private static CommandRunTracker createRunTracker(Scheduler scheduler, Command[] array) {
    int[] runIds = new int[array.length];
    for (int i = 0; i < array.length; i++) {
      requireNonNullParam(array[i], "commands[" + i + "]", "CommandRunTracker.of");
      runIds[i] = scheduler.runId(array[i]);
    }
    return array.length <= 64
        ? new Small(scheduler, array, runIds)
        : new Large(scheduler, array, runIds);
  }

  /**
   * Small variant when 64 commands or fewer are tracked, where tracking information fits into a
   * single {@code long} bitmask.
   *
   * <p>Bit {@code i} represents command {@code m_commands[i]}. A bit value of {@code 1} indicates
   * the command is running and actively tracked, while {@code 0} indicates it has completed, was
   * not running, or was pruned from further checks.
   */
  private static final class Small extends CommandRunTracker {
    /**
     * Bitmask where bit {@code i} represents the active status of command {@code m_commands[i]}
     * ({@code 1} for running, {@code 0} for completed/inactive).
     */
    private long m_activeMask;

    Small(Scheduler scheduler, Command[] commands, int[] expectedRunIds) {
      super(scheduler, commands, expectedRunIds);
      long mask = 0L;
      int count = 0;
      for (int i = 0; i < commands.length; i++) {
        if (expectedRunIds[i] != 0) {
          // Set bit i to 1: command i is active and tracked
          mask |= 1L << i;
          count++;
        }
      }
      m_activeMask = mask;
      m_remainingCount = count;
    }

    @Override
    public boolean areAllRunning() {
      if (m_commands.length == 0 || m_remainingCount < m_commands.length) {
        return false;
      }
      long mask = m_activeMask;
      while (mask != 0L) {
        int i = Long.numberOfTrailingZeros(mask);
        if (m_scheduler.runId(m_commands[i]) != m_expectedRunIds[i]) {
          // Clear bit i to 0: command i completed, prune from future checks
          m_activeMask &= ~(1L << i);
          m_remainingCount--;
          return false;
        }
        mask &= mask - 1L;
      }
      return true;
    }

    @Override
    public boolean isAnyRunning() {
      if (m_remainingCount == 0) {
        return false;
      }
      long mask = m_activeMask;
      while (mask != 0L) {
        int i = Long.numberOfTrailingZeros(mask);
        if (m_scheduler.runId(m_commands[i]) == m_expectedRunIds[i]) {
          return true;
        }
        // Clear bit i to 0: command i completed, prune from future checks
        m_activeMask &= ~(1L << i);
        m_remainingCount--;
        mask &= mask - 1L;
      }
      return false;
    }
  }

  /**
   * Large variant used when more than 64 commands are tracked, where tracking information exceeds a
   * single 64-bit {@code long} bitmask and requires an array of words ({@code long[]}).
   *
   * <p>To maximize execution efficiency on the scheduler hot path and minimize instruction latency,
   * arithmetic division and modulo operations are replaced by bit shifts and bitwise masking:
   *
   * <ul>
   *   <li><b>Bitmask Structure:</b> Bit {@code (i & 63)} of word {@code (i >>> 6)} represents
   *       command {@code m_commands[i]}. A bit value of {@code 1} indicates the command is running
   *       and actively tracked, while {@code 0} indicates it has completed, was not running, or was
   *       pruned from further checks.
   *   <li><b>Fast Ceiling Division ({@code (commands.length + 63) >>> 6}):</b> Computes {@code
   *       ceil(N / 64)} to allocate the minimum number of 64-bit words required to store all
   *       command bits without branch instructions or integer division latency.
   *   <li><b>Word and Bit Mapping ({@code i >>> 6} and {@code i & 63}):</b> Maps command index
   *       {@code i} to word index {@code floor(i / 64)} and bit position {@code i % 64} within that
   *       word, setting active bits via {@code 1L << (i & 63)}.
   *   <li><b>Index Reconstruction ({@code (w << 6) + bit}):</b> Reconstructs linear command index
   *       {@code (w * 64) + bit} during active bit scans with {@link Long#numberOfTrailingZeros}.
   * </ul>
   */
  private static final class Large extends CommandRunTracker {
    /**
     * Bit vector words where bit {@code (i & 63)} of word {@code (i >>> 6)} represents the active
     * status of command {@code m_commands[i]} ({@code 1} for running, {@code 0} for
     * completed/inactive).
     */
    private final long[] m_words;

    Large(Scheduler scheduler, Command[] commands, int[] expectedRunIds) {
      super(scheduler, commands, expectedRunIds);
      // Fast ceiling division ceil(N / 64): adding 63 promotes non-zero remainders to the next
      // word, and unsigned right-shifting by 6 divides by 64 without division instructions.
      int wordCount = (commands.length + 63) >>> 6;
      m_words = new long[wordCount];
      int count = 0;
      for (int i = 0; i < commands.length; i++) {
        if (expectedRunIds[i] != 0) {
          // Set bit (i & 63) in word (i >>> 6) to 1: command i is active and tracked.
          // i >>> 6 computes word index (floor(i / 64)); i & 63 computes bit offset (i % 64).
          m_words[i >>> 6] |= 1L << (i & 63);
          count++;
        }
      }
      m_remainingCount = count;
    }

    @Override
    public boolean areAllRunning() {
      if (m_commands.length == 0 || m_remainingCount < m_commands.length) {
        return false;
      }
      for (int w = 0; w < m_words.length; w++) {
        long mask = m_words[w];
        while (mask != 0L) {
          int bit = Long.numberOfTrailingZeros(mask);
          // Reconstruct linear command index: (w * 64) + bit
          int idx = (w << 6) + bit;
          if (m_scheduler.runId(m_commands[idx]) != m_expectedRunIds[idx]) {
            // Clear bit to 0: command idx completed, prune from future checks
            m_words[w] &= ~(1L << bit);
            m_remainingCount--;
            return false;
          }
          mask &= mask - 1L;
        }
      }
      return true;
    }

    @Override
    public boolean isAnyRunning() {
      if (m_remainingCount == 0) {
        return false;
      }
      for (int w = 0; w < m_words.length; w++) {
        long mask = m_words[w];
        while (mask != 0L) {
          int bit = Long.numberOfTrailingZeros(mask);
          // Reconstruct linear command index: (w * 64) + bit
          int i = (w << 6) + bit;
          if (m_scheduler.runId(m_commands[i]) == m_expectedRunIds[i]) {
            return true;
          }
          // Clear bit to 0: command idx completed, prune from future checks
          m_words[w] &= ~(1L << bit);
          m_remainingCount--;
          mask &= mask - 1L;
        }
      }
      return false;
    }
  }
}
