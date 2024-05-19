package edu.wpi.first.wpilibj3.command.async;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertFalse;
import static org.junit.jupiter.api.Assertions.assertInstanceOf;
import static org.junit.jupiter.api.Assertions.assertTrue;
import static org.junit.jupiter.api.Assertions.fail;

import java.util.ArrayList;
import java.util.Set;
import java.util.concurrent.atomic.AtomicBoolean;
import java.util.concurrent.atomic.AtomicInteger;
import org.junit.jupiter.api.BeforeEach;
import org.junit.jupiter.api.Test;
import org.junit.jupiter.api.Timeout;

@Timeout(5)
class AsyncSchedulerTest {
  private AsyncScheduler scheduler;

  @BeforeEach
  void setup() {
    scheduler = new AsyncScheduler();
  }

  @Test
  void basic() throws Exception {
    var enabled = new AtomicBoolean(false);
    var ran = new AtomicBoolean(false);
    var command =
        AsyncCommand.noRequirements(
                (coroutine) -> {
                  do {
                    coroutine.yield();
                  } while (!enabled.get());
                  ran.set(true);
                })
            .named("Basic Command");

    scheduler.schedule(command);
    scheduler.run();
    assertTrue(scheduler.isRunning(command), "Command should be running after being scheduled");

    enabled.set(true);
    scheduler.run();
    if (scheduler.isRunning(command)) {
      fail("Command should no longer be running after awaiting its completion");
    }

    assertTrue(ran.get());
  }

  @Test
  void higherPriorityCancels() {
    var subsystem = new HardwareResource("Subsystem", scheduler);

    var lower = new PriorityCommand(-1000, subsystem);
    var higher = new PriorityCommand(+1000, subsystem);

    scheduler.schedule(lower);
    scheduler.run();
    assertTrue(scheduler.isRunning(lower));

    scheduler.schedule(higher);
    scheduler.run();
    assertTrue(scheduler.isRunning(higher));
    assertFalse(scheduler.isRunning(lower));
  }

  @Test
  void lowerPriorityDoesNotCancel() {
    var subsystem = new HardwareResource("Subsystem", scheduler);

    var lower = new PriorityCommand(-1000, subsystem);
    var higher = new PriorityCommand(+1000, subsystem);

    scheduler.schedule(higher);
    scheduler.run();
    assertTrue(scheduler.isRunning(higher));

    scheduler.schedule(lower);
    scheduler.run();
    if (!scheduler.isRunning(higher)) {
      fail("Higher priority command should still be running");
    }
    if (scheduler.isRunning(lower)) {
      fail("Lower priority command should not be running");
    }
  }

  @Test
  void samePriorityCancels() {
    var subsystem = new HardwareResource("Subsystem", scheduler);

    var first = new PriorityCommand(512, subsystem);
    var second = new PriorityCommand(512, subsystem);

    scheduler.schedule(first);
    scheduler.run();
    assertTrue(scheduler.isRunning(first));

    scheduler.schedule(second);
    scheduler.run();
    assertTrue(scheduler.isRunning(second), "New command should be running");
    assertFalse(scheduler.isRunning(first), "Old command should be canceled");
  }

  @Test
  void atomicity() {
    var resource =
        new HardwareResource("X", scheduler) {
          int x = 0;
        };

    // Launch 100 commands that each call `x++` 500 times.
    // If commands run on different threads, the lack of atomic
    // operations or locks will mean the final number will be
    // less than the expected 50,000
    int numCommands = 100;
    int iterations = 500;

    for (int cmdCount = 0; cmdCount < numCommands; cmdCount++) {
      var command =
          AsyncCommand.noRequirements(
                  (coroutine) -> {
                    for (int i = 0; i < iterations; i++) {
                      coroutine.yield();
                      resource.x++;
                    }
                  })
              .named("CountCommand[" + cmdCount + "]");

      scheduler.schedule(command);
    }

    for (int i = 0; i < iterations; i++) {
      scheduler.run();
    }
    scheduler.run();

    assertEquals(numCommands * iterations, resource.x);
  }

  @Test
  void errorDetection() throws Exception {
    var resource = new HardwareResource("X", scheduler);

    var command =
        resource
            .run(
                (coroutine) -> {
                  throw new RuntimeException("The exception");
                })
            .named("Bad Behavior");

    scheduler.schedule(command);

    try {
      scheduler.run();
      fail("An exception should have been thrown");
    } catch (CommandExecutionException e) {
      if (e.getCommand() != command) {
        fail("Expected command " + command + ", but was " + e.getCommand());
      }

      var cause = e.getCause();
      if (cause instanceof RuntimeException re) {
        assertEquals("The exception", re.getMessage());
      } else {
        fail(
            "Expected cause to be a RuntimeException with message 'The exception', but was "
                + cause);
      }
    } catch (Throwable t) {
      fail("Expected a CommandExecutionException to be thrown, but got " + t);
    }
  }

  @Test
  void runResource() {
    var example =
        new HardwareResource("Counting", scheduler) {
          int x = 0;
        };

    AsyncCommand countToTen =
        example
            .run(
                (coroutine) -> {
                  example.x = 0;
                  for (int i = 0; i < 10; i++) {
                    coroutine.yield();
                    example.x++;
                  }
                })
            .named("Count To Ten");

    scheduler.schedule(countToTen);
    for (int i = 0; i < 10; i++) {
      scheduler.run();
    }
    scheduler.run();

    assertEquals(10, example.x);
  }

  @Test
  void cancelOnInterruptDoesNotResume() throws Exception {
    var count = new AtomicInteger(0);

    var resource = new HardwareResource("Resource", scheduler);

    var interrupter =
        AsyncCommand.requiring(resource)
            .executing((coroutine) -> {})
            .withPriority(2)
            .named("Interrupter");

    var canceledCommand =
        AsyncCommand.requiring(resource)
            .executing(
                (coroutine) -> {
                  count.set(1);
                  coroutine.yield();
                  count.set(2);
                })
            .withPriority(1)
            .named("Cancel By Default");

    scheduler.schedule(canceledCommand);
    scheduler.run();

    scheduler.schedule(interrupter);
    scheduler.run();
    assertEquals(1, count.get()); // the second "set" call should not have run
  }

  @Test
  void scheduleOverDefaultDoesNotRescheduleDefault() throws Exception {
    var count = new AtomicInteger(0);

    var resource = new HardwareResource("Resource", scheduler);
    var defaultCmd =
        resource
            .run(
                (coroutine) -> {
                  while (true) {
                    count.incrementAndGet();
                    coroutine.yield();
                  }
                })
            .suspendingOnInterrupt()
            .withPriority(-1)
            .named("Default Command");

    var newerCmd = resource.run((coroutine) -> {}).named("Newer Command");
    resource.setDefaultCommand(defaultCmd);
    scheduler.run();
    assertTrue(scheduler.isRunning(defaultCmd), "Default command should be running");

    scheduler.schedule(newerCmd);
    scheduler.run();
    assertFalse(scheduler.isRunning(defaultCmd), "Default command should have been interrupted");
    assertEquals(1, count.get(), "Default command should have run once");

    scheduler.run();
    assertTrue(scheduler.isRunning(defaultCmd));
  }

  @Test
  void cancelAllCancelsAll() {
    var commands = new ArrayList<AsyncCommand>(10);
    for (int i = 1; i <= 10; i++) {
      commands.add(AsyncCommand.noRequirements(Coroutine::yield).named("Command " + i));
    }
    commands.forEach(scheduler::schedule);
    scheduler.run();
    scheduler.cancelAll();
    for (AsyncCommand command : commands) {
      if (scheduler.isRunning(command)) {
        fail(command.name() + " was not canceled by cancelAll()");
      }
    }
  }

  @Test
  void cancelAllStartsDefaults() {
    var resources = new ArrayList<HardwareResource>(10);
    for (int i = 1; i <= 10; i++) {
      resources.add(new HardwareResource("System " + i, scheduler));
    }

    var command =
        new AsyncCommandBuilder()
            .requiring(resources)
            .executing(Coroutine::yield)
            .named("Big Command");

    // Scheduling the command should evict the on-deck default commands
    scheduler.schedule(command);
    // Then running should get it into the set of running commands
    scheduler.run();
    // Cancelling should clear out the set of running commands
    scheduler.cancelAll();
    // Then ticking the scheduler once to fully remove the command and schedule the defaults
    scheduler.run();
    // Then one more tick to start running the scheduled defaults
    scheduler.run();

    if (scheduler.isRunning(command)) {
      System.err.println(scheduler.getRunningCommands());
      fail(command.name() + " was not canceled by cancelAll()");
    }

    for (var resource : resources) {
      var runningCommands = scheduler.getRunningCommandsFor(resource);
      assertEquals(
          1,
          runningCommands.size(),
          "Resource " + resource + " should have exactly one running command");
      assertInstanceOf(
          IdleCommand.class,
          runningCommands.getFirst(),
          "Resource " + resource + " is not running the default command");
    }
  }

  @Test
  void nestedResourceNestedCommands() {
    var inner = new HardwareResource("Inner", scheduler);
    var outer =
        new HardwareResource("Outer", scheduler) {
          @Override
          public Set<HardwareResource> nestedResources() {
            return Set.of(inner);
          }
        };

    var innerRan = new AtomicBoolean(false);
    var outerRan = new AtomicBoolean(false);

    var innerCommand =
        inner
            .run(
                (coroutine) -> {
                  innerRan.set(true);
                })
            .named("Inner Command");

    var outerCommand =
        outer
            .run(
                (coroutine) -> {
                  scheduler.scheduleAndWait(innerCommand);
                  outerRan.set(true);
                })
            .named("Outer Command");

    scheduler.schedule(outerCommand);
    // First run: schedules the inner command
    scheduler.run();

    // Second run: inner command sets the flag and exits
    scheduler.run();
    assertTrue(innerRan.get());
    assertFalse(scheduler.isRunning(innerCommand));

    // Third run: outer command sets the flag and exits
    scheduler.run();
    assertTrue(outerRan.get());
    assertFalse(scheduler.isRunning(outerCommand));
  }

  record PriorityCommand(int priority, HardwareResource... subsystems) implements AsyncCommand {
    @Override
    public void run(Coroutine coroutine) {
      coroutine.park();
    }

    @Override
    public Set<HardwareResource> requirements() {
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