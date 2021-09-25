// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.command;

import static org.junit.jupiter.api.Assertions.assertAll;
import static org.junit.jupiter.api.Assertions.assertDoesNotThrow;
import static org.junit.jupiter.api.Assertions.assertEquals;

import org.junit.jupiter.api.BeforeEach;

/** The basic test for all {@link Command} tests. */
public abstract class AbstractCommandTest {
  @BeforeEach
  void commandSetup() {
    Scheduler.getInstance().removeAll();
    Scheduler.getInstance().enable();
  }

  public static class ASubsystem extends Subsystem {
    Command m_command;

    @Override
    protected void initDefaultCommand() {
      if (m_command != null) {
        setDefaultCommand(m_command);
      }
    }

    public void init(Command command) {
      m_command = command;
    }
  }

  protected void assertCommandState(
      MockCommand command, int initialize, int execute, int isFinished, int end, int interrupted) {
    assertAll(
        () -> assertEquals(initialize, command.getInitializeCount()),
        () -> assertEquals(execute, command.getExecuteCount()),
        () -> assertEquals(isFinished, command.getIsFinishedCount()),
        () -> assertEquals(end, command.getEndCount()),
        () -> assertEquals(interrupted, command.getInterruptedCount()));
  }

  protected void sleep(int time) {
    assertDoesNotThrow(() -> Thread.sleep(time));
  }
}
