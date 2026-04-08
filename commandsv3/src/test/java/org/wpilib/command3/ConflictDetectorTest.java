// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.command3;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertThrows;
import static org.wpilib.command3.ConflictDetector.findAllConflicts;
import static org.wpilib.command3.ConflictDetector.throwIfConflicts;

import java.util.List;
import java.util.Set;
import org.junit.jupiter.api.Test;
import org.wpilib.command3.ConflictDetector.Conflict;

class ConflictDetectorTest extends CommandTestBase {
  @Test
  void emptyInputHasNoConflicts() {
    var conflicts = findAllConflicts(List.of());
    assertEquals(0, conflicts.size());
  }

  @Test
  void singleInputHasNoConflicts() {
    var mech = new Mechanism("Mech", m_scheduler);
    var command = Command.requiring(mech).executing(Coroutine::park).named("Command");
    var conflicts = findAllConflicts(List.of(command));
    assertEquals(0, conflicts.size());
  }

  @Test
  void commandDoesNotConflictWithSelf() {
    var mech = new Mechanism("Mech", m_scheduler);
    var command = Command.requiring(mech).executing(Coroutine::park).named("Command");
    var conflicts = findAllConflicts(List.of(command, command));
    assertEquals(0, conflicts.size());
  }

  @Test
  void detectManyConflicts() {
    var mech1 = new Mechanism("Mech 1", m_scheduler);
    var mech2 = new Mechanism("Mech 2", m_scheduler);

    var command1 = Command.requiring(mech1, mech2).executing(Coroutine::park).named("Command1");
    var command2 = Command.requiring(mech1).executing(Coroutine::park).named("Command2");
    var command3 = Command.requiring(mech2).executing(Coroutine::park).named("Command3");
    var command4 = Command.requiring(mech2, mech1).executing(Coroutine::park).named("Command4");
    var allCommands = List.of(command1, command2, command3, command4);

    var conflicts = findAllConflicts(allCommands);
    assertEquals(5, conflicts.size(), "Five conflicting pairs should have been found");
    assertEquals(new Conflict(command1, command2, Set.of(mech1)), conflicts.get(0));
    assertEquals(new Conflict(command1, command3, Set.of(mech2)), conflicts.get(1));
    assertEquals(new Conflict(command1, command4, Set.of(mech1, mech2)), conflicts.get(2));
    assertEquals(new Conflict(command2, command4, Set.of(mech1)), conflicts.get(3));
    assertEquals(new Conflict(command3, command4, Set.of(mech2)), conflicts.get(4));

    // error messaging
    var error = assertThrows(IllegalArgumentException.class, () -> throwIfConflicts(allCommands));
    assertEquals(
        "Commands running in parallel cannot share requirements: "
            + "Command1 and Command2 both require Mech 1; "
            + "Command1 and Command3 both require Mech 2; "
            + "Command1 and Command4 both require Mech 1, Mech 2; "
            + "Command2 and Command4 both require Mech 1; "
            + "Command3 and Command4 both require Mech 2",
        error.getMessage());
  }
}
