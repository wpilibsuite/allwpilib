package $package.subsystems;

import $package.Robot;

import edu.wpi.first.wpilibj.*;
import edu.wpi.first.wpilibj.command.PIDSubsystem;
import edu.wpi.first.wpilibj.interfaces.Potentiometer;
import edu.wpi.first.wpilibj.livewindow.LiveWindow;

/**
 * The Pivot subsystem contains the Van-door motor and the pot for PID control
 * of angle of the pivot and claw.
 */
public class Pivot extends PIDSubsystem {
	// Constants for some useful angles 
	public static final double COLLECT = 105;
	public static final double LOW_GOAL = 90;
	public static final double SHOOT = 45;
	public static final double SHOOT_NEAR = 30;
	
	// Subsystem devices
	private DigitalInput upperLimitSwitch;
	private DigitalInput lowerLimitSwitch;
	private Potentiometer pot;
	private SpeedController motor;

	public Pivot() {
		super("Pivot", 7.0, 0.0, 8.0);
		setAbsoluteTolerance(0.005);
		getPIDController().setContinuous(false);
		if (Robot.isSimulation()) { // PID is different in simulation.
			getPIDController().setPID(0.5, 0.001, 2);
			setAbsoluteTolerance(5);
		}
		
		// Motor to move the pivot.
		motor = new Victor(5);
		
		// Sensors for measuring the position of the pivot.
		upperLimitSwitch = new DigitalInput(13);
		lowerLimitSwitch = new DigitalInput(12);
		
		// 0 degrees is vertical facing up.
		// Angle increases the more forward the pivot goes.
		pot = new AnalogPotentiometer(1); 

		// Put everything to the LiveWindow for testing.
		LiveWindow.addSensor("Pivot", "Upper Limit Switch", upperLimitSwitch);
		LiveWindow.addSensor("Pivot", "Lower Limit Switch", lowerLimitSwitch);
		LiveWindow.addSensor("Pivot", "Pot", (AnalogPotentiometer) pot);
		LiveWindow.addActuator("Pivot", "Motor", (Victor) motor);
		LiveWindow.addActuator("Pivot", "PIDSubsystem Controller", getPIDController());
	}

	/**
	 *  No default command, if PID is enabled, the current setpoint will be maintained.
	 */
	public void initDefaultCommand() {}

	/**
	 * @return The angle read in by the potentiometer 
	 */
	protected double returnPIDInput() {
		return pot.get();
	}

	/**
	 * Set the motor speed based off of the PID output
	 */
	protected void usePIDOutput(double output) {
		motor.pidWrite(output);
	}

	/**
	 * @return If the pivot is at its upper limit.
	 */
	public boolean isAtUpperLimit() {
		return upperLimitSwitch.get(); // TODO: inverted from real robot (prefix with !)
	}

	/**
	 * @return If the pivot is at its lower limit.
	 */
	public boolean isAtLowerLimit() {
		return lowerLimitSwitch.get(); // TODO: inverted from real robot (prefix with !)
	}

	/**
	 * @return The current angle of the pivot.
	 */
	public double getAngle() {
		return pot.get();
	}
}
