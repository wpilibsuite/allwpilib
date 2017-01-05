package edu.wpi.first.wpilibj.command;

import static org.junit.Assert.assertTrue;
import static org.junit.Assert.assertFalse;

import org.junit.Before;
import org.junit.Test;

import edu.wpi.first.wpilibj.command.MockCommand;
import edu.wpi.first.wpilibj.command.MockConditionalCommand;

public class ConditionalCommandTest extends AbstractCommandTest {
  MockConditionalCommand m_command;
  MockCommand m_onTrue;
  MockCommand m_onFalse;

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
    Scheduler.getInstance().run();// init command and select onTrue
    Scheduler.getInstance().run();// init onTrue
    Scheduler.getInstance().run();
    assertTrue("Did not initialize the true command", m_onTrue.getInitializeCount() > 0);
    assertTrue("Initialized the false command", m_onFalse.getInitializeCount() == 0);
    assertFalse("Conditional Command has not stopped", m_command.isRunning());
  }

  @Test
  public void testOnFalse() {
    m_command.setCondition(false);
    Scheduler.getInstance().add(m_command);
    Scheduler.getInstance().run();// init command and select onFalse
    Scheduler.getInstance().run();// init onFalse
    Scheduler.getInstance().run();
    assertTrue("Did not initialize the false command", m_onFalse.getInitializeCount() > 0);
    assertTrue("Initialized the true command", m_onTrue.getInitializeCount() == 0);
    assertFalse("Conditional Command has not stopped", m_command.isRunning());
  }
}
