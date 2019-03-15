package edu.wpi.first.wpilibj.experimental.command;

import org.junit.jupiter.api.Test;

import edu.wpi.first.hal.sim.DriverStationSim;
import edu.wpi.first.wpilibj.DriverStation;

import static org.junit.jupiter.api.Assertions.assertFalse;
import static org.junit.jupiter.api.Assertions.assertTrue;

public class RobotDisabledCommandTest extends CommandTestBase {
  @Test
  void robotDisabledCommandCancelTest() {
    DriverStationSim sim = new DriverStationSim();
    sim.setDsAttached(true);

    CommandScheduler scheduler = new CommandScheduler();

    MockCommandHolder holder = new MockCommandHolder(false, new Subsystem[0]);
    Command mockCommand = holder.getMock();

    scheduler.scheduleCommand(mockCommand, true);

    assertTrue(scheduler.isScheduled(mockCommand));

    sim.setEnabled(false);
    sim.notifyNewData();
    DriverStation.getInstance().isNewControlData();
    while (DriverStation.getInstance().isEnabled()) {
      try {
        Thread.sleep(1);
      } catch (InterruptedException exception) {
        exception.printStackTrace();
      }
    }

    scheduler.run();

    assertFalse(scheduler.isScheduled(mockCommand));

    sim.setEnabled(true);
    sim.notifyNewData();
    DriverStation.getInstance().isNewControlData();
    while (!DriverStation.getInstance().isEnabled()) {
      try {
        Thread.sleep(1);
      } catch (InterruptedException exception) {
        exception.printStackTrace();
      }
    }
  }

  @Test
  void runWhenDisabledTest() {
    DriverStationSim sim = new DriverStationSim();
    sim.setDsAttached(true);

    CommandScheduler scheduler = new CommandScheduler();

    MockCommandHolder holder = new MockCommandHolder(true, new Subsystem[0]);
    Command mockCommand = holder.getMock();

    scheduler.scheduleCommand(mockCommand, true);

    assertTrue(scheduler.isScheduled(mockCommand));

    sim.setEnabled(false);
    sim.notifyNewData();
    DriverStation.getInstance().isNewControlData();
    while (DriverStation.getInstance().isEnabled()) {
      try {
        Thread.sleep(1);
      } catch (InterruptedException exception) {
        exception.printStackTrace();
      }
    }

    scheduler.run();

    assertTrue(scheduler.isScheduled(mockCommand));

    sim.setEnabled(true);
    sim.notifyNewData();
    DriverStation.getInstance().isNewControlData();
    while (!DriverStation.getInstance().isEnabled()) {
      try {
        Thread.sleep(1);
      } catch (InterruptedException exception) {
        exception.printStackTrace();
      }
    }
  }

  @Test
  void sequentialGroupRunWhenDisabledTest() {
    DriverStationSim sim = new DriverStationSim();
    sim.setDsAttached(true);

    CommandScheduler scheduler = new CommandScheduler();

    MockCommandHolder command1Holder = new MockCommandHolder(true, new Subsystem[0]);
    Command command1 = command1Holder.getMock();
    MockCommandHolder command2Holder = new MockCommandHolder(true, new Subsystem[0]);
    Command command2 = command2Holder.getMock();
    MockCommandHolder command3Holder = new MockCommandHolder(true, new Subsystem[0]);
    Command command3 = command3Holder.getMock();
    MockCommandHolder command4Holder = new MockCommandHolder(false, new Subsystem[0]);
    Command command4 = command4Holder.getMock();

    Command runWhenDisabled = new SequentialCommandGroup(command1, command2);
    Command dontRunWhenDisabled = new SequentialCommandGroup(command3, command4);

    scheduler.scheduleCommand(runWhenDisabled, true);
    scheduler.scheduleCommand(dontRunWhenDisabled, true);

    sim.setEnabled(false);
    sim.notifyNewData();
    DriverStation.getInstance().isNewControlData();
    while (DriverStation.getInstance().isEnabled()) {
      try {
        Thread.sleep(1);
      } catch (InterruptedException exception) {
        exception.printStackTrace();
      }
    }

    scheduler.run();

    assertTrue(scheduler.isScheduled(runWhenDisabled));
    assertFalse(scheduler.isScheduled(dontRunWhenDisabled));

    sim.setEnabled(true);
    sim.notifyNewData();
    DriverStation.getInstance().isNewControlData();
    while (!DriverStation.getInstance().isEnabled()) {
      try {
        Thread.sleep(1);
      } catch (InterruptedException exception) {
        exception.printStackTrace();
      }
    }
  }

  @Test
  void parallelGroupRunWhenDisabledTest() {
    DriverStationSim sim = new DriverStationSim();
    sim.setDsAttached(true);

    CommandScheduler scheduler = new CommandScheduler();

    MockCommandHolder command1Holder = new MockCommandHolder(true, new Subsystem[0]);
    Command command1 = command1Holder.getMock();
    MockCommandHolder command2Holder = new MockCommandHolder(true, new Subsystem[0]);
    Command command2 = command2Holder.getMock();
    MockCommandHolder command3Holder = new MockCommandHolder(true, new Subsystem[0]);
    Command command3 = command3Holder.getMock();
    MockCommandHolder command4Holder = new MockCommandHolder(false, new Subsystem[0]);
    Command command4 = command4Holder.getMock();

    Command runWhenDisabled = new ParallelCommandGroup(command1, command2);
    Command dontRunWhenDisabled = new ParallelCommandGroup(command3, command4);

    scheduler.scheduleCommand(runWhenDisabled, true);
    scheduler.scheduleCommand(dontRunWhenDisabled, true);

    sim.setEnabled(false);
    sim.notifyNewData();
    DriverStation.getInstance().isNewControlData();
    while (DriverStation.getInstance().isEnabled()) {
      try {
        Thread.sleep(1);
      } catch (InterruptedException exception) {
        exception.printStackTrace();
      }
    }

    scheduler.run();

    assertTrue(scheduler.isScheduled(runWhenDisabled));
    assertFalse(scheduler.isScheduled(dontRunWhenDisabled));

    sim.setEnabled(true);
    sim.notifyNewData();
    DriverStation.getInstance().isNewControlData();
    while (!DriverStation.getInstance().isEnabled()) {
      try {
        Thread.sleep(1);
      } catch (InterruptedException exception) {
        exception.printStackTrace();
      }
    }
  }
}
