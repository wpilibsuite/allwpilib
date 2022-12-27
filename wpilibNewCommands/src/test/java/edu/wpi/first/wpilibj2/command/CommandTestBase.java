// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj2.command;

import static org.mockito.Mockito.*;

import edu.wpi.first.wpilibj.DriverStation;
import edu.wpi.first.wpilibj.simulation.DriverStationSim;
import edu.wpi.first.wpilibj2.command.Command.InterruptionBehavior;
import java.util.Set;
import org.junit.jupiter.api.AfterEach;
import org.junit.jupiter.api.BeforeEach;
import org.junit.jupiter.api.parallel.ResourceLock;

/** Basic setup for all {@link Command tests}. */
@ResourceLock("CommandScheduler")
public class CommandTestBase {
  protected CommandTestBase() {}

  @BeforeEach
  void commandSetup() {
    CommandScheduler.getInstance().cancelAll();
    CommandScheduler.getInstance().enable();
    CommandScheduler.getInstance().getActiveButtonLoop().clear();
    CommandScheduler.getInstance().clearComposedCommands();

    setDSEnabled(true);
  }

  @AfterEach
  void commandCleanup() {
    CommandScheduler.getInstance().cancelAll();
    CommandScheduler.getInstance().disable();
    CommandScheduler.getInstance().getActiveButtonLoop().clear();
    CommandScheduler.getInstance().clearComposedCommands();
    CommandScheduler.getInstance().close();
  }

  public void setDSEnabled(boolean enabled) {
    DriverStationSim.setDsAttached(true);

    DriverStationSim.setEnabled(enabled);
    DriverStationSim.notifyNewData();
    while (DriverStation.isEnabled() != enabled) {
      try {
        Thread.sleep(1);
      } catch (InterruptedException exception) {
        exception.printStackTrace();
      }
    }
  }

  public static class MockCommandHolder {
    private final Command m_mockCommand = mock(Command.class);

    public MockCommandHolder(boolean runWhenDisabled, Subsystem... requirements) {
      when(m_mockCommand.getRequirements()).thenReturn(Set.of(requirements));
      when(m_mockCommand.isFinished()).thenReturn(false);
      when(m_mockCommand.runsWhenDisabled()).thenReturn(runWhenDisabled);
      when(m_mockCommand.getInterruptionBehavior()).thenReturn(InterruptionBehavior.kCancelSelf);
      doCallRealMethod().when(m_mockCommand).schedule();
      when(m_mockCommand.isScheduled()).thenCallRealMethod();
      doCallRealMethod().when(m_mockCommand).cancel();
    }

    public Command getMock() {
      return m_mockCommand;
    }

    public void setFinished(boolean finished) {
      when(m_mockCommand.isFinished()).thenReturn(finished);
    }
  }
}
