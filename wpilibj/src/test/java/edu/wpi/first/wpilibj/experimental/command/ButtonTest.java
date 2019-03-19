package edu.wpi.first.wpilibj.experimental.command;

import org.junit.jupiter.api.Test;

import edu.wpi.first.wpilibj.experimental.buttons.InternalButton;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.mockito.Mockito.never;
import static org.mockito.Mockito.times;
import static org.mockito.Mockito.verify;
import static org.mockito.Mockito.when;


class ButtonTest extends CommandTestBase {
  @Test
  void whenPressedTest() {
    CommandScheduler scheduler = CommandScheduler.getInstance();
    MockCommandHolder command1Holder = new MockCommandHolder(true, new Subsystem[0]);
    Command command1 = command1Holder.getMock();

    InternalButton button = new InternalButton();
    button.setPressed(false);
    button.whenPressed(command1);
    scheduler.run();
    verify(command1, never()).schedule(true);
    button.setPressed(true);
    scheduler.run();
    scheduler.run();
    verify(command1).schedule(true);
  }

  @Test
  void whenReleasedTest() {
    CommandScheduler scheduler = CommandScheduler.getInstance();
    MockCommandHolder command1Holder = new MockCommandHolder(true, new Subsystem[0]);
    Command command1 = command1Holder.getMock();

    InternalButton button = new InternalButton();
    button.setPressed(true);
    button.whenReleased(command1);
    scheduler.run();
    verify(command1, never()).schedule(true);
    button.setPressed(false);
    scheduler.run();
    scheduler.run();
    verify(command1).schedule(true);
  }

  @Test
  void whileHeldTest() {
    CommandScheduler scheduler = CommandScheduler.getInstance();
    MockCommandHolder command1Holder = new MockCommandHolder(true, new Subsystem[0]);
    Command command1 = command1Holder.getMock();

    InternalButton button = new InternalButton();
    button.setPressed(false);
    button.whileHeld(command1);
    scheduler.run();
    verify(command1, never()).schedule(true);
    button.setPressed(true);
    scheduler.run();
    scheduler.run();
    verify(command1, times(2)).schedule(true);
    button.setPressed(false);
    scheduler.run();
    verify(command1).cancel();
  }

  @Test
  void whenHeldTest() {
    CommandScheduler scheduler = CommandScheduler.getInstance();
    MockCommandHolder command1Holder = new MockCommandHolder(true, new Subsystem[0]);
    Command command1 = command1Holder.getMock();

    InternalButton button = new InternalButton();
    button.setPressed(false);
    button.whenHeld(command1);
    scheduler.run();
    verify(command1, never()).schedule(true);
    button.setPressed(true);
    scheduler.run();
    scheduler.run();
    verify(command1).schedule(true);
    button.setPressed(false);
    scheduler.run();
    verify(command1).cancel();
  }

  @Test
  void toggleWhenPressedTest() {
    CommandScheduler scheduler = CommandScheduler.getInstance();
    MockCommandHolder command1Holder = new MockCommandHolder(true, new Subsystem[0]);
    Command command1 = command1Holder.getMock();

    InternalButton button = new InternalButton();
    button.setPressed(false);
    button.toggleWhenPressed(command1);
    scheduler.run();
    verify(command1, never()).schedule(true);
    button.setPressed(true);
    scheduler.run();
    when(command1.isScheduled()).thenReturn(true);
    scheduler.run();
    verify(command1).schedule(true);
    button.setPressed(false);
    scheduler.run();
    verify(command1, never()).cancel();
    button.setPressed(true);
    scheduler.run();
    verify(command1).cancel();
  }

  @Test
  void cancelWhenPressedTest() {
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

  @Test
  void runnableBindingTest() {

    InternalButton buttonWhenPressed = new InternalButton();
    InternalButton buttonWhileHeld = new InternalButton();
    InternalButton buttonWhenReleased = new InternalButton();

    buttonWhenPressed.setPressed(false);
    buttonWhileHeld.setPressed(true);
    buttonWhenReleased.setPressed(true);

    Counter counter = new Counter();

    buttonWhenPressed.whenPressed(counter::increment);
    buttonWhileHeld.whileHeld(counter::increment);
    buttonWhenReleased.whenReleased(counter::increment);

    CommandScheduler scheduler = CommandScheduler.getInstance();

    scheduler.run();
    buttonWhenPressed.setPressed(true);
    buttonWhenReleased.setPressed(false);
    scheduler.run();

    assertEquals(counter.m_counter, 4);
  }
}
