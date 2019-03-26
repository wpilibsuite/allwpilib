package edu.wpi.first.wpilibj.experimental.command;

/**
 * A Command that runs instantly; it will initialize, execute once, and end on the same
 * iteration of the scheduler.  Users can either pass in a Runnable and a set of requirements,
 * or else subclass this command if desired.
 */
public class InstantCommand extends SendableCommandBase {

  private final Runnable m_toRun;

  /**
   * Creates a new InstantCommand that runs the given Runnable with the given requirements.
   *
   * @param toRun        the Runnable to run
   * @param requirements the subsystems required by this command
   */
  public InstantCommand(Runnable toRun, Subsystem... requirements) {
    m_toRun = toRun;
    addRequirements(requirements);
  }

  /**
   * Creates a new InstantCommand with a Runnable that does nothing.  Useful only as a no-arg
   * constructor to call from subclass constructors.
   */
  public InstantCommand() {
    //let's all laugh really hard at the style checker...
    m_toRun = () -> {
    };
  }

  @Override
  public void initialize() {
    m_toRun.run();
  }

  @Override
  public final boolean isFinished() {
    return true;
  }
}
