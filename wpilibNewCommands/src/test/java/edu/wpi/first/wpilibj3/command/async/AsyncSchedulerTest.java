package edu.wpi.first.wpilibj3.command.async;

import static edu.wpi.first.units.Units.Milliseconds;
import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertFalse;
import static org.junit.jupiter.api.Assertions.assertThrows;
import static org.junit.jupiter.api.Assertions.assertTrue;
import static org.junit.jupiter.api.Assertions.fail;

import java.util.ArrayList;
import java.util.Set;
import java.util.concurrent.atomic.AtomicBoolean;
import org.junit.jupiter.api.BeforeAll;
import org.junit.jupiter.api.RepeatedTest;
import org.junit.jupiter.api.Test;
import org.junit.jupiter.api.Timeout;

@Timeout(5)
class AsyncSchedulerTest {
  @BeforeAll
  static void warmup() {
    var scheduler = new AsyncScheduler();
    scheduler.schedule(AsyncCommand.noHardware(() -> {}).named("warmup"));
  }

  @Test
  void basic() throws Exception {
    var ran = new AtomicBoolean(false);
    var scheduler = new AsyncScheduler();
    var command = AsyncCommand.noHardware(() -> ran.set(true)).named("Basic");

    scheduler.schedule(command);
    assertTrue(scheduler.isRunning(command));

    scheduler.await(command);
    assertFalse(scheduler.isRunning(command));

    assertTrue(ran.get());
  }

  @Test
  void higherPriorityCancels() {
    var subsystem = new Resource("Subsystem");

    var lower = new PriorityCommand(-1000, subsystem);
    var higher = new PriorityCommand(+1000, subsystem);

    var scheduler = new AsyncScheduler();
    scheduler.schedule(lower);
    assertTrue(scheduler.isRunning(lower));

    scheduler.schedule(higher);
    assertTrue(scheduler.isRunning(higher));
    assertFalse(scheduler.isRunning(lower));
  }

  @Test
  void lowerPriorityDoesNotCancel() {
    var subsystem = new Resource("Subsystem");

    var lower = new PriorityCommand(-1000, subsystem);
    var higher = new PriorityCommand(+1000, subsystem);

    var scheduler = new AsyncScheduler();
    scheduler.schedule(higher);
    assertTrue(scheduler.isRunning(higher));

    scheduler.schedule(lower);
    assertTrue(scheduler.isRunning(higher), "Higher priority command should still be running");
    assertFalse(scheduler.isRunning(lower), "Lower priority command should not be running");
  }

  @Test
  void samePriorityCancels() {
    var subsystem = new Resource("Subsystem");

    var first = new PriorityCommand(512, subsystem);
    var second = new PriorityCommand(512, subsystem);

    var scheduler = new AsyncScheduler();
    scheduler.schedule(first);
    assertTrue(scheduler.isRunning(first));

    scheduler.schedule(second);
    assertTrue(scheduler.isRunning(second), "New command should be running");
    assertFalse(scheduler.isRunning(first), "Old command should be canceled");
  }

  @RepeatedTest(10)
  void group() throws Exception {
    // NOTE: If this is the first test that runs, its first iteration will be flaky,
    //       likely due to loading classes and static initialization
    var scheduler = new AsyncScheduler();

    var s1 = new Resource("S1", scheduler);
    var s2 = new Resource("S2", scheduler);

    var s1c1 = new PriorityCommand(11, s1);
    var s2c1 = new PriorityCommand(21, s2);

    var s1c2 = new PriorityCommand(12, s1);
    var s2c2 = new PriorityCommand(22, s2);

    var group =
        s1c1.alongWith(s2c1).withTimeout(Milliseconds.of(120))
            .andThen(s1c2.alongWith(s2c2).withTimeout(Milliseconds.of(80)));

    // schedule the entire group
    scheduler.schedule(group);

    // need to wait a bit for the virtual threads to spin up and the commands to get scheduled
    Thread.sleep(10);

    // both commands in stage 1 should be running now
    assertTrue(scheduler.isRunning(s1c1));
    assertTrue(scheduler.isRunning(s2c1));

    // wait for both of the commands in the first stage to complete
    scheduler.await(s1c1);
    scheduler.await(s2c1);

    Thread.sleep(10); // wait a bit...

    // both commands in stage 2 should now be running
    assertTrue(scheduler.isRunning(s1c2));
    assertTrue(scheduler.isRunning(s2c2));

    // ... and both commands in stage 1 shouldn't be
    assertFalse(scheduler.isRunning(s1c1));
    assertFalse(scheduler.isRunning(s2c1));

    // wait for both of the commands in the second stage to complete
    scheduler.await(s1c2);
    scheduler.await(s2c2);

    // everything should have stopped by this point
    assertFalse(scheduler.isRunning(s1c2));
    assertFalse(scheduler.isRunning(s2c2));
  }

  @Test
  void atomicity() throws Exception {
    var scheduler = new AsyncScheduler();
    var resource = new Resource("X", scheduler) {
      int x = 0;
    };

    // Launch 100 commands that each call `x++` 500 times.
    // If commands run on different threads, the lack of atomic
    // operations or locks will mean the final number will be
    // less than the expected 50,000
    int numCommands = 100;
    int iterations = 500;
    var commands = new ArrayList<AsyncCommand>(numCommands);

    for (int cmdCount = 0; cmdCount < numCommands; cmdCount++) {
      var command = AsyncCommand.noHardware(() -> {
        for (int i = 0; i < iterations; i++) {
          Thread.sleep(1);
          resource.x++;
        }
      }).named("CountCommand[" + cmdCount + "]");

      scheduler.schedule(command);
      commands.add(command);
    }

    for (var command : commands) {
      scheduler.await(command);
    }

    assertEquals(numCommands * iterations, resource.x);
  }

  @Test
  void errorDetection() throws Exception {
    var scheduler = new AsyncScheduler();
    var resource = new Resource("X", scheduler);

    var command = resource.run(() -> {
      Thread.sleep(10);
      throw new RuntimeException("The exception");
    }).named("Bad Behavior");

    scheduler.schedule(command);

    // wait for the command to complete, but not using await, since that would throw an error
    // immediately - and we want to confirm that checkForErrors will correctly capture and throw
    while (scheduler.isRunning(command)) {
      Thread.sleep(1);
    }

    try {
      scheduler.checkForErrors();
      fail("An exception should have been thrown");
    } catch (CommandExecutionException e) {
      System.err.println(e.getMessage());
      e.printStackTrace(System.err);
      if (e.getCommand() != command) {
        fail("Expected command " + command + ", but was " + e.getCommand());
      }

      var cause = e.getCause();
      if (cause instanceof RuntimeException re) {
        assertEquals("The exception", re.getMessage());
      } else {
        fail("Expected cause to be a RuntimeException with message 'The exception', but was " + cause);
      }
    }
  }

  @Test
  void runResource() throws Exception {
    var scheduler = new AsyncScheduler();
    var example = new Resource("Counting", scheduler) {
      int x = 0;
    };

    AsyncCommand countToTen = example.run(() -> {
      example.x = 0;
      for (int i = 0; i < 10; i++) {
        AsyncCommand.yield();
        example.x++;
      }
    }).named("Count To Ten");

    scheduler.schedule(countToTen);
    scheduler.await(countToTen);

    assertEquals(10, example.x);
  }

  record PriorityCommand(int priority, Resource... subsystems) implements AsyncCommand {
    @Override
    public void run() throws Exception {
      Thread.sleep(Long.MAX_VALUE);
    }

    @Override
    public Set<Resource> requirements() {
      return Set.of(subsystems);
    }

    @Override
    public String name() {
      return toString();
    }

    @Override
    public String toString() {
      return "PriorityCommand[priority=" + priority + ", subsystems=" + Set.of(subsystems) + "]";
    }
  }
}
