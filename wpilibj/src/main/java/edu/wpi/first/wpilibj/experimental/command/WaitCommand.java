package edu.wpi.first.wpilibj.experimental.command;

import edu.wpi.first.wpilibj.Timer;

public class WaitCommand extends SendableCommandBase {
  Timer m_timer = new Timer();
  final double m_duration;

  /**
   * Creates a new WaitCommand.  This command will do nothing, and end after the specified duration.
   *
   * @param seconds the time to wait, in seconds
   */
  public WaitCommand(double seconds) {
    m_duration = seconds;
  }

  @Override
  public void initialize() {
    m_timer.start();
  }

  @Override
  public void execute() {

  }

  @Override
  public void end() {
    m_timer.stop();
    m_timer.reset();
  }

  @Override
  public boolean isFinished() {
    return m_timer.hasPeriodPassed(m_duration);
  }

  @Override
  public boolean runsWhenDisabled() {
    return true;
  }
}
