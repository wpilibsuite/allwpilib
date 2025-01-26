// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj2.command;

import static org.junit.jupiter.api.Assertions.assertDoesNotThrow;
import static org.junit.jupiter.api.Assertions.assertFalse;
import static org.junit.jupiter.api.Assertions.assertTrue;
import static org.mockito.Mockito.mock;
import static org.mockito.Mockito.never;
import static org.mockito.Mockito.only;
import static org.mockito.Mockito.times;
import static org.mockito.Mockito.verify;
import static org.mockito.Mockito.when;

import java.util.Set;
import java.util.function.Supplier;
import org.junit.jupiter.api.Test;
import org.junit.jupiter.params.ParameterizedTest;
import org.junit.jupiter.params.provider.ValueSource;

class DeferredCommandTest extends CommandTestBase {
  @ParameterizedTest
  @ValueSource(booleans = {true, false})
  void deferredFunctionsTest(boolean interrupted) {
    MockCommandHolder innerCommand = new MockCommandHolder(false);
    DeferredCommand command = new DeferredCommand(innerCommand::getMock, Set.of());

    command.initialize();
    verify(innerCommand.getMock()).initialize();

    command.execute();
    verify(innerCommand.getMock()).execute();

    assertFalse(command.isFinished());
    verify(innerCommand.getMock()).isFinished();

    innerCommand.setFinished(true);
    assertTrue(command.isFinished());
    verify(innerCommand.getMock(), times(2)).isFinished();

    command.end(interrupted);
    verify(innerCommand.getMock()).end(interrupted);
  }

  @SuppressWarnings("unchecked")
  @Test
  void deferredSupplierOnlyCalledDuringInit() {
    Supplier<Command> supplier = (Supplier<Command>) mock(Supplier.class);
    when(supplier.get()).thenReturn(Commands.none(), Commands.none());

    DeferredCommand command = new DeferredCommand(supplier, Set.of());
    verify(supplier, never()).get();

    scheduler.schedule(command);
    verify(supplier, only()).get();
    scheduler.run();

    scheduler.schedule(command);
    verify(supplier, times(2)).get();
  }

  @Test
  void deferredRequirementsTest() {
    Subsystem subsystem = new Subsystem() {};
    DeferredCommand command = new DeferredCommand(Commands::none, Set.of(subsystem));

    assertTrue(command.getRequirements().contains(subsystem));
  }

  @Test
  void deferredNullCommandTest() {
    DeferredCommand command = new DeferredCommand(() -> null, Set.of());
    assertDoesNotThrow(
        () -> {
          command.initialize();
          command.execute();
          command.isFinished();
          command.end(false);
        });
  }
}
