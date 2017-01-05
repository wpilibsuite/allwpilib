/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2017. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.command;

import static org.junit.Assert.assertTrue;

import org.junit.Before;
import org.junit.Test;

public class ConditionalCommandTest extends AbstractCommandTest {
  MockConditionalCommand m_command;
  MockCommand m_onTrue;
  MockCommand m_onFalse;
  Boolean m_condition;

  @Before
  public void initCommands() {
    m_onTrue = new MockCommand();
    m_onFalse = new MockCommand();
    m_command = new MockConditionalCommand(m_onTrue, m_onFalse);
  }

  @Test
  public void testOnTrue() {
    m_command.setCondition(true);

    Scheduler.getInstance().add(m_command);
    assertCommandState(m_onTrue, 0, 0, 0, 0, 0);
    Scheduler.getInstance().run();  // init command and select m_onTrue
    assertCommandState(m_onTrue, 0, 0, 0, 0, 0);
    Scheduler.getInstance().run();  // init m_onTrue
    assertCommandState(m_onTrue, 0, 0, 0, 0, 0);
    Scheduler.getInstance().run();
    assertCommandState(m_onTrue, 1, 1, 2, 0, 0);
    Scheduler.getInstance().run();
    assertCommandState(m_onTrue, 1, 2, 4, 0, 0);

    assertTrue("Did not initialize the true command", m_onTrue.getInitializeCount() > 0);
    assertTrue("Initialized the false command", m_onFalse.getInitializeCount() == 0);
  }

  @Test
  public void testOnFalse() {
    m_command.setCondition(false);

    Scheduler.getInstance().add(m_command);
    assertCommandState(m_onFalse, 0, 0, 0, 0, 0);
    Scheduler.getInstance().run();  // init command and select m_onFalse
    assertCommandState(m_onFalse, 0, 0, 0, 0, 0);
    Scheduler.getInstance().run();  // init m_onFalse
    assertCommandState(m_onFalse, 0, 0, 0, 0, 0);
    Scheduler.getInstance().run();
    assertCommandState(m_onFalse, 1, 1, 2, 0, 0);
    Scheduler.getInstance().run();
    assertCommandState(m_onFalse, 1, 2, 4, 0, 0);

    assertTrue("Did not initialize the false command", m_onFalse.getInitializeCount() > 0);
    assertTrue("Initialized the true command", m_onTrue.getInitializeCount() == 0);
  }
}
