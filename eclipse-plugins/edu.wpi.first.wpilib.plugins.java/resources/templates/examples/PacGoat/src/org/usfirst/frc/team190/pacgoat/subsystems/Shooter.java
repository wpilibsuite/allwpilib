package $package.subsystems;

import edu.wpi.first.wpilibj.DigitalInput;
import edu.wpi.first.wpilibj.DoubleSolenoid;
import edu.wpi.first.wpilibj.Solenoid;
import edu.wpi.first.wpilibj.command.Subsystem;
import edu.wpi.first.wpilibj.livewindow.LiveWindow;

/**
 * The Shooter subsystem handles shooting. The mechanism for shooting is
 * slightly complicated because it has to pneumatic cylinders for shooting, and
 * a third latch to allow the pressure to partially build up and reduce the
 * effect of the airflow. For shorter shots, when full power isn't needed, only
 * one cylinder fires.
 * 
 * NOTE: Simulation currently approximates this as as single pneumatic cylinder
 * and ignores the latch.
 */
public class Shooter extends Subsystem {
	// Devices
	DoubleSolenoid piston1;
	DoubleSolenoid piston2;
	Solenoid latchPiston;
	DigitalInput piston1ReedSwitchFront;
	DigitalInput piston1ReedSwitchBack;
	DigitalInput hotGoalSensor; // NOTE: Currently ignored in simulation

	public Shooter() {
		// Configure Devices
		hotGoalSensor = new DigitalInput(3);
		piston1 = new DoubleSolenoid(1, 3, 4);
		piston2 = new DoubleSolenoid(1, 5, 6);
		latchPiston = new Solenoid(1, 2);
		piston1ReedSwitchFront = new DigitalInput(9);
		piston1ReedSwitchBack = new DigitalInput(11);

		// Put everything to the LiveWindow for testing.
		LiveWindow.addSensor("Shooter", "Hot Goal Sensor", hotGoalSensor);
		LiveWindow.addSensor("Shooter", "Piston1 Reed Switch Front ",
				piston1ReedSwitchFront);
		LiveWindow.addSensor("Shooter", "Piston1 Reed Switch Back ",
				piston1ReedSwitchBack);
		LiveWindow.addActuator("Shooter", "Latch Piston", latchPiston);
	}

	/**
	 * No default command.
	 */
	public void initDefaultCommand() {}

	/** 
	 * Extend both solenoids to shoot.
	 */
	public void extendBoth() {
		piston1.set(DoubleSolenoid.Value.kForward);
		piston2.set(DoubleSolenoid.Value.kForward);
	}

	/**
	 * Retract both solenoids to prepare to shoot.
	 */
	public void retractBoth() {
		piston1.set(DoubleSolenoid.Value.kReverse);
		piston2.set(DoubleSolenoid.Value.kReverse);
	}

	/**
	 * Extend solenoid 1 to shoot.
	 */
	public void extend1() {
		piston1.set(DoubleSolenoid.Value.kForward);
	}

	/**
	 * Retract solenoid 1 to prepare to shoot.
	 */
	public void retract1() {
		piston1.set(DoubleSolenoid.Value.kReverse);
	}

	/**
	 * Extend solenoid 2 to shoot.
	 */
	public void extend2() {
		piston2.set(DoubleSolenoid.Value.kReverse);
	}

	/**
	 * Retract solenoid 2 to prepare to shoot.
	 */
	public void retract2() {
		piston2.set(DoubleSolenoid.Value.kForward);
	}

	/**
	 * Turns off the piston1 double solenoid. This won't actuate anything
	 * because double solenoids preserve their state when turned off. This
	 * should be called in order to reduce the amount of time that the coils are
	 * powered.
	 */
	public void off1() {
		piston1.set(DoubleSolenoid.Value.kOff);
	}

	/**
	 * Turns off the piston1 double solenoid. This won't actuate anything
	 * because double solenoids preserve their state when turned off. This
	 * should be called in order to reduce the amount of time that the coils are
	 * powered.
	 */
	public void off2() {
		piston2.set(DoubleSolenoid.Value.kOff);
	}

	/**
	 * Release the latch so that we can shoot
	 */
	public void unlatch() {
		latchPiston.set(true);
	}

	/**
	 * Latch so that pressure can build up and we aren't limited by air flow.
	 */
	public void latch() {
		latchPiston.set(false);
	}

	/**
	 * Toggles the latch postions
	 */
	public void toggleLatchPosition() {
		latchPiston.set(!latchPiston.get());
	}

	/**
	 * @return Whether or not piston 1 is fully extended.
	 */
	public boolean piston1IsExtended() {
		return !piston1ReedSwitchFront.get();
	}

	/**
	 * @return Whether or not piston 1 is fully retracted.
	 */
	public boolean piston1IsRetracted() {
		return !piston1ReedSwitchBack.get();
	}

	/**
	 * Turns off all double solenoids. Double solenoids hold their position when
	 * they are turned off. We should turn them off whenever possible to extend
	 * the life of the coils
	 */
	public void offBoth() {
		piston1.set(DoubleSolenoid.Value.kOff);
		piston2.set(DoubleSolenoid.Value.kOff);
	}

	/**
	 * @return Whether or not the goal is hot as read by the banner sensor
	 */
	public boolean goalIsHot() {
		return hotGoalSensor.get();
	}
}
