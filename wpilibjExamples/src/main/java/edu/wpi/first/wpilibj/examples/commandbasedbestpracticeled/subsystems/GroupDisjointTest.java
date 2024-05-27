package frc.robot.subsystems;

// General purpose class to create test commands.

// The class constructor accepts a parameter for the resource (subsystem) ID.

// The Command factory "testDuration" accepts parameters for the test number
// and the duration the test command should run. (Note that is a "history" example.)

// The default command is "built-in" so changes must be recoded in the class.

import edu.wpi.first.wpilibj.Timer;
import edu.wpi.first.wpilibj2.command.Command;
import edu.wpi.first.wpilibj2.command.SubsystemBase;

public class GroupDisjointTest extends SubsystemBase {

/*
 * All Commands factories are "public."
 * 
 * All other methods are "private" to prevent other classes from forgetting to
 * add requirements of these resources if creating commands from these methods.
 */

    private String resourceID = "X";
    // variables used to produce the periodic output
    private String output = ""; // set for what to output
    private boolean outputFresh = false; // periodically checking but sometimes there is no new data
    private String outputPrevious = ""; // previous output setting used to suppress duplicates
    private int repeatedOutputCount = 0; // number of duplicates
    private static int repeatedOutputLimit = 250; // 5 seconds worth at 50 Hz loop frequency

    public GroupDisjointTest (String resourceID) {
      this.resourceID = resourceID;
    }

    public void beforeCommands() {}

    public void afterCommands() {
    // processing in periodic I/O should be kept to a minimum to get the best consistent set
    // of I/O. This example is complicated to minimize the large quantity of output possible.

      // output - compressed; persistent values even stale ones
      boolean compressOutput = false;
      if (compressOutput)
      { // process "output" variable

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
      } // end output compression
      else       
      if (outputFresh) {// output - everything; only new values (duplicate values output, too, but only refreshed ones)
        outputFresh = false;
        System.out.print(output);
      }
    }

    /*
     * Public Commands
     */

    /**
     * Recommendation is don't use the setDefaultCommand because default commands are not
     * run inside composed commands. Using "disjointSequence", etc. (ProxyCommands) can
     * mitigate this problem since it allows the default command to run when individual
     * commands end.
     * If you use a default command then recommendation is don't use more than one default
     * because it may not be obvious which default command is active (last one set is active).
     * 
     * Example code below to allow no more than one call to this set default command of the
     * resource/subsystem.
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

    public void setDefaultCommand() {
      setDefaultCommand(defaultCommand);
    }

    private final Command defaultCommand = run(()->{
      output = resourceID + "d";
      outputFresh = true;
      });

    /**
     * Command Factory for GroupDisjointTest subsystems
     * 
     * @param testNumber output this number and the resource (subsystem) ID
     * @param testDuration seconds to run execute() to produce output
     * @return
     */
    public Command testDuration(int testNumber, double testDuration) {

      return new TestDuration(testNumber, testDuration);
    }

    private class TestDuration extends Command {

      private int testNumber;
      private double testDuration;
      Timer endTime = new Timer(); // need a memory of time; put in the narrowest scope
                                   // wouldn't have to have quite so large of command template code if
                                   // time history scope was put in RobotContainer but then it's hard to
                                   // manage since each subsystem instance would need a manually coded
                                   // time history variable in RobotContainer while here it's automatic

      private TestDuration(int testNumber, double testDuration) {

        this.testNumber = testNumber;
        this.testDuration = testDuration;
        addRequirements(GroupDisjointTest.this); // requirements are too easy to forget
      }

      @Override
      public void initialize() {

        endTime.restart();
      }

      @Override
      public void execute() {

        output = resourceID + testNumber;
        outputFresh = true;
      }

      @Override
      public boolean isFinished() {

        return endTime.hasElapsed(testDuration);
      }
    }
}
// note that the Commands.print("testing " + testNumber) does not require a subsystem which
// is needed for this test so System.out.print() was used more directly.
// And related note that Command factories cannot be "static" since they require the subsystem
// instance ("this"; implied in this example by the runOnce() command).