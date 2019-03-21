package edu.wpi.first.wpilibj.experimental.command;

/**
 * A Command that runs instantly; it will initialize, execute once, and end on the same
 * iteration of the scheduler.  Users can either pass in a Runnable and a set of requirements,
 * or else subclass this command if desired.
 */
public class InstantCommand extends SendableCommandBase {

  private final Runnable m_runnable;

  /**
   * Creates a new InstantCommand that runs the given Runnable with the given requirements.
   *
   * @param runnable     the Runnable to run
   * @param requirements the subsystems required by this command
   */
  public InstantCommand(Runnable runnable, Subsystem... requirements) {
    m_runnable = runnable;
    addRequirements(requirements);
  }

  /**
   * Creates a new InstantCommand that runs the given Runnable with no requirements.
   *
   * @param runnable the Runnable to run
   */
  public InstantCommand(Runnable runnable) {
    this(runnable, new Subsystem[0]);
  }

  /**
   * Creates a new InstantCommand with a Runnable that does nothing.  Useful only as a no-arg
   * constructor to call from subclass constructors.
   */
  public InstantCommand() {
    //let's all laugh really hard at the style checker...
    m_runnable = () -> {
    };
  }

  @Override
  public void initialize() {
    m_runnable.run();
  }

  @Override
  public final boolean isFinished() {
    return true;
  }
}
