/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.command;

import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertTrue;

import org.junit.Before;
//import org.junit.Ignore;
import org.junit.Test;

public class ConditionalCommandTest extends AbstractCommandTest {
  MockConditionalCommand m_command;
  MockCommand m_onTrue;
  MockCommand m_onFalse;
  MockSubsystem m_subsys;
  Boolean m_condition;

  @Before
  public void initCommands() {
    m_subsys = new MockSubsystem();
    m_onTrue = new MockCommand(m_subsys);
    m_onFalse = new MockCommand(m_subsys);
    m_command = new MockConditionalCommand(m_onTrue, m_onFalse);
  }

  protected void assertConditionalCommandState(MockConditionalCommand command, int initialize,
                                               int execute, int isFinished, int end,
                                               int interrupted) {
    assertEquals(initialize, command.getInitializeCount());
    assertEquals(execute, command.getExecuteCount());
    assertEquals(isFinished, command.getIsFinishedCount());
    assertEquals(end, command.getEndCount());
    assertEquals(interrupted, command.getInterruptedCount());
  }

  @Test
  //@Ignore
  public void testOnTrue() {
    m_command.setCondition(true);

    Scheduler.getInstance().add(m_command);
    assertCommandState(m_onTrue, 0, 0, 0, 0, 0);
    assertCommandState(m_onFalse, 0, 0, 0, 0, 0);
    assertConditionalCommandState(m_command, 0, 0, 0, 0, 0);
    Scheduler.getInstance().run();  // init command and select m_onTrue
    assertCommandState(m_onTrue, 0, 0, 0, 0, 0);
    assertCommandState(m_onFalse, 0, 0, 0, 0, 0);
    assertConditionalCommandState(m_command, 0, 0, 0, 0, 0);
    Scheduler.getInstance().run();  // init m_onTrue
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

    assertTrue("Did not initialize the true command", m_onTrue.getInitializeCount() > 0);
    assertTrue("Initialized the false command", m_onFalse.getInitializeCount() == 0);
  }

  @Test
  //@Ignore
  public void testOnFalse() {
    m_command.setCondition(false);

    Scheduler.getInstance().add(m_command);
    assertCommandState(m_onFalse, 0, 0, 0, 0, 0);
    assertCommandState(m_onTrue, 0, 0, 0, 0, 0);
    assertConditionalCommandState(m_command, 0, 0, 0, 0, 0);
    Scheduler.getInstance().run();  // init command and select m_onFalse
    assertCommandState(m_onFalse, 0, 0, 0, 0, 0);
    assertCommandState(m_onTrue, 0, 0, 0, 0, 0);
    assertConditionalCommandState(m_command, 0, 0, 0, 0, 0);
    Scheduler.getInstance().run();  // init m_onFalse
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

    assertTrue("Did not initialize the false command", m_onFalse.getInitializeCount() > 0);
    assertTrue("Initialized the true command", m_onTrue.getInitializeCount() == 0);
  }

  @Test
  //@Ignore
  //@Ignore("WPILIB conditional command bug, canceling inner command doesn't cancel conditional "
  //        + "command")
  public void testCancelSubCommand() {
    m_command.setCondition(true);

    Scheduler.getInstance().add(m_command);
    assertCommandState(m_onTrue, 0, 0, 0, 0, 0);
    assertCommandState(m_onFalse, 0, 0, 0, 0, 0);
    assertConditionalCommandState(m_command, 0, 0, 0, 0, 0);
    Scheduler.getInstance().run();  // init command and select m_onTrue
    assertCommandState(m_onTrue, 0, 0, 0, 0, 0);
    assertCommandState(m_onFalse, 0, 0, 0, 0, 0);
    assertConditionalCommandState(m_command, 0, 0, 0, 0, 0);
    Scheduler.getInstance().run();  // init m_onTrue
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
  //@Ignore
  public void testCancelRequires() {
    m_command.setCondition(true);

    Scheduler.getInstance().add(m_command);
    assertCommandState(m_onTrue, 0, 0, 0, 0, 0);
    assertCommandState(m_onFalse, 0, 0, 0, 0, 0);
    assertConditionalCommandState(m_command, 0, 0, 0, 0, 0);
    Scheduler.getInstance().run();  // init command and select m_onTrue
    assertCommandState(m_onTrue, 0, 0, 0, 0, 0);
    assertCommandState(m_onFalse, 0, 0, 0, 0, 0);
    assertConditionalCommandState(m_command, 0, 0, 0, 0, 0);
    Scheduler.getInstance().run();  // init m_onTrue
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
  //@Ignore
  public void testCancelCondCommand() {
    m_command.setCondition(true);

    Scheduler.getInstance().add(m_command);
    assertCommandState(m_onTrue, 0, 0, 0, 0, 0);
    assertCommandState(m_onFalse, 0, 0, 0, 0, 0);
    assertConditionalCommandState(m_command, 0, 0, 0, 0, 0);
    Scheduler.getInstance().run();  // init command and select m_onTrue
    assertCommandState(m_onTrue, 0, 0, 0, 0, 0);
    assertCommandState(m_onFalse, 0, 0, 0, 0, 0);
    assertConditionalCommandState(m_command, 0, 0, 0, 0, 0);
    Scheduler.getInstance().run();  // init m_onTrue
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
  //@Ignore
  public void testOnTrueTwice() {
    m_command.setCondition(true);

    Scheduler.getInstance().add(m_command);
    assertCommandState(m_onTrue, 0, 0, 0, 0, 0);
    assertCommandState(m_onFalse, 0, 0, 0, 0, 0);
    assertConditionalCommandState(m_command, 0, 0, 0, 0, 0);
    Scheduler.getInstance().run();  // init command and select m_onTrue
    assertCommandState(m_onTrue, 0, 0, 0, 0, 0);
    assertCommandState(m_onFalse, 0, 0, 0, 0, 0);
    assertConditionalCommandState(m_command, 0, 0, 0, 0, 0);
    Scheduler.getInstance().run();  // init m_onTrue
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

    m_onTrue.setHasFinished(false);
    Scheduler.getInstance().add(m_command);
    assertCommandState(m_onTrue, 1, 3, 3, 1, 0);
    assertCommandState(m_onFalse, 0, 0, 0, 0, 0);
    assertConditionalCommandState(m_command, 1, 5, 5, 1, 0);
    Scheduler.getInstance().run();  // init command and select m_onTrue
    assertCommandState(m_onTrue, 1, 3, 3, 1, 0);
    assertCommandState(m_onFalse, 0, 0, 0, 0, 0);
    assertConditionalCommandState(m_command, 1, 5, 5, 1, 0);
    Scheduler.getInstance().run();  // init m_onTrue
    assertCommandState(m_onTrue, 1, 3, 3, 1, 0);
    assertCommandState(m_onFalse, 0, 0, 0, 0, 0);
    assertConditionalCommandState(m_command, 2, 6, 6, 1, 0);
    Scheduler.getInstance().run();
    assertCommandState(m_onTrue, 2, 4, 4, 1, 0);
    assertCommandState(m_onFalse, 0, 0, 0, 0, 0);
    assertConditionalCommandState(m_command, 2, 7, 7, 1, 0);
    Scheduler.getInstance().run();
    assertCommandState(m_onTrue, 2, 5, 5, 1, 0);
    assertCommandState(m_onFalse, 0, 0, 0, 0, 0);
    assertConditionalCommandState(m_command, 2, 8, 8, 1, 0);
    m_onTrue.setHasFinished(true);
    Scheduler.getInstance().run();
    assertCommandState(m_onTrue, 2, 6, 6, 2, 0);
    assertCommandState(m_onFalse, 0, 0, 0, 0, 0);
    assertConditionalCommandState(m_command, 2, 9, 9, 1, 0);
    Scheduler.getInstance().run();
    assertCommandState(m_onTrue, 2, 6, 6, 2, 0);
    assertCommandState(m_onFalse, 0, 0, 0, 0, 0);
    assertConditionalCommandState(m_command, 2, 10, 10, 2, 0);
  }
}
