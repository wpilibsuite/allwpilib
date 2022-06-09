// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj2.command.button;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertFalse;
import static org.junit.jupiter.api.Assertions.assertTrue;
import static org.mockito.Mockito.never;
import static org.mockito.Mockito.times;
import static org.mockito.Mockito.verify;
import static org.mockito.Mockito.when;

import edu.wpi.first.wpilibj.simulation.SimHooks;
import edu.wpi.first.wpilibj2.command.Command;
import edu.wpi.first.wpilibj2.command.CommandScheduler;
import edu.wpi.first.wpilibj2.command.CommandTestBase;
import java.util.concurrent.atomic.AtomicInteger;
import java.util.function.BooleanSupplier;
import org.junit.jupiter.api.Test;

class ButtonTest extends CommandTestBase {
  @Test
  void whenPressedTest() {
    CommandScheduler scheduler = CommandScheduler.getInstance();
    MockCommandHolder command1Holder = new MockCommandHolder(true);
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
    MockCommandHolder command1Holder = new MockCommandHolder(true);
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
    MockCommandHolder command1Holder = new MockCommandHolder(true);
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
    MockCommandHolder command1Holder = new MockCommandHolder(true);
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
    MockCommandHolder command1Holder = new MockCommandHolder(true);
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
    MockCommandHolder command1Holder = new MockCommandHolder(true);
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

    AtomicInteger counter = new AtomicInteger(0);

    buttonWhenPressed.whenPressed(counter::incrementAndGet);
    buttonWhileHeld.whileHeld(counter::incrementAndGet);
    buttonWhenReleased.whenReleased(counter::incrementAndGet);

    CommandScheduler scheduler = CommandScheduler.getInstance();

    scheduler.run();
    buttonWhenPressed.setPressed(true);
    buttonWhenReleased.setPressed(false);
    scheduler.run();

    assertEquals(counter.get(), 4);
  }

  @Test
  void buttonCompositionTest() {
    InternalButton button1 = new InternalButton();
    InternalButton button2 = new InternalButton();

    button1.setPressed(true);
    button2.setPressed(false);

    assertFalse(button1.and(button2).getAsBoolean());
    assertTrue(button1.or(button2).getAsBoolean());
    assertFalse(button1.negate().getAsBoolean());
    assertTrue(button1.and(button2.negate()).getAsBoolean());
  }

  @Test
  void buttonCompositionSupplierTest() {
    InternalButton button1 = new InternalButton();
    BooleanSupplier booleanSupplier = () -> false;

    button1.setPressed(true);

    assertFalse(button1.and(booleanSupplier).getAsBoolean());
    assertTrue(button1.or(booleanSupplier).getAsBoolean());
  }

  @Test
  void debounceTest() {
    CommandScheduler scheduler = CommandScheduler.getInstance();
    MockCommandHolder commandHolder = new MockCommandHolder(true);
    Command command = commandHolder.getMock();

    InternalButton button = new InternalButton();
    Trigger debounced = button.debounce(0.1);

    debounced.whenActive(command);

    button.setPressed(true);
    scheduler.run();
    verify(command, never()).schedule(true);

    SimHooks.stepTiming(0.3);

    button.setPressed(true);
    scheduler.run();
    verify(command).schedule(true);
  }

  @Test
  void booleanSupplierTest() {
    InternalButton button = new InternalButton();

    assertFalse(button.getAsBoolean());
    button.setPressed(true);
    assertTrue(button.getAsBoolean());
  }
}
