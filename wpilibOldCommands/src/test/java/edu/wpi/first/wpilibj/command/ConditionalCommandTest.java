// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.command;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertSame;
import static org.junit.jupiter.api.Assertions.assertTrue;

import org.junit.jupiter.api.BeforeEach;
import org.junit.jupiter.api.Test;

class ConditionalCommandTest extends AbstractCommandTest {
  MockConditionalCommand m_command;
  MockConditionalCommand m_commandNull;
  MockCommand m_onTrue;
  MockCommand m_onFalse;
  MockSubsystem m_subsys;
  Boolean m_condition;

  @BeforeEach
  void initCommands() {
    m_subsys = new MockSubsystem();
    m_onTrue = new MockCommand(m_subsys);
    m_onFalse = new MockCommand(m_subsys);
    m_command = new MockConditionalCommand(m_onTrue, m_onFalse);
    m_commandNull = new MockConditionalCommand(m_onTrue, null);
  }

  protected void assertConditionalCommandState(
      MockConditionalCommand command,
      int initialize,
      int execute,
      int isFinished,
      int end,
      int interrupted) {
    assertEquals(initialize, command.getInitializeCount());
    assertEquals(execute, command.getExecuteCount());
    assertEquals(isFinished, command.getIsFinishedCount());
    assertEquals(end, command.getEndCount());
    assertEquals(interrupted, command.getInterruptedCount());
  }

  @Test
  void onTrueTest() {
    m_command.setCondition(true);

    Scheduler.getInstance().add(m_command);
    assertCommandState(m_onTrue, 0, 0, 0, 0, 0);
    assertCommandState(m_onFalse, 0, 0, 0, 0, 0);
    assertConditionalCommandState(m_command, 0, 0, 0, 0, 0);
    Scheduler.getInstance().run(); // init command and select m_onTrue
    assertCommandState(m_onTrue, 0, 0, 0, 0, 0);
    assertCommandState(m_onFalse, 0, 0, 0, 0, 0);
    assertConditionalCommandState(m_command, 0, 0, 0, 0, 0);
    Scheduler.getInstance().run(); // init m_onTrue
    assertCommandState(m_onTrue, 0, 0, 0, 0, 0);
    assertCommandState(m_onFalse, 0, 0, 0, 0, 0);
    assertConditionalCommandState(m_command, 1, 1, 1, 0, 0);
    Scheduler.getInstance().run();
    assertCommandState(m_onTrue, 1, 1, 1, 0, 0);
    assertCommandState(m_onFalse, 0, 0, 0, 0, 0);
    assertConditionalCommandState(m_command, 1, 2, 2, 0, 0);
    Scheduler.getInstance().run();
    assertCommandState(m_onTrue, 1, 2, 2, 0, 0);
    assertCommandState(m_onFalse, 0, 0, 0, 0, 0);
    assertConditionalCommandState(m_command, 1, 3, 3, 0, 0);
    m_onTrue.setHasFinished(true);
    Scheduler.getInstance().run();
    assertCommandState(m_onTrue, 1, 3, 3, 1, 0);
    assertCommandState(m_onFalse, 0, 0, 0, 0, 0);
    assertConditionalCommandState(m_command, 1, 4, 4, 0, 0);
    Scheduler.getInstance().run();
    assertCommandState(m_onTrue, 1, 3, 3, 1, 0);
    assertCommandState(m_onFalse, 0, 0, 0, 0, 0);
    assertConditionalCommandState(m_command, 1, 5, 5, 1, 0);

    assertTrue(m_onTrue.getInitializeCount() > 0, "Did not initialize the true command");
    assertSame(m_onFalse.getInitializeCount(), 0, "Initialized the false command");
  }

  @Test
  void onFalseTest() {
    m_command.setCondition(false);

    Scheduler.getInstance().add(m_command);
    assertCommandState(m_onFalse, 0, 0, 0, 0, 0);
    assertCommandState(m_onTrue, 0, 0, 0, 0, 0);
    assertConditionalCommandState(m_command, 0, 0, 0, 0, 0);
    Scheduler.getInstance().run(); // init command and select m_onFalse
    assertCommandState(m_onFalse, 0, 0, 0, 0, 0);
    assertCommandState(m_onTrue, 0, 0, 0, 0, 0);
    assertConditionalCommandState(m_command, 0, 0, 0, 0, 0);
    Scheduler.getInstance().run(); // init m_onFalse
    assertCommandState(m_onFalse, 0, 0, 0, 0, 0);
    assertCommandState(m_onTrue, 0, 0, 0, 0, 0);
    assertConditionalCommandState(m_command, 1, 1, 1, 0, 0);
    Scheduler.getInstance().run();
    assertCommandState(m_onFalse, 1, 1, 1, 0, 0);
    assertCommandState(m_onTrue, 0, 0, 0, 0, 0);
    assertConditionalCommandState(m_command, 1, 2, 2, 0, 0);
    Scheduler.getInstance().run();
    assertCommandState(m_onFalse, 1, 2, 2, 0, 0);
    assertCommandState(m_onTrue, 0, 0, 0, 0, 0);
    assertConditionalCommandState(m_command, 1, 3, 3, 0, 0);
    m_onFalse.setHasFinished(true);
    Scheduler.getInstance().run();
    assertCommandState(m_onFalse, 1, 3, 3, 1, 0);
    assertCommandState(m_onTrue, 0, 0, 0, 0, 0);
    assertConditionalCommandState(m_command, 1, 4, 4, 0, 0);
    Scheduler.getInstance().run();
    assertCommandState(m_onFalse, 1, 3, 3, 1, 0);
    assertCommandState(m_onTrue, 0, 0, 0, 0, 0);
    assertConditionalCommandState(m_command, 1, 5, 5, 1, 0);

    assertTrue(m_onFalse.getInitializeCount() > 0, "Did not initialize the false command");
    assertSame(m_onTrue.getInitializeCount(), 0, "Initialized the true command");
  }

  @Test
  void cancelSubCommandTest() {
    m_command.setCondition(true);

    Scheduler.getInstance().add(m_command);
    assertCommandState(m_onTrue, 0, 0, 0, 0, 0);
    assertCommandState(m_onFalse, 0, 0, 0, 0, 0);
    assertConditionalCommandState(m_command, 0, 0, 0, 0, 0);
    Scheduler.getInstance().run(); // init command and select m_onTrue
    assertCommandState(m_onTrue, 0, 0, 0, 0, 0);
    assertCommandState(m_onFalse, 0, 0, 0, 0, 0);
    assertConditionalCommandState(m_command, 0, 0, 0, 0, 0);
    Scheduler.getInstance().run(); // init m_onTrue
    assertCommandState(m_onTrue, 0, 0, 0, 0, 0);
    assertCommandState(m_onFalse, 0, 0, 0, 0, 0);
    assertConditionalCommandState(m_command, 1, 1, 1, 0, 0);
    Scheduler.getInstance().run();
    assertCommandState(m_onTrue, 1, 1, 1, 0, 0);
    assertCommandState(m_onFalse, 0, 0, 0, 0, 0);
    assertConditionalCommandState(m_command, 1, 2, 2, 0, 0);
    Scheduler.getInstance().run();
    assertCommandState(m_onTrue, 1, 2, 2, 0, 0);
    assertCommandState(m_onFalse, 0, 0, 0, 0, 0);
    assertConditionalCommandState(m_command, 1, 3, 3, 0, 0);
    m_onTrue.cancel();
    Scheduler.getInstance().run();
    assertCommandState(m_onTrue, 1, 2, 2, 0, 1);
    assertCommandState(m_onFalse, 0, 0, 0, 0, 0);
    assertConditionalCommandState(m_command, 1, 4, 4, 0, 0);
    Scheduler.getInstance().run();
    assertCommandState(m_onTrue, 1, 2, 2, 0, 1);
    assertCommandState(m_onFalse, 0, 0, 0, 0, 0);
    assertConditionalCommandState(m_command, 1, 5, 5, 1, 0);
    Scheduler.getInstance().run();
    assertCommandState(m_onTrue, 1, 2, 2, 0, 1);
    assertCommandState(m_onFalse, 0, 0, 0, 0, 0);
    assertConditionalCommandState(m_command, 1, 5, 5, 1, 0);
  }

  @Test
  void cancelRequiresTest() {
    m_command.setCondition(true);

    Scheduler.getInstance().add(m_command);
    assertCommandState(m_onTrue, 0, 0, 0, 0, 0);
    assertCommandState(m_onFalse, 0, 0, 0, 0, 0);
    assertConditionalCommandState(m_command, 0, 0, 0, 0, 0);
    Scheduler.getInstance().run(); // init command and select m_onTrue
    assertCommandState(m_onTrue, 0, 0, 0, 0, 0);
    assertCommandState(m_onFalse, 0, 0, 0, 0, 0);
    assertConditionalCommandState(m_command, 0, 0, 0, 0, 0);
    Scheduler.getInstance().run(); // init m_onTrue
    assertCommandState(m_onTrue, 0, 0, 0, 0, 0);
    assertCommandState(m_onFalse, 0, 0, 0, 0, 0);
    assertConditionalCommandState(m_command, 1, 1, 1, 0, 0);
    Scheduler.getInstance().run();
    assertCommandState(m_onTrue, 1, 1, 1, 0, 0);
    assertCommandState(m_onFalse, 0, 0, 0, 0, 0);
    assertConditionalCommandState(m_command, 1, 2, 2, 0, 0);
    Scheduler.getInstance().run();
    assertCommandState(m_onTrue, 1, 2, 2, 0, 0);
    assertCommandState(m_onFalse, 0, 0, 0, 0, 0);
    assertConditionalCommandState(m_command, 1, 3, 3, 0, 0);
    m_onFalse.start();
    Scheduler.getInstance().run();
    assertCommandState(m_onTrue, 1, 3, 3, 0, 0);
    assertCommandState(m_onFalse, 0, 0, 0, 0, 0);
    assertConditionalCommandState(m_command, 1, 4, 4, 0, 1);
    Scheduler.getInstance().run();
    assertCommandState(m_onTrue, 1, 3, 3, 0, 1);
    assertCommandState(m_onFalse, 1, 1, 1, 0, 0);
    assertConditionalCommandState(m_command, 1, 4, 4, 0, 1);
  }

  @Test
  void cancelCondCommandTest() {
    m_command.setCondition(true);

    Scheduler.getInstance().add(m_command);
    assertCommandState(m_onTrue, 0, 0, 0, 0, 0);
    assertCommandState(m_onFalse, 0, 0, 0, 0, 0);
    assertConditionalCommandState(m_command, 0, 0, 0, 0, 0);
    Scheduler.getInstance().run(); // init command and select m_onTrue
    assertCommandState(m_onTrue, 0, 0, 0, 0, 0);
    assertCommandState(m_onFalse, 0, 0, 0, 0, 0);
    assertConditionalCommandState(m_command, 0, 0, 0, 0, 0);
    Scheduler.getInstance().run(); // init m_onTrue
    assertCommandState(m_onTrue, 0, 0, 0, 0, 0);
    assertCommandState(m_onFalse, 0, 0, 0, 0, 0);
    assertConditionalCommandState(m_command, 1, 1, 1, 0, 0);
    Scheduler.getInstance().run();
    assertCommandState(m_onTrue, 1, 1, 1, 0, 0);
    assertCommandState(m_onFalse, 0, 0, 0, 0, 0);
    assertConditionalCommandState(m_command, 1, 2, 2, 0, 0);
    Scheduler.getInstance().run();
    assertCommandState(m_onTrue, 1, 2, 2, 0, 0);
    assertCommandState(m_onFalse, 0, 0, 0, 0, 0);
    assertConditionalCommandState(m_command, 1, 3, 3, 0, 0);
    m_command.cancel();
    Scheduler.getInstance().run();
    assertCommandState(m_onTrue, 1, 2, 2, 0, 1);
    assertCommandState(m_onFalse, 0, 0, 0, 0, 0);
    assertConditionalCommandState(m_command, 1, 3, 3, 0, 1);
    Scheduler.getInstance().run();
    assertCommandState(m_onTrue, 1, 2, 2, 0, 1);
    assertCommandState(m_onFalse, 0, 0, 0, 0, 0);
    assertConditionalCommandState(m_command, 1, 3, 3, 0, 1);
  }

  @Test
  void onTrueTwiceTest() {
    m_command.setCondition(true);

    Scheduler.getInstance().add(m_command);
    assertCommandState(m_onTrue, 0, 0, 0, 0, 0);
    assertCommandState(m_onFalse, 0, 0, 0, 0, 0);
    assertConditionalCommandState(m_command, 0, 0, 0, 0, 0);
    Scheduler.getInstance().run(); // init command and select m_onTrue
    assertCommandState(m_onTrue, 0, 0, 0, 0, 0);
    assertCommandState(m_onFalse, 0, 0, 0, 0, 0);
    assertConditionalCommandState(m_command, 0, 0, 0, 0, 0);
    Scheduler.getInstance().run(); // init m_onTrue
    assertCommandState(m_onTrue, 0, 0, 0, 0, 0);
    assertCommandState(m_onFalse, 0, 0, 0, 0, 0);
    assertConditionalCommandState(m_command, 1, 1, 1, 0, 0);
    Scheduler.getInstance().run();
    assertCommandState(m_onTrue, 1, 1, 1, 0, 0);
    assertCommandState(m_onFalse, 0, 0, 0, 0, 0);
    assertConditionalCommandState(m_command, 1, 2, 2, 0, 0);
    Scheduler.getInstance().run();
    assertCommandState(m_onTrue, 1, 2, 2, 0, 0);
    assertCommandState(m_onFalse, 0, 0, 0, 0, 0);
    assertConditionalCommandState(m_command, 1, 3, 3, 0, 0);
    m_onTrue.setHasFinished(true);
    Scheduler.getInstance().run();
    assertCommandState(m_onTrue, 1, 3, 3, 1, 0);
    assertCommandState(m_onFalse, 0, 0, 0, 0, 0);
    assertConditionalCommandState(m_command, 1, 4, 4, 0, 0);
    Scheduler.getInstance().run();
    assertCommandState(m_onTrue, 1, 3, 3, 1, 0);
    assertCommandState(m_onFalse, 0, 0, 0, 0, 0);
    assertConditionalCommandState(m_command, 1, 5, 5, 1, 0);

    m_onTrue.resetCounters();
    m_command.resetCounters();
    m_command.setCondition(true);
    Scheduler.getInstance().add(m_command);
    assertCommandState(m_onTrue, 0, 0, 0, 0, 0);
    assertCommandState(m_onFalse, 0, 0, 0, 0, 0);
    assertConditionalCommandState(m_command, 0, 0, 0, 0, 0);
    Scheduler.getInstance().run(); // init command and select m_onTrue
    assertCommandState(m_onTrue, 0, 0, 0, 0, 0);
    assertCommandState(m_onFalse, 0, 0, 0, 0, 0);
    assertConditionalCommandState(m_command, 0, 0, 0, 0, 0);
    Scheduler.getInstance().run(); // init m_onTrue
    assertCommandState(m_onTrue, 0, 0, 0, 0, 0);
    assertCommandState(m_onFalse, 0, 0, 0, 0, 0);
    assertConditionalCommandState(m_command, 1, 1, 1, 0, 0);
    Scheduler.getInstance().run();
    assertCommandState(m_onTrue, 1, 1, 1, 0, 0);
    assertCommandState(m_onFalse, 0, 0, 0, 0, 0);
    assertConditionalCommandState(m_command, 1, 2, 2, 0, 0);
    Scheduler.getInstance().run();
    assertCommandState(m_onTrue, 1, 2, 2, 0, 0);
    assertCommandState(m_onFalse, 0, 0, 0, 0, 0);
    assertConditionalCommandState(m_command, 1, 3, 3, 0, 0);
    m_onTrue.setHasFinished(true);
    Scheduler.getInstance().run();
    assertCommandState(m_onTrue, 1, 3, 3, 1, 0);
    assertCommandState(m_onFalse, 0, 0, 0, 0, 0);
    assertConditionalCommandState(m_command, 1, 4, 4, 0, 0);
    Scheduler.getInstance().run();
    assertCommandState(m_onTrue, 1, 3, 3, 1, 0);
    assertCommandState(m_onFalse, 0, 0, 0, 0, 0);
    assertConditionalCommandState(m_command, 1, 5, 5, 1, 0);
  }

  @Test
  void onTrueInstantTest() {
    m_command.setCondition(true);
    m_onTrue.setHasFinished(true);

    Scheduler.getInstance().add(m_command);
    assertCommandState(m_onTrue, 0, 0, 0, 0, 0);
    assertCommandState(m_onFalse, 0, 0, 0, 0, 0);
    assertConditionalCommandState(m_command, 0, 0, 0, 0, 0);
    Scheduler.getInstance().run(); // init command and select m_onTrue
    assertCommandState(m_onTrue, 0, 0, 0, 0, 0);
    assertCommandState(m_onFalse, 0, 0, 0, 0, 0);
    assertConditionalCommandState(m_command, 0, 0, 0, 0, 0);
    Scheduler.getInstance().run(); // init m_onTrue
    assertCommandState(m_onTrue, 0, 0, 0, 0, 0);
    assertCommandState(m_onFalse, 0, 0, 0, 0, 0);
    assertConditionalCommandState(m_command, 1, 1, 1, 0, 0);
    Scheduler.getInstance().run();
    assertCommandState(m_onTrue, 1, 1, 1, 1, 0);
    assertCommandState(m_onFalse, 0, 0, 0, 0, 0);
    assertConditionalCommandState(m_command, 1, 2, 2, 0, 0);
    Scheduler.getInstance().run();
    assertCommandState(m_onTrue, 1, 1, 1, 1, 0);
    assertCommandState(m_onFalse, 0, 0, 0, 0, 0);
    assertConditionalCommandState(m_command, 1, 3, 3, 1, 0);
    Scheduler.getInstance().run();
    assertCommandState(m_onTrue, 1, 1, 1, 1, 0);
    assertCommandState(m_onFalse, 0, 0, 0, 0, 0);
    assertConditionalCommandState(m_command, 1, 3, 3, 1, 0);
  }

  @Test
  void onFalseNullTest() {
    m_commandNull.setCondition(false);

    Scheduler.getInstance().add(m_commandNull);
    assertCommandState(m_onTrue, 0, 0, 0, 0, 0);
    assertConditionalCommandState(m_commandNull, 0, 0, 0, 0, 0);
    Scheduler.getInstance().run(); // init command and select m_onFalse
    assertCommandState(m_onTrue, 0, 0, 0, 0, 0);
    assertConditionalCommandState(m_commandNull, 0, 0, 0, 0, 0);
    Scheduler.getInstance().run(); // init m_onFalse
    assertCommandState(m_onTrue, 0, 0, 0, 0, 0);
    assertConditionalCommandState(m_commandNull, 1, 1, 1, 1, 0);
    Scheduler.getInstance().run();
    assertCommandState(m_onTrue, 0, 0, 0, 0, 0);
    assertConditionalCommandState(m_commandNull, 1, 1, 1, 1, 0);
  }
}
