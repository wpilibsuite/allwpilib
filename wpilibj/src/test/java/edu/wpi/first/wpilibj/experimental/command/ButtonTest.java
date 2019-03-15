package edu.wpi.first.wpilibj.experimental.command;

import org.junit.jupiter.api.Test;

import edu.wpi.first.wpilibj.experimental.buttons.InternalButton;

import static org.mockito.Mockito.never;
import static org.mockito.Mockito.times;
import static org.mockito.Mockito.verify;
import static org.mockito.Mockito.when;


public class ButtonTest extends CommandTestBase {
  @Test
  public void whenPressedTest() {
    CommandScheduler scheduler = CommandScheduler.getInstance();
    MockCommandHolder command1Holder = new MockCommandHolder(true, new Subsystem[0]);
    Command command1 = command1Holder.getMock();

    InternalButton button = new InternalButton();
    button.setPressed(false);
    button.whenPressed(command1);
    scheduler.run();
    verify(command1, never()).start(true);
    button.setPressed(true);
    scheduler.run();
    scheduler.run();
    verify(command1).start(true);
  }

  @Test
  public void whenReleasedTest() {
    CommandScheduler scheduler = CommandScheduler.getInstance();
    MockCommandHolder command1Holder = new MockCommandHolder(true, new Subsystem[0]);
    Command command1 = command1Holder.getMock();

    InternalButton button = new InternalButton();
    button.setPressed(true);
    button.whenReleased(command1);
    scheduler.run();
    verify(command1, never()).start(true);
    button.setPressed(false);
    scheduler.run();
    scheduler.run();
    verify(command1).start(true);
  }

  @Test
  public void whileHeldTest() {
    CommandScheduler scheduler = CommandScheduler.getInstance();
    MockCommandHolder command1Holder = new MockCommandHolder(true, new Subsystem[0]);
    Command command1 = command1Holder.getMock();

    InternalButton button = new InternalButton();
    button.setPressed(false);
    button.whileHeld(command1);
    scheduler.run();
    verify(command1, never()).start(true);
    button.setPressed(true);
    scheduler.run();
    scheduler.run();
    verify(command1, times(2)).start(true);
    button.setPressed(false);
    scheduler.run();
    verify(command1).cancel();
  }

  @Test
  public void toggleWhenPressedTest() {
    CommandScheduler scheduler = CommandScheduler.getInstance();
    MockCommandHolder command1Holder = new MockCommandHolder(true, new Subsystem[0]);
    Command command1 = command1Holder.getMock();

    InternalButton button = new InternalButton();
    button.setPressed(false);
    button.toggleWhenPressed(command1);
    scheduler.run();
    verify(command1, never()).start(true);
    button.setPressed(true);
    scheduler.run();
    when(command1.isRunning()).thenReturn(true);
    scheduler.run();
    verify(command1).start(true);
    button.setPressed(false);
    scheduler.run();
    verify(command1, never()).cancel();
    button.setPressed(true);
    scheduler.run();
    verify(command1).cancel();
  }

  @Test
  public void cancelWhenPressedTest() {
    CommandScheduler scheduler = CommandScheduler.getInstance();
    MockCommandHolder command1Holder = new MockCommandHolder(true, new Subsystem[0]);
    Command command1 = command1Holder.getMock();

    InternalButton button = new InternalButton();
    button.setPressed(false);
    button.cancelWhenPressed(command1);
    scheduler.run();
    verify(command1, never()).cancel();
    button.setPressed(true);
    scheduler.run();
    scheduler.run();
    verify(command1).cancel();
  }
}
