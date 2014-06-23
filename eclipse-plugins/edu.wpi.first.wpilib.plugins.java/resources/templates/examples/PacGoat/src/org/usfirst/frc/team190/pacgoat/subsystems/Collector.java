package $package.subsystems;

import edu.wpi.first.wpilibj.DigitalInput;
import edu.wpi.first.wpilibj.Solenoid;
import edu.wpi.first.wpilibj.SpeedController;
import edu.wpi.first.wpilibj.Victor;
import edu.wpi.first.wpilibj.command.Subsystem;
import edu.wpi.first.wpilibj.livewindow.LiveWindow;

/**
 * The Collector subsystem has one motor for the rollers, a limit switch for ball
 * detection, a piston for opening and closing the claw, and a reed switch to
 * check if the piston is open.
 */
public class Collector extends Subsystem {
	// Constants for some useful speeds
	public static final double FORWARD = 1;
	public static final double STOP = 0;
	public static final double REVERSE = -1;
	
	// Subsystem devices
	private SpeedController rollerMotor;
	private DigitalInput ballDetector;
	private Solenoid piston;
	private DigitalInput openDetector;

	public Collector() {
		// Configure devices
		rollerMotor = new Victor(6);
		ballDetector = new DigitalInput(10);
		openDetector = new DigitalInput(6);
		piston = new Solenoid(1, 1);
		
		// Put everything to the LiveWindow for testing.
		LiveWindow.addActuator("Collector", "Roller Motor", (Victor) rollerMotor);
		LiveWindow.addSensor("Collector", "Ball Detector", ballDetector);
		LiveWindow.addSensor("Collector", "Claw Open Detector", openDetector);
		LiveWindow.addActuator("Collector", "Piston", piston);
	}

	/**
	 * NOTE: The current simulation model uses the the lower part of the claw
	 * since the limit switch wasn't exported. At some point, this will be
	 * updated.
	 * 
	 * @return Whether or not the robot has the ball.
	 */
	public boolean hasBall() {
		return ballDetector.get(); // TODO: prepend ! to reflect real robot
	}

	/**
	 * @param speed The speed to spin the rollers.
	 */
	public void setSpeed(double speed) {
		rollerMotor.set(-speed);
	}

	/**
	 * Stop the rollers from spinning
	 */
	public void stop() {
		rollerMotor.set(0);
	}

	/**
	 * @return Whether or not the claw is open.
	 */
	public boolean isOpen() {
		return openDetector.get(); // TODO: prepend ! to reflect real robot
	}

	/**
	 * Open the claw up. (For shooting)
	 */
	public void open() {
		piston.set(true);
	}

	/**
	 * Close the claw. (For collecting and driving)
	 */
	public void close() {
		piston.set(false);
	}

	/**
	 * No default command.
	 */
	@Override protected void initDefaultCommand() {}
}
