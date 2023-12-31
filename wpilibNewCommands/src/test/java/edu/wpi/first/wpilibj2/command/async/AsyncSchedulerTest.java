package edu.wpi.first.wpilibj2.command.async;

import static edu.wpi.first.units.Units.Seconds;
import static org.junit.jupiter.api.Assertions.assertFalse;
import static org.junit.jupiter.api.Assertions.assertTrue;

import java.util.Set;
import java.util.concurrent.atomic.AtomicBoolean;
import org.junit.jupiter.api.Test;
import org.junit.jupiter.api.Timeout;

@Timeout(5)
class AsyncSchedulerTest {
  @Test
  void basic() throws Exception {
    var ran = new AtomicBoolean(false);
    var scheduler = new AsyncScheduler();
    var command = AsyncCommand.noHardware(() -> ran.set(true));

    scheduler.schedule(command);
    assertTrue(scheduler.isRunning(command));

    scheduler.await(command);
    assertFalse(scheduler.isRunning(command));

    assertTrue(ran.get());
  }

  @Test
  void higherPriorityCancels() {
    var subsystem = new Resource<>("Subsystem");

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
    var subsystem = new Resource<>("Subsystem");

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
    var subsystem = new Resource<>("Subsystem");

    var first = new PriorityCommand(512, subsystem);
    var second = new PriorityCommand(512, subsystem);

    var scheduler = new AsyncScheduler();
    scheduler.schedule(first);
    assertTrue(scheduler.isRunning(first));

    scheduler.schedule(second);
    assertTrue(scheduler.isRunning(second), "New command should be running");
    assertFalse(scheduler.isRunning(first), "Old command should be canceled");
  }

  @Test
  void group() throws Exception {
    var s1 = new Resource<>("S1");
    var s2 = new Resource<>("S2");

    var s1c1 = new PriorityCommand(11, s1);
    var s2c1 = new PriorityCommand(21, s2);

    var s1c2 = new PriorityCommand(12, s1);
    var s2c2 = new PriorityCommand(22, s2);

    var group =
        s1c1.alongWith(s2c1).withTimeout(Seconds.of(0.12))
            .andThen(s1c2.alongWith(s2c2).withTimeout(Seconds.of(0.08)));

    var scheduler = new AsyncScheduler();

    // schedule the entire group
    scheduler.schedule(group);

    // need to wait a bit for the virtual threads to spin up and the commands to get scheduled
    Thread.sleep(10);

    // both commands in stage 1 should be running now
    assertTrue(scheduler.isRunning(s1c1));
    assertTrue(scheduler.isRunning(s2c1));

    // wait for one of the commands in the first stage to complete
    scheduler.await(s1c1);

    Thread.sleep(10); // wait a bit...

    // both commands in stage 2 should now be running
    assertTrue(scheduler.isRunning(s1c2));
    assertTrue(scheduler.isRunning(s2c2));

    // ... and both commands in stage 1 shouldn't be
    assertFalse(scheduler.isRunning(s1c1));
    assertFalse(scheduler.isRunning(s2c1));

    // wait for one of the commands in the second stage to complete
    scheduler.await(s2c2);

    // everything should have stopped by this point
    assertFalse(scheduler.isRunning(s1c2));
    assertFalse(scheduler.isRunning(s2c2));
  }

  record PriorityCommand(int priority, Resource<?>... subsystems) implements AsyncCommand {
    @Override
    public void run() throws Exception {
      Thread.sleep(Long.MAX_VALUE);
    }

    @Override
    public Set<Resource> requirements() {
      return Set.of(subsystems);
    }

    @Override
    public String toString() {
      return "PriorityCommand[priority=" + priority + ", subsystems=" + Set.of(subsystems) + "]";
    }
  }
}
