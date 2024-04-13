package edu.wpi.first.wpilibj3.command.async;

import static edu.wpi.first.units.Units.Milliseconds;
import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertFalse;
import static org.junit.jupiter.api.Assertions.assertTrue;
import static org.junit.jupiter.api.Assertions.fail;

import java.util.ArrayList;
import java.util.Map;
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
    var subsystem = new HardwareResource("Subsystem");

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
    var subsystem = new HardwareResource("Subsystem");

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
    var subsystem = new HardwareResource("Subsystem");

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

    var s1 = new HardwareResource("S1", scheduler);
    var s2 = new HardwareResource("S2", scheduler);

    var s1c1 = new PriorityCommand(11, s1);
    var s2c1 = new PriorityCommand(21, s2);

    var s1c2 = new PriorityCommand(12, s1);
    var s2c2 = new PriorityCommand(22, s2);

    var firstStage =
        ParallelGroup.onScheduler(scheduler)
            .all(s1c1, s2c1)
            .withTimeout(Milliseconds.of(120))
            .named("First Stage");
    var secondStage =
        ParallelGroup.onScheduler(scheduler)
            .all(s1c2, s2c2)
            .withTimeout(Milliseconds.of(80))
            .named("Second Stage");

    var group = new Sequence(scheduler, firstStage, secondStage);

    assertEquals(Set.of(s1, s2), group.requirements());
    assertEquals(Set.of(s1, s2), firstStage.requirements());
    assertEquals(Set.of(s1, s2), secondStage.requirements());

    // schedule the entire group
    scheduler.schedule(group);

    // need to wait a bit for the virtual threads to spin up and the commands to get scheduled
    Thread.sleep(20);

    // The "running" commands should only be the top-level owners, no nested commands should be here
    assertEquals(Map.of(s1, group, s2, group), scheduler.getRunningCommands());

    // The sequence and both commands in stage 1 should be running now
    assertTrue(scheduler.isRunning(firstStage));
    assertFalse(scheduler.isRunning(secondStage));
    assertTrue(scheduler.isRunning(s1c1));
    assertTrue(scheduler.isRunning(s2c1));

    // wait for the first stag to complete...
    scheduler.await(firstStage);

    Thread.sleep(10); // wait a bit the second stage to get scheduled

    // both commands in stage 2 should now be running
    assertTrue(scheduler.isRunning(s1c2));
    assertTrue(scheduler.isRunning(s2c2));
    assertTrue(scheduler.isRunning(secondStage));

    // ... and both commands in stage 1 shouldn't be
    assertFalse(scheduler.isRunning(s1c1));
    assertFalse(scheduler.isRunning(s2c1));
    assertFalse(scheduler.isRunning(firstStage));

    // wait for both of the commands in the second stage to complete
    scheduler.await(s1c2);
    scheduler.await(s2c2);

    // everything should have stopped by this point
    assertFalse(scheduler.isRunning(s1c2));
    assertFalse(scheduler.isRunning(s2c2));

    scheduler.await(group); // wait for the group to fully complete (including cleanup)

    // The shadowrun should no longer have any references to any commands in the group
    assertEquals(
        Map.of(
            s1, Set.of(s1.getDefaultCommand()),
            s2, Set.of(s2.getDefaultCommand())),
        scheduler.shadowrun);
  }

  @Test
  void atomicity() throws Exception {
    var scheduler = new AsyncScheduler();
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
                      Thread.sleep(1);
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

  @Test
  void errorDetection() throws Exception {
    var scheduler = new AsyncScheduler();
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
      System.err.println(e.getMessage());
      e.printStackTrace(System.err);
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

  @Test
  void runResource() throws Exception {
    var scheduler = new AsyncScheduler();
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
                    AsyncCommand.pause();
                    example.x++;
                  }
                })
            .named("Count To Ten");

    scheduler.schedule(countToTen);
    scheduler.await(countToTen);

    assertEquals(10, example.x);
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
