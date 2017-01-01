/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008-2017. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.command;

import org.junit.Before;

import edu.wpi.first.wpilibj.UnitTestUtility;

import static org.junit.Assert.assertEquals;
import static org.junit.Assert.fail;

/**
 * The basic test for all {@link Command} tests.
 */
public abstract class AbstractCommandTest {

  @Before
  public void commandSetup() {
    UnitTestUtility.setupMockBase();
    Scheduler.getInstance().removeAll();
    Scheduler.getInstance().enable();
  }

  public class ASubsystem extends Subsystem {
    Command m_command;

    protected void initDefaultCommand() {
      if (m_command != null) {
        setDefaultCommand(m_command);
      }
    }

    public void init(Command command) {
      m_command = command;
    }
  }


  protected void assertCommandState(MockCommand command, int initialize, int execute,
                                    int isFinished, int end, int interrupted) {
    assertEquals(initialize, command.getInitializeCount());
    assertEquals(execute, command.getExecuteCount());
    assertEquals(isFinished, command.getIsFinishedCount());
    assertEquals(end, command.getEndCount());
    assertEquals(interrupted, command.getInterruptedCount());
  }

  protected void sleep(int time) {
    try {
      Thread.sleep(time);
    } catch (InterruptedException ex) {
      fail("Sleep Interrupted!?!?!?!?");
    }
  }
}
