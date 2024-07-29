// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj2.command.button;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertFalse;
import static org.junit.jupiter.api.Assertions.assertTrue;
import static org.mockito.Mockito.never;
import static org.mockito.Mockito.verify;

import edu.wpi.first.wpilibj.simulation.SimHooks;
import edu.wpi.first.wpilibj2.command.Command;
import edu.wpi.first.wpilibj2.command.CommandScheduler;
import edu.wpi.first.wpilibj2.command.CommandTestBase;
import edu.wpi.first.wpilibj2.command.FunctionalCommand;
import edu.wpi.first.wpilibj2.command.RunCommand;
import edu.wpi.first.wpilibj2.command.StartEndCommand;
import edu.wpi.first.wpilibj2.command.WaitUntilCommand;
import java.util.concurrent.atomic.AtomicBoolean;
import java.util.concurrent.atomic.AtomicInteger;
import java.util.function.BooleanSupplier;
import org.junit.jupiter.api.Test;

class TriggerTest extends CommandTestBase {
  @Test
  void onTrueTest() {
    CommandScheduler scheduler = CommandScheduler.getInstance();
    AtomicBoolean finished = new AtomicBoolean(false);
    Command command1 = new WaitUntilCommand(finished::get);

    InternalButton button = new InternalButton();
    button.setPressed(false);
    button.onTrue(command1);
    scheduler.run();
    assertFalse(command1.isScheduled());
    button.setPressed(true);
    scheduler.run();
    assertTrue(command1.isScheduled());
    finished.set(true);
    scheduler.run();
    assertFalse(command1.isScheduled());
  }

  @Test
  void onFalseTest() {
    CommandScheduler scheduler = CommandScheduler.getInstance();
    AtomicBoolean finished = new AtomicBoolean(false);
    Command command1 = new WaitUntilCommand(finished::get);

    InternalButton button = new InternalButton();
    button.setPressed(true);
    button.onFalse(command1);
    scheduler.run();
    assertFalse(command1.isScheduled());
    button.setPressed(false);
    scheduler.run();
    assertTrue(command1.isScheduled());
    finished.set(true);
    scheduler.run();
    assertFalse(command1.isScheduled());
  }

  @Test
  void onChangeTest() {
    CommandScheduler scheduler = CommandScheduler.getInstance();
    AtomicBoolean finished = new AtomicBoolean(false);
    Command command1 = new WaitUntilCommand(finished::get);

    InternalButton button = new InternalButton();
    button.setPressed(true);
    button.onChange(command1);
    scheduler.run();
    assertFalse(command1.isScheduled());
    button.setPressed(false);
    scheduler.run();
    assertTrue(command1.isScheduled());
    finished.set(true);
    scheduler.run();
    assertFalse(command1.isScheduled());
    finished.set(false);
    button.setPressed(true);
    scheduler.run();
    assertTrue(command1.isScheduled());
    finished.set(true);
    scheduler.run();
    assertFalse(command1.isScheduled());
  }

  @Test
  void whileTrueRepeatedlyTest() {
    CommandScheduler scheduler = CommandScheduler.getInstance();
    AtomicInteger inits = new AtomicInteger(0);
    AtomicInteger counter = new AtomicInteger(0);
    // the repeatedly() here is the point!
    Command command1 =
        new FunctionalCommand(
                inits::incrementAndGet,
                () -> {},
                interrupted -> {},
                () -> counter.incrementAndGet() % 2 == 0)
            .repeatedly();

    InternalButton button = new InternalButton();
    button.setPressed(false);
    button.whileTrue(command1);
    scheduler.run();
    assertEquals(0, inits.get());
    button.setPressed(true);
    scheduler.run();
    assertEquals(1, inits.get());
    scheduler.run();
    assertEquals(1, inits.get());
    scheduler.run();
    assertEquals(2, inits.get());
    button.setPressed(false);
    scheduler.run();
    assertEquals(2, inits.get());
  }

  @Test
  void whileTrueLambdaRunTest() {
    CommandScheduler scheduler = CommandScheduler.getInstance();
    AtomicInteger counter = new AtomicInteger(0);
    // the repeatedly() here is the point!
    Command command1 = new RunCommand(counter::incrementAndGet);

    InternalButton button = new InternalButton();
    button.setPressed(false);
    button.whileTrue(command1);
    scheduler.run();
    assertEquals(0, counter.get());
    button.setPressed(true);
    scheduler.run();
    assertEquals(1, counter.get());
    scheduler.run();
    assertEquals(2, counter.get());
    button.setPressed(false);
    scheduler.run();
    assertEquals(2, counter.get());
  }

  @Test
  void whileTrueOnceTest() {
    CommandScheduler scheduler = CommandScheduler.getInstance();
    AtomicInteger startCounter = new AtomicInteger(0);
    AtomicInteger endCounter = new AtomicInteger(0);
    Command command1 =
        new StartEndCommand(startCounter::incrementAndGet, endCounter::incrementAndGet);

    InternalButton button = new InternalButton();
    button.setPressed(false);
    button.whileTrue(command1);
    scheduler.run();
    assertEquals(0, startCounter.get());
    assertEquals(0, endCounter.get());
    button.setPressed(true);
    scheduler.run();
    scheduler.run();
    assertEquals(1, startCounter.get());
    assertEquals(0, endCounter.get());
    button.setPressed(false);
    scheduler.run();
    assertEquals(1, startCounter.get());
    assertEquals(1, endCounter.get());
  }

  @Test
  void toggleOnTrueTest() {
    CommandScheduler scheduler = CommandScheduler.getInstance();
    AtomicInteger startCounter = new AtomicInteger(0);
    AtomicInteger endCounter = new AtomicInteger(0);
    Command command1 =
        new StartEndCommand(startCounter::incrementAndGet, endCounter::incrementAndGet);

    InternalButton button = new InternalButton();
    button.setPressed(false);
    button.toggleOnTrue(command1);
    scheduler.run();
    assertEquals(0, startCounter.get());
    assertEquals(0, endCounter.get());
    button.setPressed(true);
    scheduler.run();
    scheduler.run();
    assertEquals(1, startCounter.get());
    assertEquals(0, endCounter.get());
    button.setPressed(false);
    scheduler.run();
    assertEquals(1, startCounter.get());
    assertEquals(0, endCounter.get());
    button.setPressed(true);
    scheduler.run();
    assertEquals(1, startCounter.get());
    assertEquals(1, endCounter.get());
  }

  @Test
  void cancelWhenActiveTest() {
    CommandScheduler scheduler = CommandScheduler.getInstance();
    AtomicInteger startCounter = new AtomicInteger(0);
    AtomicInteger endCounter = new AtomicInteger(0);

    InternalButton button = new InternalButton();
    Command command1 =
        new StartEndCommand(startCounter::incrementAndGet, endCounter::incrementAndGet)
            .until(button);

    button.setPressed(false);
    command1.schedule();
    scheduler.run();
    assertEquals(1, startCounter.get());
    assertEquals(0, endCounter.get());
    button.setPressed(true);
    scheduler.run();
    assertEquals(1, startCounter.get());
    assertEquals(1, endCounter.get());
    scheduler.run();
    assertEquals(1, startCounter.get());
    assertEquals(1, endCounter.get());
  }

  @Test
  void triggerCompositionTest() {
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
  void triggerCompositionSupplierTest() {
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

    debounced.onTrue(command);

    button.setPressed(true);
    scheduler.run();
    verify(command, never()).schedule();

    SimHooks.stepTiming(0.3);

    button.setPressed(true);
    scheduler.run();
    verify(command).schedule();
  }

  @Test
  void booleanSupplierTest() {
    InternalButton button = new InternalButton();

    assertFalse(button.getAsBoolean());
    button.setPressed(true);
    assertTrue(button.getAsBoolean());
  }
}
