// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.command3;

import static org.wpilib.util.ErrorMessages.requireNonNullParam;

import java.util.ArrayList;
import java.util.Collection;
import java.util.HashSet;
import java.util.List;
import java.util.Set;
import java.util.stream.Collectors;

/** Utility class for helping with detecting conflicts between commands. */
public final class ConflictDetector {
  private ConflictDetector() {
    // This is a utility class!
  }

  /**
   * A conflict between two commands.
   *
   * @param a The first conflicting command.
   * @param b The second conflicting command.
   * @param sharedRequirements The set of mechanisms required by both commands. This set is
   *     read-only
   */
  public record Conflict(Command a, Command b, Set<Mechanism> sharedRequirements) {
    /**
     * Gets a descriptive message for the conflict. The description includes the names of the
     * conflicting commands and the names of all mechanisms required by both commands.
     *
     * @return A description of the conflict.
     */
    public String description() {
      var shared =
          sharedRequirements.stream()
              .map(Mechanism::getName)
              .sorted()
              .collect(Collectors.joining(", "));
      return "%s and %s both require %s".formatted(a.name(), b.name(), shared);
    }
  }

  /**
   * Validates that a set of commands have no internal requirement conflicts. An error is thrown if
   * a conflict is detected.
   *
   * @param commands The commands to validate
   * @throws IllegalArgumentException If at least one pair of commands is found in the input where
   *     both commands have at least one required mechanism in common
   */
  public static void throwIfConflicts(Collection<? extends Command> commands) {
    requireNonNullParam(commands, "commands", "ConflictDetector.throwIfConflicts");

    var conflicts = findAllConflicts(commands);
    if (conflicts.isEmpty()) {
      // No conflicts, all good
      return;
    }

    StringBuilder message =
        new StringBuilder("Commands running in parallel cannot share requirements: ");
    for (int i = 0; i < conflicts.size(); i++) {
      Conflict conflict = conflicts.get(i);
      message.append(conflict.description());
      if (i < conflicts.size() - 1) {
        message.append("; ");
      }
    }

    throw new IllegalArgumentException(message.toString());
  }

  /**
   * Finds all conflicting pairs of commands in the input collection.
   *
   * @param commands The commands to check.
   * @return All detected conflicts. The returned list is empty if no conflicts were found.
   */
  @SuppressWarnings("PMD.CompareObjectsWithEquals")
  public static List<Conflict> findAllConflicts(Collection<? extends Command> commands) {
    requireNonNullParam(commands, "commands", "ConflictDetector.findAllConflicts");

    List<Conflict> conflicts = new ArrayList<>();

    int i = 0;
    for (Command command : commands) {
      i++;
      int j = 0;
      for (Command other : commands) {
        j++;
        if (j <= i) {
          // Skip all elements in 0..i so the inner loop only checks elements from i + 1 onward.
          // Ordering of the elements in the pair doesn't matter, and this prevents finding every
          // pair twice eg (A, B) and (B, A).
          continue;
        }

        if (command == other) {
          // Commands cannot conflict with themselves, so just in case the input collection happens
          // to have duplicate elements we just skip any pairs of a command with itself.
          continue;
        }

        if (command.conflictsWith(other)) {
          conflicts.add(findConflict(command, other));
        }
      }
    }

    return conflicts;
  }

  private static Conflict findConflict(Command a, Command b) {
    Set<Mechanism> sharedRequirements = new HashSet<>(a.requirements());
    sharedRequirements.retainAll(b.requirements());
    return new Conflict(a, b, Set.copyOf(sharedRequirements));
  }
}
