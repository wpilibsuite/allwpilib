// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.command3;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertFalse;
import static org.junit.jupiter.api.Assertions.assertThrows;
import static org.junit.jupiter.api.Assertions.assertTrue;

import java.util.ArrayList;
import org.junit.jupiter.api.Test;

class SchedulerErrorHandlingTests extends CommandTestBase {
  @Test
  void errorDetection() {
    var mechanism = new Mechanism("X", m_scheduler);

    var command =
        mechanism
            .run(
                coroutine -> {
                  throw new RuntimeException("The exception");
                })
            .named("Bad Behavior");

    new Trigger(m_scheduler, () -> true).onTrue(command);

    var e = assertThrows(RuntimeException.class, m_scheduler::run);
    assertEquals("The exception", e.getMessage());

    assertEquals(
        "org.wpilib.command3.SchedulerErrorHandlingTests", e.getStackTrace()[0].getClassName());
    assertEquals("lambda$errorDetection$0", e.getStackTrace()[0].getMethodName());

    assertEquals("=== Command Binding Trace ===", e.getStackTrace()[2].getClassName());

    assertEquals(getClass().getName(), e.getStackTrace()[3].getClassName());
    assertEquals("errorDetection", e.getStackTrace()[3].getMethodName());
  }

  @Test
  void nestedErrorDetection() {
    var command =
        Command.noRequirements()
            .executing(
                co -> {
                  co.await(
                      Command.noRequirements()
                          .executing(
                              c2 -> {
                                new Trigger(m_scheduler, () -> true)
                                    .onTrue(
                                        Command.noRequirements()
                                            .executing(
                                                c3 -> {
                                                  // Throws IndexOutOfBoundsException
                                                  var unused = new ArrayList<>(0).get(-1);
                                                })
                                            .named("Throws IndexOutOfBounds"));
                                c2.park();
                              })
                          .named("Schedules With Trigger"));
                })
            .named("Schedules Directly");

    m_scheduler.schedule(command);

    // The first run sets up the trigger, but does not fire
    // The second run will fire the trigger and cause the inner command to run and throw
    m_scheduler.run();

    var e = assertThrows(IndexOutOfBoundsException.class, m_scheduler::run);
    StackTraceElement[] stackTrace = e.getStackTrace();

    assertEquals("Index -1 out of bounds for length 0", e.getMessage());
    int nestedIndex = 0;
    for (; nestedIndex < stackTrace.length; nestedIndex++) {
      if (stackTrace[nestedIndex].getClassName().equals(getClass().getName())) {
        break;
      }
    }

    // user code trace for the scheduler run invocation (to `scheduler.run()` in the try block)
    assertEquals("lambda$nestedErrorDetection$3", stackTrace[nestedIndex].getMethodName());
    assertEquals("assertThrows", stackTrace[nestedIndex + 1].getMethodName());

    // user code trace for where the command was scheduled (the `.onTrue()` line)
    assertEquals("=== Command Binding Trace ===", stackTrace[nestedIndex + 2].getClassName());
    assertEquals("lambda$nestedErrorDetection$4", stackTrace[nestedIndex + 3].getMethodName());
    assertEquals("lambda$nestedErrorDetection$5", stackTrace[nestedIndex + 4].getMethodName());
    assertEquals("nestedErrorDetection", stackTrace[nestedIndex + 5].getMethodName());
  }

  @Test
  void commandEncounteringErrorCancelsChildren() {
    var child = Command.noRequirements().executing(Coroutine::park).named("Child 1");
    var command =
        Command.noRequirements()
            .executing(
                co -> {
                  co.fork(child);
                  throw new RuntimeException("The exception");
                })
            .named("Bad Behavior");

    m_scheduler.schedule(command);
    assertThrows(RuntimeException.class, m_scheduler::run);
    assertFalse(
        m_scheduler.isScheduledOrRunning(command),
        "Command should have been removed from the scheduler");
    assertFalse(
        m_scheduler.isScheduledOrRunning(child),
        "Child should have been removed from the scheduler");
  }

  @Test
  void childCommandEncounteringErrorCancelsParent() {
    var child =
        Command.noRequirements()
            .executing(
                co -> {
                  throw new RuntimeException("The exception"); // note: bubbles up to the parent
                })
            .named("Child 1");
    var command =
        Command.noRequirements()
            .executing(
                co -> {
                  co.await(child);
                  co.park(); // pretend other things would happen after the child
                })
            .named("Parent");

    m_scheduler.schedule(command);
    assertThrows(RuntimeException.class, m_scheduler::run);
    assertFalse(
        m_scheduler.isRunning(command),
        "Parent command should have been removed from the scheduler");
    assertFalse(m_scheduler.isRunning(child), "Child should have been removed from the scheduler");
  }

  @Test
  @SuppressWarnings("PMD.CompareObjectsWithEquals")
  void childCommandEncounteringErrorAfterRemountCancelsParent() {
    var child =
        Command.noRequirements()
            .executing(
                co -> {
                  co.yield();
                  throw new RuntimeException("The exception"); // does not bubble up to the parent
                })
            .named("Child 1");
    var command =
        Command.noRequirements()
            .executing(
                co -> {
                  co.await(child);
                  co.park(); // pretend other things would happen after the child
                })
            .named("Parent");

    m_scheduler.schedule(command);

    // first run schedules the child and adds it to the running set
    m_scheduler.run();

    // second run encounters the error in the child
    final var error = assertThrows(RuntimeException.class, m_scheduler::run);
    assertFalse(
        m_scheduler.isRunning(command),
        "Parent command should have been removed from the scheduler");
    assertFalse(m_scheduler.isRunning(child), "Child should have been removed from the scheduler");

    // Full event history
    assertEquals(9, m_events.size());
    assertTrue(
        m_events.get(0) instanceof SchedulerEvent.Scheduled s && s.command() == command,
        "First event should be parent scheduled");
    assertTrue(
        m_events.get(1) instanceof SchedulerEvent.Mounted m && m.command() == command,
        "Second event should be parent mounted");
    assertTrue(
        m_events.get(2) instanceof SchedulerEvent.Scheduled s && s.command() == child,
        "Third event should be child scheduled");
    assertTrue(
        m_events.get(3) instanceof SchedulerEvent.Mounted m && m.command() == child,
        "Fourth event should be child mounted");
    assertTrue(
        m_events.get(4) instanceof SchedulerEvent.Yielded y && y.command() == child,
        "Fifth event should be child yielded");
    assertTrue(
        m_events.get(5) instanceof SchedulerEvent.Yielded y && y.command() == command,
        "Sixth event should be parent yielded");
    assertTrue(
        m_events.get(6) instanceof SchedulerEvent.Mounted m && m.command() == child,
        "Seventh event should be child remounted");
    assertTrue(
        m_events.get(7) instanceof SchedulerEvent.CompletedWithError c
            && c.command() == child
            && c.error() == error,
        "Eighth event should be child completed with error");
    assertTrue(
        m_events.get(8) instanceof SchedulerEvent.Canceled c && c.command() == command,
        "Ninth event should be parent canceled");
  }
}
