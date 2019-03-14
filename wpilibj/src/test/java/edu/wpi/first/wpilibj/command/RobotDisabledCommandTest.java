package edu.wpi.first.wpilibj.command;

import edu.wpi.first.hal.sim.DriverStationSim;
import edu.wpi.first.wpilibj.DriverStation;
import org.junit.jupiter.api.Test;

import static org.junit.jupiter.api.Assertions.*;
import static org.mockito.Mockito.*;

public class RobotDisabledCommandTest extends ICommandTestBase {
  @Test
  void robotDisabledCommandCancelTest() {
    DriverStationSim sim = new DriverStationSim();
    sim.setDsAttached(true);

    SchedulerNew scheduler = new SchedulerNew();

    MockCommandHolder holder = new MockCommandHolder(false, new Subsystem[0]);
    ICommand mockCommand = holder.getMock();

    scheduler.scheduleCommand(mockCommand, true);

    assertTrue(scheduler.isScheduled(mockCommand));

    sim.setEnabled(false);
    sim.notifyNewData();
    DriverStation.getInstance().isNewControlData();
    while (DriverStation.getInstance().isEnabled()) {
    }

    scheduler.run();

    assertFalse(scheduler.isScheduled(mockCommand));

    sim.setEnabled(true);
    sim.notifyNewData();
    DriverStation.getInstance().isNewControlData();
    while (!DriverStation.getInstance().isEnabled()) {
    }
  }

  @Test
  void runWhenDisabledTest() {
    DriverStationSim sim = new DriverStationSim();
    sim.setDsAttached(true);

    SchedulerNew scheduler = new SchedulerNew();

    MockCommandHolder holder = new MockCommandHolder(true, new Subsystem[0]);
    ICommand mockCommand = holder.getMock();

    scheduler.scheduleCommand(mockCommand, true);

    assertTrue(scheduler.isScheduled(mockCommand));

    sim.setEnabled(false);
    sim.notifyNewData();
    DriverStation.getInstance().isNewControlData();
    while (DriverStation.getInstance().isEnabled()) {
    }

    scheduler.run();

    assertTrue(scheduler.isScheduled(mockCommand));

    sim.setEnabled(true);
    sim.notifyNewData();
    DriverStation.getInstance().isNewControlData();
    while (!DriverStation.getInstance().isEnabled()) {
    }
  }

  @Test
  void sequentialGroupRunWhenDisabledTest() {
    DriverStationSim sim = new DriverStationSim();
    sim.setDsAttached(true);

    SchedulerNew scheduler = new SchedulerNew();

    MockCommandHolder command1Holder = new MockCommandHolder(true, new Subsystem[0]);
    ICommand command1 = command1Holder.getMock();
    MockCommandHolder command2Holder = new MockCommandHolder(true, new Subsystem[0]);
    ICommand command2 = command2Holder.getMock();
    MockCommandHolder command3Holder = new MockCommandHolder(true, new Subsystem[0]);
    ICommand command3 = command3Holder.getMock();
    MockCommandHolder command4Holder = new MockCommandHolder(false, new Subsystem[0]);
    ICommand command4 = command4Holder.getMock();

    ICommand runWhenDisabled = new SequentialCommandGroup(command1, command2);
    ICommand dontRunWhenDisabled = new SequentialCommandGroup(command3, command4);

    scheduler.scheduleCommand(runWhenDisabled, true);
    scheduler.scheduleCommand(runWhenDisabled, true);

    sim.setEnabled(false);
    sim.notifyNewData();
    DriverStation.getInstance().isNewControlData();
    while (DriverStation.getInstance().isEnabled()) {
    }

    assertTrue(scheduler.isScheduled(runWhenDisabled));
    assertFalse(scheduler.isScheduled(dontRunWhenDisabled));

    sim.setEnabled(true);
    sim.notifyNewData();
    DriverStation.getInstance().isNewControlData();
    while (!DriverStation.getInstance().isEnabled()) {
    }
  }

  @Test
  void parallelGroupRunWhenDisabledTest() {
    DriverStationSim sim = new DriverStationSim();
    sim.setDsAttached(true);

    SchedulerNew scheduler = new SchedulerNew();

    MockCommandHolder command1Holder = new MockCommandHolder(true, new Subsystem[0]);
    ICommand command1 = command1Holder.getMock();
    MockCommandHolder command2Holder = new MockCommandHolder(true, new Subsystem[0]);
    ICommand command2 = command2Holder.getMock();
    MockCommandHolder command3Holder = new MockCommandHolder(true, new Subsystem[0]);
    ICommand command3 = command3Holder.getMock();
    MockCommandHolder command4Holder = new MockCommandHolder(false, new Subsystem[0]);
    ICommand command4 = command4Holder.getMock();

    ICommand runWhenDisabled = new ParallelCommandGroup(command1, command2);
    ICommand dontRunWhenDisabled = new ParallelCommandGroup(command3, command4);

    scheduler.scheduleCommand(runWhenDisabled, true);
    scheduler.scheduleCommand(runWhenDisabled, true);

    sim.setEnabled(false);
    sim.notifyNewData();
    DriverStation.getInstance().isNewControlData();
    while (DriverStation.getInstance().isEnabled()) {
    }

    assertTrue(scheduler.isScheduled(runWhenDisabled));
    assertFalse(scheduler.isScheduled(dontRunWhenDisabled));

    sim.setEnabled(true);
    sim.notifyNewData();
    DriverStation.getInstance().isNewControlData();
    while (!DriverStation.getInstance().isEnabled()) {
    }
  }
}
