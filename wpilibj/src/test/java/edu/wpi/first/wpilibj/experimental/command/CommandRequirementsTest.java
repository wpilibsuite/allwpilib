package edu.wpi.first.wpilibj.experimental.command;

import java.util.Map;

import org.junit.jupiter.api.Test;

import edu.wpi.first.wpilibj.command.IllegalUseOfCommandException;

import static org.junit.jupiter.api.Assertions.assertFalse;
import static org.junit.jupiter.api.Assertions.assertThrows;
import static org.junit.jupiter.api.Assertions.assertTrue;
import static org.mockito.Mockito.never;
import static org.mockito.Mockito.verify;

@SuppressWarnings("PMD.TooManyMethods")
public class CommandRequirementsTest extends CommandTestBase {
  @Test
  void requirementInterruptTest() {
    CommandScheduler scheduler = new CommandScheduler();

    Subsystem requirement = new TestSubsystem();

    MockCommandHolder interruptedHolder = new MockCommandHolder(true, requirement);
    Command interrupted = interruptedHolder.getMock();
    MockCommandHolder interrupterHolder = new MockCommandHolder(true, requirement);
    Command interrupter = interrupterHolder.getMock();

    scheduler.scheduleCommand(interrupted, true);
    scheduler.run();
    scheduler.scheduleCommand(interrupter, true);
    scheduler.run();

    verify(interrupted).initialize();
    verify(interrupted).execute();
    verify(interrupted).end(true);

    verify(interrupter).initialize();
    verify(interrupter).execute();

    assertFalse(scheduler.isScheduled(interrupted));
    assertTrue(scheduler.isScheduled(interrupter));
  }

  @Test
  void requirementUninterruptibleTest() {
    CommandScheduler scheduler = new CommandScheduler();

    Subsystem requirement = new TestSubsystem();

    MockCommandHolder interruptedHolder = new MockCommandHolder(true, requirement);
    Command notInterrupted = interruptedHolder.getMock();
    MockCommandHolder interrupterHolder = new MockCommandHolder(true, requirement);
    Command interrupter = interrupterHolder.getMock();

    scheduler.scheduleCommand(notInterrupted, false);
    scheduler.scheduleCommand(interrupter, true);

    assertTrue(scheduler.isScheduled(notInterrupted));
    assertFalse(scheduler.isScheduled(interrupter));
  }

  @Test
  void parallelGroupRequirementTest() {
    Subsystem system1 = new TestSubsystem();
    Subsystem system2 = new TestSubsystem();
    Subsystem system3 = new TestSubsystem();
    Subsystem system4 = new TestSubsystem();

    CommandScheduler scheduler = new CommandScheduler();

    MockCommandHolder command1Holder = new MockCommandHolder(true, system1, system2);
    Command command1 = command1Holder.getMock();
    MockCommandHolder command2Holder = new MockCommandHolder(true, system3);
    Command command2 = command2Holder.getMock();
    MockCommandHolder command3Holder = new MockCommandHolder(true, system3, system4);
    Command command3 = command3Holder.getMock();

    Command group = new ParallelCommandGroup(command1, command2);

    scheduler.scheduleCommand(group, true);
    scheduler.scheduleCommand(command3, true);

    assertFalse(scheduler.isScheduled(group));
    assertTrue(scheduler.isScheduled(command3));
  }

  @Test
  void parallelRaceRequirementTest() {
    Subsystem system1 = new TestSubsystem();
    Subsystem system2 = new TestSubsystem();
    Subsystem system3 = new TestSubsystem();
    Subsystem system4 = new TestSubsystem();

    CommandScheduler scheduler = new CommandScheduler();

    MockCommandHolder command1Holder = new MockCommandHolder(true, system1, system2);
    Command command1 = command1Holder.getMock();
    MockCommandHolder command2Holder = new MockCommandHolder(true, system3);
    Command command2 = command2Holder.getMock();
    MockCommandHolder command3Holder = new MockCommandHolder(true, system3, system4);
    Command command3 = command3Holder.getMock();

    Command group = new ParallelRaceGroup(command1, command2);

    scheduler.scheduleCommand(group, true);
    scheduler.scheduleCommand(command3, true);

    assertFalse(scheduler.isScheduled(group));
    assertTrue(scheduler.isScheduled(command3));
  }

  @Test
  void parallelDictatorRequirementTest() {
    Subsystem system1 = new TestSubsystem();
    Subsystem system2 = new TestSubsystem();
    Subsystem system3 = new TestSubsystem();
    Subsystem system4 = new TestSubsystem();

    CommandScheduler scheduler = new CommandScheduler();

    MockCommandHolder command1Holder = new MockCommandHolder(true, system1, system2);
    Command command1 = command1Holder.getMock();
    MockCommandHolder command2Holder = new MockCommandHolder(true, system3);
    Command command2 = command2Holder.getMock();
    MockCommandHolder command3Holder = new MockCommandHolder(true, system3, system4);
    Command command3 = command3Holder.getMock();

    Command group = new ParallelDictatorGroup(command1, command2);

    scheduler.scheduleCommand(group, true);
    scheduler.scheduleCommand(command3, true);

    assertFalse(scheduler.isScheduled(group));
    assertTrue(scheduler.isScheduled(command3));
  }

  @Test
  void parallelGroupRequirementErrorTest() {
    Subsystem system1 = new TestSubsystem();
    Subsystem system2 = new TestSubsystem();
    Subsystem system3 = new TestSubsystem();

    MockCommandHolder command1Holder = new MockCommandHolder(true, system1, system2);
    Command command1 = command1Holder.getMock();
    MockCommandHolder command2Holder = new MockCommandHolder(true, system2, system3);
    Command command2 = command2Holder.getMock();

    assertThrows(IllegalUseOfCommandException.class,
        () -> new ParallelCommandGroup(command1, command2));
  }

  @Test
  void parallelRaceRequirementErrorTest() {
    Subsystem system1 = new TestSubsystem();
    Subsystem system2 = new TestSubsystem();
    Subsystem system3 = new TestSubsystem();

    MockCommandHolder command1Holder = new MockCommandHolder(true, system1, system2);
    Command command1 = command1Holder.getMock();
    MockCommandHolder command2Holder = new MockCommandHolder(true, system2, system3);
    Command command2 = command2Holder.getMock();

    assertThrows(IllegalUseOfCommandException.class,
        () -> new ParallelRaceGroup(command1, command2));
  }

  @Test
  void parallelDictatorRequirementErrorTest() {
    Subsystem system1 = new TestSubsystem();
    Subsystem system2 = new TestSubsystem();
    Subsystem system3 = new TestSubsystem();

    MockCommandHolder command1Holder = new MockCommandHolder(true, system1, system2);
    Command command1 = command1Holder.getMock();
    MockCommandHolder command2Holder = new MockCommandHolder(true, system2, system3);
    Command command2 = command2Holder.getMock();

    assertThrows(IllegalUseOfCommandException.class,
        () -> new ParallelDictatorGroup(command1, command2));
  }

  @Test
  void sequentialGroupRequirementTest() {
    Subsystem system1 = new TestSubsystem();
    Subsystem system2 = new TestSubsystem();
    Subsystem system3 = new TestSubsystem();
    Subsystem system4 = new TestSubsystem();

    CommandScheduler scheduler = new CommandScheduler();

    MockCommandHolder command1Holder = new MockCommandHolder(true, system1, system2);
    Command command1 = command1Holder.getMock();
    MockCommandHolder command2Holder = new MockCommandHolder(true, system3);
    Command command2 = command2Holder.getMock();
    MockCommandHolder command3Holder = new MockCommandHolder(true, system3, system4);
    Command command3 = command3Holder.getMock();

    Command group = new SequentialCommandGroup(command1, command2);

    scheduler.scheduleCommand(group, true);
    scheduler.scheduleCommand(command3, true);

    assertFalse(scheduler.isScheduled(group));
    assertTrue(scheduler.isScheduled(command3));
  }

  @Test
  void selectCommandRequirementTest() {
    Subsystem system1 = new TestSubsystem();
    Subsystem system2 = new TestSubsystem();
    Subsystem system3 = new TestSubsystem();
    Subsystem system4 = new TestSubsystem();

    CommandScheduler scheduler = new CommandScheduler();

    MockCommandHolder command1Holder = new MockCommandHolder(true, system1, system2);
    Command command1 = command1Holder.getMock();
    MockCommandHolder command2Holder = new MockCommandHolder(true, system3);
    Command command2 = command2Holder.getMock();
    MockCommandHolder command3Holder = new MockCommandHolder(true, system3, system4);
    Command command3 = command3Holder.getMock();

    SelectCommand<String> selectCommand =
        new SelectCommand<>(Map.ofEntries(
            Map.entry("one", command1),
            Map.entry("two", command2),
            Map.entry("three", command3)),
            () -> "one");

    scheduler.scheduleCommand(selectCommand, true);
    scheduler.scheduleCommand(command2, true);

    assertTrue(scheduler.isScheduled(command2));
    assertFalse(scheduler.isScheduled(selectCommand));

    verify(command1).end(true);
    verify(command2, never()).end(true);
    verify(command3, never()).end(true);
  }

  @Test
  void conditionalCommandRequirementTest() {
    Subsystem system1 = new TestSubsystem();
    Subsystem system2 = new TestSubsystem();
    Subsystem system3 = new TestSubsystem();

    CommandScheduler scheduler = new CommandScheduler();

    MockCommandHolder command1Holder = new MockCommandHolder(true, system1, system2);
    Command command1 = command1Holder.getMock();
    MockCommandHolder command2Holder = new MockCommandHolder(true, system3);
    Command command2 = command2Holder.getMock();

    ConditionalCommand conditionalCommand = new ConditionalCommand(command1, command2, () -> true);

    scheduler.scheduleCommand(conditionalCommand, true);
    scheduler.scheduleCommand(command2, true);

    assertTrue(scheduler.isScheduled(command2));
    assertFalse(scheduler.isScheduled(conditionalCommand));

    verify(command1).end(true);
    verify(command2, never()).end(true);
  }

  @Test
  void defaultCommandRequirementErrorTest() {
    CommandScheduler scheduler = new CommandScheduler();

    Subsystem system = new TestSubsystem();

    Command missingRequirement = new WaitUntilCommand(() -> false);
    Command ends = new InstantCommand(() -> { }, system);

    assertThrows(IllegalUseOfCommandException.class,
        () -> scheduler.setDefaultCommand(system, missingRequirement));
    assertThrows(IllegalUseOfCommandException.class,
        () -> scheduler.setDefaultCommand(system, ends));
  }
}
