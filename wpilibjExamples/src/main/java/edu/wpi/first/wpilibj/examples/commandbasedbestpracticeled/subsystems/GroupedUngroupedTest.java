package frc.robot.subsystems;

import edu.wpi.first.wpilibj2.command.Command;
import edu.wpi.first.wpilibj2.command.SubsystemBase;

public class GroupedUngroupedTest extends SubsystemBase {

/*
 * All Commands factories are "public."
 * 
 * All other methods are "private" to prevent other classes from forgetting to
 * add requirements of these resources if creating commands from these methods.
 */

    public GroupedUngroupedTest () {

      setDefaultCommand(defaultCommand);
    }

    // variables used to produce the periodic output
    private String output = ""; // set for what to output
    private String outputPrevious = ""; // previous output setting used to suppress duplicates
    private int repeatedOutputCount = 0; // number of duplicates
    private static final int repeatedOutputLimit = 250; // 5 seconds worth at 50 Hz loop frequency

    public void beforeCommands() {}

    public void afterCommands() {
      { // process "output" variable
    // processing in periodic I/O should be kept to a minimum to get the best consistent set
    // of I/O. This example is complicated to minimize the large quantity of output possible.

    // Note that using this periodic output scheme - as implemented - causes the last output
    // value to persist through iterative periods if it hasn't changed. This behavior could be
    // changed with a check for stale data. It is problem dependent on what should be done -
    // persist output or no output?

      boolean newOutput = ! output.equals(outputPrevious);

      if ( ! newOutput) {
        repeatedOutputCount++;
      }

      if (newOutput || repeatedOutputCount >= repeatedOutputLimit) {
        if (repeatedOutputCount > 1) {
          System.out.println(" --- " + repeatedOutputCount + " times");
          repeatedOutputCount = 0;
        }
        else {
          System.out.println();
        }
        System.out.print(output);
      }

      outputPrevious = output;
      } // end process "output" variable

    }
/* output of sequential group test commands

Ungrouped sequential commands have their default command run when they are not active
Compare that to the normal sequential commands that have the default command blocked during a composed command run.

Warning at edu.wpi.first.wpilibj.DriverStation.reportJoystickUnpluggedWarning(DriverStation.java:1364): Joystick Button 2 on port 0 not available, check if controller is plugged in
 --- 250 times
Warning at edu.wpi.first.wpilibj.DriverStation.reportJoystickUnpluggedWarning(DriverStation.java:1364): Joystick Button 2 on port 0 not available, check if controller is plugged in
 --- 29 times
default command --- 250 times
default command --- 247 times
[Note that the default command is allowed to run between commands (during the waitSeconds) if using the "ungroupedSequence()".]
testing 1
default command --- 4 times
testing 2
default command --- 6 times
testing 3
default command --- 84 times
[Note that using the "sequence()" (not "ungroupedSequence()") the default command does not run between commands.]
[The excessive testing prints for 4 and 5 is the "afterPeriodic()" writing persistent output variable after the command completed.]
testing 4 --- 4 times
testing 5 --- 5 times
testing 6
default command --- 250 times
default command --- 250 times
default command --- 250 times
default command
*/

    /*
     * Public Commands
     */

    /**
     * Recommendation is don't use the setDefaultCommand because default commands are not
     * run inside composed commands. Using "ungroupedSequence()" can mitigate this problem
     * since it allows the default command to run when individual commands end.
     * If you insist on a default command then recommendation is don't use more than one default
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

    private final Command defaultCommand = run(()->{
      output = "default command";
      });
    // private final Command defaultCommand = run(()->System.out.println("default command"));

    public final Command setTest(int testNumber) {

        return runOnce(()->{
          output = "testing " + testNumber;
        });
    }

    // note that the Commands.print("testing " + testNumber) does not require a subsystem which
    // is needed for this test so System.out.print() was used more directly.
    // And related note that Command factories cannot be "static" since they require the subsystem
    // instance ("this"; implied in this example by the runOnce() command).
}