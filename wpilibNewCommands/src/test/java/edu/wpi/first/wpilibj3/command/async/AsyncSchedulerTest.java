package edu.wpi.first.wpilibj3.command.async;

import static edu.wpi.first.units.Units.Milliseconds;
import static edu.wpi.first.units.Units.Seconds;
import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertFalse;
import static org.junit.jupiter.api.Assertions.assertThrows;
import static org.junit.jupiter.api.Assertions.assertTrue;
import static org.junit.jupiter.api.Assertions.fail;

import java.util.ArrayList;
import java.util.Set;
import java.util.concurrent.atomic.AtomicBoolean;
import java.util.concurrent.atomic.AtomicInteger;
import org.junit.jupiter.api.AfterEach;
import org.junit.jupiter.api.BeforeEach;
import org.junit.jupiter.api.RepeatedTest;
import org.junit.jupiter.api.Timeout;

@Timeout(5)
class AsyncSchedulerTest {
  private AsyncScheduler scheduler;

  @BeforeEach
  void setup() {
    Logger.clear();

    // Initialize with high default timeouts so random jitter doesn't make tests flake out
    scheduler = new AsyncScheduler(
        Seconds.one(), // event loop period
        Milliseconds.of(25), // cancel timeout
        Milliseconds.of(25)); // schedule timeout
  }

  @AfterEach
  void printLogs() {
    System.out.println(Logger.formattedLogTable());
    Logger.clear();
  }

  @RepeatedTest(value = 1000, failureThreshold = 1)
  void basic() throws Exception {
    var enabled = new AtomicBoolean(false);
    var ran = new AtomicBoolean(false);
    var command = AsyncCommand.noHardware(() -> {
      do {} while (!enabled.get());
      ran.set(true);
    }).named("Basic Command");

    scheduler.schedule(command);
    assertTrue(scheduler.isRunning(command), "Command should be running after being scheduled");

    enabled.set(true);
    scheduler.await(command);
    if (scheduler.isRunning(command)) {
      System.err.println(Logger.formattedLogTable());
      fail("Command should no longer be running after awaiting its completion");
    }

    assertTrue(ran.get());
  }

  @RepeatedTest(value = 100, failureThreshold = 1)
  void higherPriorityCancels() {
    var subsystem = new HardwareResource("Subsystem", scheduler);

    var lower = new PriorityCommand(-1000, subsystem);
    var higher = new PriorityCommand(+1000, subsystem);

    scheduler.scheduleTimeout = Milliseconds.of(25);
    scheduler.schedule(lower);
    assertTrue(scheduler.isRunning(lower));

    scheduler.schedule(higher);
    assertTrue(scheduler.isRunning(higher));
    assertFalse(scheduler.isRunning(lower));
  }

  @RepeatedTest(value = 100, failureThreshold = 1)
  void lowerPriorityDoesNotCancel() {
    var subsystem = new HardwareResource("Subsystem", scheduler);

    var lower = new PriorityCommand(-1000, subsystem);
    var higher = new PriorityCommand(+1000, subsystem);

    scheduler.schedule(higher);
    assertTrue(scheduler.isRunning(higher));

    scheduler.schedule(lower);
    if (!scheduler.isRunning(higher)) {
      System.err.println(Logger.formattedLogTable());
      fail("Higher priority command should still be running");
    }
    if (scheduler.isRunning(lower)) {
      System.err.println(Logger.formattedLogTable());
      fail("Lower priority command should not be running");
    }
  }

  @RepeatedTest(value = 100, failureThreshold = 1)
  void samePriorityCancels() {
    var subsystem = new HardwareResource("Subsystem", scheduler);

    var first = new PriorityCommand(512, subsystem);
    var second = new PriorityCommand(512, subsystem);

    scheduler.schedule(first);
    assertTrue(scheduler.isRunning(first));

    scheduler.schedule(second);
    assertTrue(scheduler.isRunning(second), "New command should be running");
    assertFalse(scheduler.isRunning(first), "Old command should be canceled");
  }

  @RepeatedTest(value = 100, failureThreshold = 1)
  void atomicity() throws Exception {
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
    var commands = new ArrayList<AsyncCommand>(numCommands);

    for (int cmdCount = 0; cmdCount < numCommands; cmdCount++) {
      var command =
          AsyncCommand.noHardware(
                  () -> {
                    for (int i = 0; i < iterations; i++) {
                      scheduler.pauseCurrentCommand(Milliseconds.one());
                      resource.x++;
                    }
                  })
              .named("CountCommand[" + cmdCount + "]");

      scheduler.schedule(command);
      commands.add(command);
    }

    for (var command : commands) {
      scheduler.await(command);
    }

    assertEquals(numCommands * iterations, resource.x);
  }

  @RepeatedTest(value = 100, failureThreshold = 1)
  void errorDetection() throws Exception {
    var resource = new HardwareResource("X", scheduler);

    var command =
        resource
            .run(
                () -> {
                  Thread.sleep(10);
                  throw new RuntimeException("The exception");
                })
            .named("Bad Behavior");

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
    }
  }

  @RepeatedTest(value = 100, failureThreshold = 1)
  void runResource() throws Exception {
    var example =
        new HardwareResource("Counting", scheduler) {
          int x = 0;
        };

    AsyncCommand countToTen =
        example
            .run(
                () -> {
                  example.x = 0;
                  for (int i = 0; i < 10; i++) {
                    scheduler.pauseCurrentCommand(Milliseconds.one());
                    example.x++;
                  }
                })
            .named("Count To Ten");

    scheduler.schedule(countToTen);
    scheduler.await(countToTen);

    assertEquals(10, example.x);
  }

  @RepeatedTest(value = 100, failureThreshold = 1)
  void cancelOnInterruptDoesNotResume() throws Exception {
    var count = new AtomicInteger(0);

    var resource = new HardwareResource("Resource", scheduler);

    var interrupter = AsyncCommand.requiring(resource).executing(() -> {
      Thread.sleep(50);
    }).withPriority(2).named("Interrupter");

    var canceledCommand = AsyncCommand.requiring(resource).executing(() -> {
      count.set(1);
      scheduler.pauseCurrentCommand();
      count.set(2);
    }).withPriority(1).named("Cancel By Default");

    scheduler.schedule(canceledCommand);
    Thread.sleep(5); // wait for command to start up and hit the pause

    scheduler.schedule(interrupter);
    scheduler.await(canceledCommand);
    assertEquals(1, count.get()); // the second set should not have run
  }

  @RepeatedTest(value = 100, failureThreshold = 1)
  void pauseOutsideCommand() {
    assertThrows(IllegalStateException.class, scheduler::pauseCurrentCommand);
  }

  @RepeatedTest(value = 10, failureThreshold = 1)
  void scheduleOverDefaultDoesNotRescheduleDefault() throws Exception {
    var count = new AtomicInteger(0);

    var resource = new HardwareResource("Resource", scheduler);
    var defaultCmd = resource.run(() -> {
      count.incrementAndGet();
      scheduler.pauseCurrentCommand(Milliseconds.of(50));
    }).named("Default Command");

    var newerCmd = resource.run(scheduler::pauseCurrentCommand).named("Newer Command");
    resource.setDefaultCommand(defaultCmd);
    assertTrue(scheduler.isRunning(defaultCmd));

    scheduler.schedule(newerCmd);
    assertFalse(scheduler.isRunning(defaultCmd));
    assertEquals(1, count.get());

    scheduler.await(newerCmd);
    assertTrue(scheduler.isRunning(defaultCmd));
  }

  @RepeatedTest(value = 10, failureThreshold = 1)
  void cancelAllCancelsAll() {
    var commands = new ArrayList<AsyncCommand>(10);
    for (int i = 1; i <= 10; i++) {
      commands.add(AsyncCommand.noHardware(AsyncCommand::pause).named("Command " + i));
    }
    commands.forEach(scheduler::schedule);
    scheduler.cancelAll();
    for (AsyncCommand command : commands) {
      if (scheduler.isRunning(command)) {
        fail(command.name() + " was not canceled by cancelAll()");
      }
    }
  }

  @RepeatedTest(value = 100, failureThreshold = 1)
  void cancelAllStartsDefaults() {
    var resources = new ArrayList<HardwareResource>(10);
    for (int i = 1; i <= 10; i++) {
      resources.add(new HardwareResource("System " + i, scheduler));
    }

    var command =
        new AsyncCommandBuilder()
            .requiring(resources)
            .executing(AsyncCommand::pause)
            .named("Big Command");

    for (int i = 0; i < 10; i++) {
      scheduler.schedule(command);
      scheduler.cancelAll();

      if (scheduler.isRunning(command)) {
        System.err.println(scheduler.getRunningCommands());
        fail("Run " + i + ": " + command.name() + " was not canceled by cancelAll()");
      }

      for (var resource : resources) {
        if (!scheduler.isRunning(resource.getDefaultCommand())) {
          System.err.println(Logger.formattedLogTable());
          System.err.println("Running commands: " + scheduler.getRunningCommands());
          fail("Run " + i + ": " + "Default command for " + resource.getName() + " should have been scheduled after cancelAll() was called");
        }
      }
    }
  }

  static final class RunningCommand implements AsyncCommand {
    private final Set<HardwareResource> subsystems;
    boolean ran = false;

    RunningCommand(HardwareResource... subsystems) {
      this.subsystems = Set.of(subsystems);
    }

    @Override
    public void run() throws Exception {
      // Sleep and set the 'ran' flag only when interrupted
      try {
        AsyncCommand.pause(Milliseconds.of(Long.MAX_VALUE));
      } finally {
        ran = true;
      }
    }

    @Override
    public String name() {
      return "RunningCommand";
    }

    @Override
    public Set<HardwareResource> requirements() {
      return subsystems;
    }
  }

  record PriorityCommand(int priority, HardwareResource... subsystems) implements AsyncCommand {
    @Override
    public void run() throws Exception {
      Thread.sleep(Long.MAX_VALUE);
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
