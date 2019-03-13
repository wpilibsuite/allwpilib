package edu.wpi.first.wpilibj.command;

import edu.wpi.first.wpilibj.Timer;

import java.util.ArrayList;
import java.util.List;
import java.util.function.Consumer;

public class CommandState {

  /**
   * The time since this command was initialized.
   */
  private double m_startTime = -1;

  /**
   * Whether or not it is interruptible.
   */
  private boolean m_interruptible;

  private List<Consumer<Command>> m_initActions = new ArrayList<>();
  private List<Consumer<Command>> m_executeActions = new ArrayList<>();
  private List<Consumer<Command>> m_interruptActions = new ArrayList<>();
  private List<Consumer<Command>> m_endActions = new ArrayList<>();

  public CommandState(boolean interruptible) {
    m_interruptible = interruptible;
    startTiming();
    startRunning();
  }

  private void startTiming() {
    m_startTime = Timer.getFPGATimestamp();
  }

  synchronized void startRunning() {
    m_startTime = -1;
  }

  boolean isInterruptible() {
    return m_interruptible;
  }

  double timeSinceInitialized() {
    return Timer.getFPGATimestamp() - m_startTime;
  }
}
