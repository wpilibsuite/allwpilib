package edu.wpi.first.wpilibj.experimental.command;

import java.util.Set;

import org.junit.jupiter.api.BeforeEach;

import static org.mockito.Mockito.mock;
import static org.mockito.Mockito.when;

/**
 * Basic setup for all {@link Command tests}.
 */
@SuppressWarnings("PMD.AbstractClassWithoutAbstractMethod")
public abstract class CommandTestBase {

  @BeforeEach
  void commandSetup() {
    CommandScheduler.getInstance().cancelAll();
    CommandScheduler.getInstance().enable();
    CommandScheduler.getInstance().clearButtons();
    CommandGroupBase.clearGroupedCommands();
  }

  class TestSubsystem extends SendableSubsystemBase {

  }

  protected class MockCommandHolder {

    private final Command m_mockCommand = mock(Command.class);

    MockCommandHolder(boolean runWhenDisabled, Subsystem... requirements) {
      when(m_mockCommand.getRequirements()).thenReturn(Set.of(requirements));
      when(m_mockCommand.isFinished()).thenReturn(false);
      when(m_mockCommand.runsWhenDisabled()).thenReturn(runWhenDisabled);
    }

    Command getMock() {
      return m_mockCommand;
    }

    void setFinished(boolean finished) {
      when(m_mockCommand.isFinished()).thenReturn(finished);
    }

  }

  protected class Counter {
    int m_counter;

    void increment() {
      m_counter++;
    }
  }

  protected class ConditionHolder {
    private boolean m_condition;

    void setCondition(boolean condition) {
      m_condition = condition;
    }

    boolean getCondition() {
      return m_condition;
    }
  }
}
