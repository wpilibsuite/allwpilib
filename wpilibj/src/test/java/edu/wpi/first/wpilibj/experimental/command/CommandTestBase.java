package edu.wpi.first.wpilibj.experimental.command;

import java.util.Set;

import org.junit.jupiter.api.BeforeEach;

import static org.junit.jupiter.api.Assertions.assertDoesNotThrow;
import static org.mockito.Mockito.mock;
import static org.mockito.Mockito.when;

/**
 * Basic setup for all {@link Command tests}.
 */
public abstract class CommandTestBase {

  @BeforeEach
  void commandSetup() {
    CommandScheduler.getInstance().cancelAll();
    CommandScheduler.getInstance().enable();
  }

  public class ASubsystem implements Subsystem {
    private Command m_defaultCommand;

    @Override
    public void periodic() {
    }

    @Override
    public Command getDefaultCommand() {
      return m_defaultCommand;
    }

    void init(Command defaultCommand) {
      m_defaultCommand = defaultCommand;
    }
  }

  protected class MockCommandHolder {

    private Command m_mockCommand = mock(Command.class);

    MockCommandHolder(boolean runWhenDisabled, Subsystem... requirements) {
      when(m_mockCommand.getRequirements()).thenReturn(Set.of(requirements));
      when(m_mockCommand.isFinished()).thenReturn(false);
      when(m_mockCommand.getRunWhenDisabled()).thenReturn(runWhenDisabled);
    }

    Command getMock() {
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
