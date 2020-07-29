/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj2.command;

import java.util.Set;

import org.junit.jupiter.api.BeforeEach;

import edu.wpi.first.wpilibj.DriverStation;
import edu.wpi.first.wpilibj.simulation.DriverStationSim;

import static org.mockito.Mockito.mock;
import static org.mockito.Mockito.when;

/**
 * Basic setup for all {@link Command tests}."
 */
@SuppressWarnings("PMD.AbstractClassWithoutAbstractMethod")
public abstract class CommandTestBase {
  @BeforeEach
  void commandSetup() {
    CommandScheduler.getInstance().cancelAll();
    CommandScheduler.getInstance().enable();
    CommandScheduler.getInstance().clearButtons();
    CommandGroupBase.clearGroupedCommands();

    setDSEnabled(true);
  }

  public void setDSEnabled(boolean enabled) {
    DriverStationSim.setDsAttached(true);

    DriverStationSim.setEnabled(enabled);
    DriverStationSim.notifyNewData();
    DriverStation.getInstance().isNewControlData();
    while (DriverStation.getInstance().isEnabled() != enabled) {
      try {
        Thread.sleep(1);
      } catch (InterruptedException exception) {
        exception.printStackTrace();
      }
    }
  }

  public class TestSubsystem extends SubsystemBase {
  }

  public class MockCommandHolder {
    private final Command m_mockCommand = mock(Command.class);

    public MockCommandHolder(boolean runWhenDisabled, Subsystem... requirements) {
      when(m_mockCommand.getRequirements()).thenReturn(Set.of(requirements));
      when(m_mockCommand.isFinished()).thenReturn(false);
      when(m_mockCommand.runsWhenDisabled()).thenReturn(runWhenDisabled);
    }

    public Command getMock() {
      return m_mockCommand;
    }

    public void setFinished(boolean finished) {
      when(m_mockCommand.isFinished()).thenReturn(finished);
    }

  }

  public class Counter {
    public int m_counter;

    public void increment() {
      m_counter++;
    }
  }

  public class ConditionHolder {
    private boolean m_condition;

    public void setCondition(boolean condition) {
      m_condition = condition;
    }

    public boolean getCondition() {
      return m_condition;
    }
  }
}
