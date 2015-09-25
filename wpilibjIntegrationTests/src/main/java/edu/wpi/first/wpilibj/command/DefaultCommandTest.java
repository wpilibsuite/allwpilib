/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008-2014. All Rights Reserved. */
/* Open Source Software - may be modified and shared by FRC teams. The code */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project. */
/*----------------------------------------------------------------------------*/
package edu.wpi.first.wpilibj.command;

import static org.junit.Assert.*;

import java.util.logging.Logger;

import org.junit.After;
import org.junit.AfterClass;
import org.junit.Before;
import org.junit.BeforeClass;
import org.junit.Test;

import edu.wpi.first.wpilibj.mocks.MockCommand;

/**
 * @author jonathanleitschuh
 *
 */
public class DefaultCommandTest extends AbstractCommandTest {
  private static final Logger logger = Logger.getLogger(DefaultCommandTest.class.getName());

  protected Logger getClassLogger() {
    return logger;
  }

  /**
   * @throws java.lang.Exception
   */
  @BeforeClass
  public static void setUpBeforeClass() throws Exception {}

  /**
   * @throws java.lang.Exception
   */
  @AfterClass
  public static void tearDownAfterClass() throws Exception {}

  /**
   * @throws java.lang.Exception
   */
  @Before
  public void setUp() throws Exception {}

  /**
   * @throws java.lang.Exception
   */
  @After
  public void tearDown() throws Exception {}

  /**
   * Testing of default commands where the interrupting command ends itself
   */
  @Test
  public void testDefaultCommandWhereTheInteruptingCommandEndsItself() {
    final ASubsystem subsystem = new ASubsystem();


    MockCommand defaultCommand = new MockCommand() {
      {
        requires(subsystem);
      }
    };

    MockCommand anotherCommand = new MockCommand() {
      {
        requires(subsystem);
      }
    };
    assertCommandState(defaultCommand, 0, 0, 0, 0, 0);
    subsystem.init(defaultCommand);

    assertCommandState(defaultCommand, 0, 0, 0, 0, 0);
    Scheduler.getInstance().run();
    assertCommandState(defaultCommand, 0, 0, 0, 0, 0);
    Scheduler.getInstance().run();
    assertCommandState(defaultCommand, 1, 1, 1, 0, 0);
    Scheduler.getInstance().run();
    assertCommandState(defaultCommand, 1, 2, 2, 0, 0);

    anotherCommand.start();
    assertCommandState(defaultCommand, 1, 2, 2, 0, 0);
    assertCommandState(anotherCommand, 0, 0, 0, 0, 0);
    Scheduler.getInstance().run();
    assertCommandState(defaultCommand, 1, 3, 3, 0, 1);
    assertCommandState(anotherCommand, 0, 0, 0, 0, 0);
    Scheduler.getInstance().run();
    assertCommandState(defaultCommand, 1, 3, 3, 0, 1);
    assertCommandState(anotherCommand, 1, 1, 1, 0, 0);
    Scheduler.getInstance().run();
    assertCommandState(defaultCommand, 1, 3, 3, 0, 1);
    assertCommandState(anotherCommand, 1, 2, 2, 0, 0);
    anotherCommand.setHasFinished(true);
    assertCommandState(defaultCommand, 1, 3, 3, 0, 1);
    assertCommandState(anotherCommand, 1, 2, 2, 0, 0);
    Scheduler.getInstance().run();
    assertCommandState(defaultCommand, 1, 3, 3, 0, 1);
    assertCommandState(anotherCommand, 1, 3, 3, 1, 0);
    Scheduler.getInstance().run();
    assertCommandState(defaultCommand, 2, 4, 4, 0, 1);
    assertCommandState(anotherCommand, 1, 3, 3, 1, 0);
    Scheduler.getInstance().run();
    assertCommandState(defaultCommand, 2, 5, 5, 0, 1);
    assertCommandState(anotherCommand, 1, 3, 3, 1, 0);
  }


  /**
   * Testing of default commands where the interrupting command is canceled
   */
  @Test
  public void testDefaultCommandsInterruptingCommandCanceled() {
    final ASubsystem subsystem = new ASubsystem();


    MockCommand defaultCommand = new MockCommand() {
      {
        requires(subsystem);
      }
    };

    MockCommand anotherCommand = new MockCommand() {
      {
        requires(subsystem);
      }
    };
    assertCommandState(defaultCommand, 0, 0, 0, 0, 0);
    subsystem.init(defaultCommand);
    subsystem.initDefaultCommand();
    assertCommandState(defaultCommand, 0, 0, 0, 0, 0);
    Scheduler.getInstance().run();
    assertCommandState(defaultCommand, 0, 0, 0, 0, 0);
    Scheduler.getInstance().run();
    assertCommandState(defaultCommand, 1, 1, 1, 0, 0);
    Scheduler.getInstance().run();
    assertCommandState(defaultCommand, 1, 2, 2, 0, 0);

    anotherCommand.start();
    assertCommandState(defaultCommand, 1, 2, 2, 0, 0);
    assertCommandState(anotherCommand, 0, 0, 0, 0, 0);
    Scheduler.getInstance().run();
    assertCommandState(defaultCommand, 1, 3, 3, 0, 1);
    assertCommandState(anotherCommand, 0, 0, 0, 0, 0);
    Scheduler.getInstance().run();
    assertCommandState(defaultCommand, 1, 3, 3, 0, 1);
    assertCommandState(anotherCommand, 1, 1, 1, 0, 0);
    Scheduler.getInstance().run();
    assertCommandState(defaultCommand, 1, 3, 3, 0, 1);
    assertCommandState(anotherCommand, 1, 2, 2, 0, 0);
    anotherCommand.cancel();
    assertCommandState(defaultCommand, 1, 3, 3, 0, 1);
    assertCommandState(anotherCommand, 1, 2, 2, 0, 0);
    Scheduler.getInstance().run();
    assertCommandState(defaultCommand, 1, 3, 3, 0, 1);
    assertCommandState(anotherCommand, 1, 2, 2, 0, 1);
    Scheduler.getInstance().run();
    assertCommandState(defaultCommand, 2, 4, 4, 0, 1);
    assertCommandState(anotherCommand, 1, 2, 2, 0, 1);
    Scheduler.getInstance().run();
    assertCommandState(defaultCommand, 2, 5, 5, 0, 1);
    assertCommandState(anotherCommand, 1, 2, 2, 0, 1);
  }

}
