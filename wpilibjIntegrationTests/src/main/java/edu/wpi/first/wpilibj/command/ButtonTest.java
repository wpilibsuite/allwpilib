/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008-2014. All Rights Reserved. */
/* Open Source Software - may be modified and shared by FRC teams. The code */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project. */
/*----------------------------------------------------------------------------*/
package edu.wpi.first.wpilibj.command;

import java.util.logging.Logger;

import org.junit.After;
import org.junit.AfterClass;
import org.junit.Before;
import org.junit.BeforeClass;
import org.junit.Test;

import edu.wpi.first.wpilibj.buttons.InternalButton;
import edu.wpi.first.wpilibj.mocks.MockCommand;


/**
 * @author Mitchell
 * @author jonathanleitschuh
 *
 */
public class ButtonTest extends AbstractCommandTest {
  private static final Logger logger = Logger.getLogger(ButtonTest.class.getName());

  private InternalButton button1;
  private InternalButton button2;

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
  public void setUp() throws Exception {
    button1 = new InternalButton();
    button2 = new InternalButton();
  }

  /**
   * @throws java.lang.Exception
   */
  @After
  public void tearDown() throws Exception {}

  /**
   * Simple Button Test
   */
  @Test
  public void test() {
    MockCommand command1 = new MockCommand();
    MockCommand command2 = new MockCommand();
    MockCommand command3 = new MockCommand();
    MockCommand command4 = new MockCommand();

    button1.whenPressed(command1);
    button1.whenReleased(command2);
    button1.whileHeld(command3);
    button2.whileHeld(command4);

    assertCommandState(command1, 0, 0, 0, 0, 0);
    assertCommandState(command2, 0, 0, 0, 0, 0);
    assertCommandState(command3, 0, 0, 0, 0, 0);
    assertCommandState(command4, 0, 0, 0, 0, 0);
    button1.setPressed(true);
    assertCommandState(command1, 0, 0, 0, 0, 0);
    assertCommandState(command2, 0, 0, 0, 0, 0);
    assertCommandState(command3, 0, 0, 0, 0, 0);
    assertCommandState(command4, 0, 0, 0, 0, 0);
    Scheduler.getInstance().run();
    assertCommandState(command1, 0, 0, 0, 0, 0);
    assertCommandState(command2, 0, 0, 0, 0, 0);
    assertCommandState(command3, 0, 0, 0, 0, 0);
    assertCommandState(command4, 0, 0, 0, 0, 0);
    Scheduler.getInstance().run();
    assertCommandState(command1, 1, 1, 1, 0, 0);
    assertCommandState(command2, 0, 0, 0, 0, 0);
    assertCommandState(command3, 1, 1, 1, 0, 0);
    assertCommandState(command4, 0, 0, 0, 0, 0);
    Scheduler.getInstance().run();
    assertCommandState(command1, 1, 2, 2, 0, 0);
    assertCommandState(command2, 0, 0, 0, 0, 0);
    assertCommandState(command3, 1, 2, 2, 0, 0);
    assertCommandState(command4, 0, 0, 0, 0, 0);
    button2.setPressed(true);
    Scheduler.getInstance().run();
    assertCommandState(command1, 1, 3, 3, 0, 0);
    assertCommandState(command2, 0, 0, 0, 0, 0);
    assertCommandState(command3, 1, 3, 3, 0, 0);
    assertCommandState(command4, 0, 0, 0, 0, 0);
    Scheduler.getInstance().run();
    assertCommandState(command1, 1, 4, 4, 0, 0);
    assertCommandState(command2, 0, 0, 0, 0, 0);
    assertCommandState(command3, 1, 4, 4, 0, 0);
    assertCommandState(command4, 1, 1, 1, 0, 0);
    button1.setPressed(false);
    Scheduler.getInstance().run();
    assertCommandState(command1, 1, 5, 5, 0, 0);
    assertCommandState(command2, 0, 0, 0, 0, 0);
    assertCommandState(command3, 1, 4, 4, 0, 1);
    assertCommandState(command4, 1, 2, 2, 0, 0);
    Scheduler.getInstance().run();
    assertCommandState(command1, 1, 6, 6, 0, 0);
    assertCommandState(command2, 1, 1, 1, 0, 0);
    assertCommandState(command3, 1, 4, 4, 0, 1);
    assertCommandState(command4, 1, 3, 3, 0, 0);
    button2.setPressed(false);
    Scheduler.getInstance().run();
    assertCommandState(command1, 1, 7, 7, 0, 0);
    assertCommandState(command2, 1, 2, 2, 0, 0);
    assertCommandState(command3, 1, 4, 4, 0, 1);
    assertCommandState(command4, 1, 3, 3, 0, 1);
    command1.cancel();
    Scheduler.getInstance().run();
    assertCommandState(command1, 1, 7, 7, 0, 1);
    assertCommandState(command2, 1, 3, 3, 0, 0);
    assertCommandState(command3, 1, 4, 4, 0, 1);
    assertCommandState(command4, 1, 3, 3, 0, 1);
    command2.setHasFinished(true);
    Scheduler.getInstance().run();
    assertCommandState(command1, 1, 7, 7, 0, 1);
    assertCommandState(command2, 1, 4, 4, 1, 0);
    assertCommandState(command3, 1, 4, 4, 0, 1);
    assertCommandState(command4, 1, 3, 3, 0, 1);
    Scheduler.getInstance().run();
    assertCommandState(command1, 1, 7, 7, 0, 1);
    assertCommandState(command2, 1, 4, 4, 1, 0);
    assertCommandState(command3, 1, 4, 4, 0, 1);
    assertCommandState(command4, 1, 3, 3, 0, 1);
  }

}
