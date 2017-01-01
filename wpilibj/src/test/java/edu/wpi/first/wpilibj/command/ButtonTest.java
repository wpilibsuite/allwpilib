/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008-2017. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.command;

import org.junit.Before;
import org.junit.Test;

import edu.wpi.first.wpilibj.buttons.InternalButton;


/**
 * Test that covers the {@link edu.wpi.first.wpilibj.buttons.Button} with the {@link Command}
 * library.
 */
public class ButtonTest extends AbstractCommandTest {
  private InternalButton m_button1;
  private InternalButton m_button2;

  @Before
  public void setUp() throws Exception {
    m_button1 = new InternalButton();
    m_button2 = new InternalButton();
  }

  /**
   * Simple Button Test.
   */
  @Test
  public void test() {
    final MockCommand command1 = new MockCommand();
    final MockCommand command2 = new MockCommand();
    final MockCommand command3 = new MockCommand();
    final MockCommand command4 = new MockCommand();

    m_button1.whenPressed(command1);
    m_button1.whenReleased(command2);
    m_button1.whileHeld(command3);
    m_button2.whileHeld(command4);

    assertCommandState(command1, 0, 0, 0, 0, 0);
    assertCommandState(command2, 0, 0, 0, 0, 0);
    assertCommandState(command3, 0, 0, 0, 0, 0);
    assertCommandState(command4, 0, 0, 0, 0, 0);
    m_button1.setPressed(true);
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
    m_button2.setPressed(true);
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
    m_button1.setPressed(false);
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
    m_button2.setPressed(false);
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
