package edu.wpi.first.wpilibj.command;

import java.util.Set;

import org.junit.jupiter.api.BeforeEach;

import static org.junit.jupiter.api.Assertions.assertDoesNotThrow;
import static org.mockito.Mockito.mock;
import static org.mockito.Mockito.when;

/**
 * Basic setup for all {@link ICommand tests}.
 */
public abstract class ICommandTestBase {

  @BeforeEach
  void commandSetup() {
    SchedulerNew.getInstance().cancelAll();
    Scheduler.getInstance().enable();
  }

  public class ASubsystem extends Subsystem {
    ICommand m_command;

    @Override
    protected void initDefaultCommand() {
    }

    public void init(ICommand command) {
      m_command = command;
      setDefaultICommand(m_command);
    }
  }

  protected class MockCommandHolder {

    private ICommand m_mockCommand = mock(ICommand.class);

    MockCommandHolder(boolean runWhenDisabled, Subsystem... requirements) {
      when(m_mockCommand.getRequirements()).thenReturn(Set.of(requirements));
      when(m_mockCommand.isFinished()).thenReturn(false);
      when(m_mockCommand.getRunWhenDisabled()).thenReturn(runWhenDisabled);
    }

    ICommand getMock() {
      return m_mockCommand;
    }

    void setFinished(boolean finished) {
      when(m_mockCommand.isFinished()).thenReturn(finished);
    }

  }

  protected void sleep(int time) {
    assertDoesNotThrow(() -> Thread.sleep(time));
  }


}
