/*
 * Example of a subsystem composed only of a goal wrapped in its
 * implementer class to achieve that goal.
 * 
 * Note that this is a simple contrived example based on a PID controller.
 * There are better ways to do PID controller entirely within a subsystem.
 * Rarely would a resource need to escape any perceived limitations of the
 * command based subsystem architecture and this is no exception.
 * 
 * This example uses the Xbox right trigger (injected from RobotContainer)
 * to set a goal of a position on the color wheel displayed on the LEDs.
 * The PID is tuned to slowly converge on the requested color.
 */
package frc.robot.subsystems;

import java.util.function.DoubleSupplier;

import edu.wpi.first.math.MathUtil;
import edu.wpi.first.math.controller.PIDController;
import edu.wpi.first.wpilibj2.command.Command;
import edu.wpi.first.wpilibj2.command.Commands;
import edu.wpi.first.wpilibj2.command.SubsystemBase;
import edu.wpi.first.wpilibj2.command.button.Trigger;
import frc.robot.Color;
import frc.robot.LEDPattern;
import frc.robot.subsystems.RobotSignals.LEDView;

public class AchieveHueGoal {
    
    // PID intialization
    private final double kP = 0.05;
    private final double kI = 0.0;
    private final double kD = 0.0;
    private PIDController HueController = new PIDController(kP, kI, kD);
    private double minimumHue = 0.;
    private double maximumHue = 180.;
    private double hueSetpoint = 0.;
    private double currentStateHue = 0.;
    private DoubleSupplier defaultHueGoal;
    
    private LEDView robotSignals; // where the output is displayed

    private HueGoal hueGoal = new HueGoal(); // subsystem protected goal

    public AchieveHueGoal(LEDView robotSignals, DoubleSupplier defaultHueGoal) {
        this.defaultHueGoal = defaultHueGoal;
        this.robotSignals = robotSignals;
    }

    // Example of methods and triggers that the system will require are put here.
    //
    // Methods that change the system should be "private".
    // Methods and triggers that inquire about the system should be "publc".

    // this particular state inquiry is an example only and isn't used for the demonstration
    public final Trigger atHueGoal= new Trigger(this::isAtHueGoal);
    private boolean isAtHueGoal() {

        return HueController.atSetpoint();
    }

    public void beforeCommands() {}

    public void afterCommands() {
        currentStateHue = MathUtil.clamp(currentStateHue + HueController.calculate(currentStateHue, hueSetpoint), minimumHue, maximumHue);
        LEDPattern persistentPatternDemo = LEDPattern.solid(Color.fromHSV((int)currentStateHue, 200, 200));// display state;
        robotSignals.setSignal(persistentPatternDemo).schedule(); // access to the LEDS is only by command so do it that way.
    }

    /**
     * subsystem to lock the resource if a command is running and provide a default command
     * Note that the technique of restricting use of the default command is not implemented
     * in this example. That is insecure and anybody can change it that has access to the
     * instance.
     */
    public class HueGoal extends SubsystemBase {

        private HueGoal() {
        
            Command defaultCommand = 
                    Commands.run( ()-> hueSetpoint = defaultHueGoal.getAsDouble() , this);
            setDefaultCommand(defaultCommand);
        }

        /**
         * Set the goal.
         * 
         * These commands could be used situationally (one-shot) instead of a "perpetually"
         * running default command with its pre-determined supplier of the hue goal.
         * @param goal hue 0 to 180
         * @return command that can be used to set the goal
         */
        public Command setHueGoal(double goal) {

            return run(()-> hueSetpoint = goal);
        }

        public Command setHueGoal(DoubleSupplier goal) {

            return run(()-> hueSetpoint = goal.getAsDouble());
        }   
    }
}
