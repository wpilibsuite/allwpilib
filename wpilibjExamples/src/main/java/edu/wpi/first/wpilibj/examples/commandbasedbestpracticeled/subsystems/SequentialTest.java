package frc.robot.subsystems;

import edu.wpi.first.wpilibj2.command.Command;
import edu.wpi.first.wpilibj2.command.SubsystemBase;

public class SequentialTest extends SubsystemBase{


/*
 * All Commands factories are "public."
 * 
 * All other methods are "private" to prevent other classes from forgetting to
 * add requirements of these resources if creating commands from these methods.
 */

    public SequentialTest () {

      setDefaultCommand(defaultCommand);
    }

    public void beforeCommands() {}

    public void afterCommands() {}

    /*
     * Public Commands
     */

    /**
     * Recommendation is don't use the setDefaultCommand because default commands are not
     * run inside composed commands.
     * If you insist then recommendation is don't use more than one default command
     * because it may not be obvious which default command is active (last one set is active)
     * Allow no more than one call to this set of the view (resource, subsystem) default command
     * You're on your own to remember if there is a default command set or not.
     */
    @Override
    public void setDefaultCommand(Command def) {

      if (getDefaultCommand() != null) {
        throw new IllegalArgumentException("Default Command already set");
      }

      if(def != null) {
        super.setDefaultCommand(def);
      }
    }

    private final Command defaultCommand = run(()->System.out.println("default command"));

    public final Command setTest(int testNumber) {

        return runOnce(()->System.out.println("testing " + testNumber));
    }

    // note that the Commands.print("testing " + testNumber) does not require a subsystem
    // Note that Command factories cannot be "static" since they require the subsystem instance ("this")
}