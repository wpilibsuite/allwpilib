package frc.robot.subsystems;

/*
 * This is a FSM that depends on the current state, the transition event trigger, and the historical previous states.
 * 
 * This demonstrates a command that calls another command upon completion by setting up a trigger.
 * This demonstrates accessing historical data to determine the next state.
 * This demonstrates using persistent data to periodically refresh outputs past the completion of this command.
 * This demonstrates running a command periodically.
 * 
 * Caution - this is a simple, contrived example.
 * This command essentially runs periodically the hard way using memory of a time and a trigger on the time.
 * There may be better ways to do that simple scheduling structure, for example, the use of the "afterCommands()"
 * periodic as used for a sub-purpose here is a good way.
 */

import java.util.Arrays;
import java.util.Random;

import edu.wpi.first.wpilibj2.command.Command;
import edu.wpi.first.wpilibj2.command.SubsystemBase;
import edu.wpi.first.wpilibj2.command.button.CommandXboxController;
import edu.wpi.first.wpilibj2.command.button.Trigger;
import frc.robot.Color;
import frc.robot.LEDPattern;
import frc.robot.subsystems.RobotSignals.LEDView;

public class HistoryFSM extends SubsystemBase {

    private final LEDView robotSignals;
    Random rand = new Random();
    
    // Periodic output variable used for each run of "afterCommands()"
    LEDPattern persistentPatternDemo = LEDPattern.solid(Color.kBlack); // this is used before command to set it is run so start with LEDs off

    //    Add a color [hue number as subscript] and last time used to the history
    //  so that color isn't used again during a lockout period.

    //    Make the history in as narrow of scope as possible. For this simple example the scope is perfectly narrow
    //  (this instance scope) since the history doesn't depend on any values from other subsystems.

    //    Also saved from historical values are the "current" color so it persists through multiple iterations.

    //    Time data is saved for how long a color is to persist in the display.

    long[] lastTimeHistoryOfColors = new long[180];

    Color persistentCurrentColor = new Color(); // color to be seen that we want to persist through multiple iterations
    long nextTime = Long.MAX_VALUE; // initialize so the time doesn't trigger anything until the "Y" button is pressed
    private Trigger timeOfNewColor = new Trigger(this::timesUp);

    public HistoryFSM(LEDView robotSignals, CommandXboxController operatorController) {
        this.robotSignals = robotSignals;

        Arrays.fill(lastTimeHistoryOfColors, 0); // initially indicate color hasn't been used since a long time ago

        // Trigger if it's time for a new color or the operator pressed their "Y" button
        timeOfNewColor.or(operatorController.y().debounce(.04)).onTrue(runOnce(this::getHSV)/*.ignoringDisable(true)*/);
    }
    /**
     * 
     * @return has time elapsed
     */
    private boolean timesUp() {

        if( System.currentTimeMillis() >= nextTime) {
            nextTime = Long.MAX_VALUE; // reset; if a command is running it'll set the right time. If it isn't running then
                                       // wait for "Y" press
            // this locks-out automatic restarting on disable to enable change; "Y" must be pressed to get it started again.
            return true;
        }
        return false; // not time to trigger yet
    }

    /**
     * this command sets a color and quits immediately assuming the color persists
     * somehow (in "persistentCurrentColor") until the next color is later requested.
     * 
     * <p>Set a random color that hasn't been used in the last "colorLockoutPeriod"
     */
    public void getHSV() {

        long currentTime = System.currentTimeMillis();
        long changeColorPeriod = 2_000; // milliseconds
        long colorLockoutPeriod = 20_000; // milliseconds
        nextTime = currentTime + changeColorPeriod; // this method sets up the time for the trigger of its next run
        int randomHue; // to be the next color
        int loopCounter = 1; // count attempts to find a different hue
        int loopCounterLimit = 100; // limit attempts to find a different hue

        do {
            // Generate random integers in range 0 to 180 (a computer color wheel)
            randomHue = rand.nextInt(180);
            // if hue hasn't been used recently, then use it now and update its history
            if(lastTimeHistoryOfColors[randomHue] < currentTime - colorLockoutPeriod) {
                lastTimeHistoryOfColors[randomHue] = currentTime;
                break;
            }
        // hue used recently so loop to get another hue
        // limit attempts - no infinite loops allowed
        } while (loopCounter++ < loopCounterLimit);

        persistentPatternDemo = LEDPattern.solid(Color.fromHSV(randomHue, 200, 200));
    }

    /**
     * Disallow default command
     * This prevents accidentally assuming the default command will run in composite commands which it wont.
     * Or "disjointSequence()" could be used. Default command not used in this example.
     */
    @Override
    public void setDefaultCommand(Command def) {

        throw new IllegalArgumentException("Default Command not allowed");
      }

    /**
     * beforeCommands() and afterCommands() run from the Robot.periodic()
     * via RobotContainer and are run before and after Commands and Triggers are run.
     */
    public void beforeCommands() {}

    public void afterCommands() {

        // Set and refresh the color could be done many ways:
        // here which is called periodically through Robot.periodic(),
        // where the data were created,
        // the default command,
        // or the device that receives the data may keep the last value alive.
        // Being done here for illustrative purposes.

        robotSignals.setSignal(persistentPatternDemo).schedule(); // access to the LEDS is only by command so do it that way.
        // Note that because this method runs in disabled mode, the color persists in Disabled mode even if the command was
        // not to run in disabled mode.
        // Could check here for that and black out if necessary. Or do something in disabledInit().

        // Thus, we end up demonstrating how to run a command periodically (as a minor part of the bigger picture).
        // This usage within a subsystem is NOT (maybe) the same as a command directly scheduling another command;
        // that technique should be avoided.
    }
}
