package edu.wpi.first.wpilibj.examples.motorcontrol;

import edu.wpi.first.wpilibj.IterativeRobot;
import edu.wpi.first.wpilibj.Joystick;
import edu.wpi.first.wpilibj.SpeedController;
import edu.wpi.first.wpilibj.Spark;

/**
 * This sample program shows how to control a motor using a joystick. In the
 * operator control part of the program, the joystick is read and the value is
 * written to the motor.
 *
 * Joystick analog values range from -1 to 1 and speed controller inputs also
 * range from -1 to 1 making it easy to work together.
 */
public class Robot extends IterativeRobot {

	private static final int kMotorPort = 0;
	private static final int kJoystickPort = 0;

	private SpeedController motor;
	private Joystick joystick;

	@Override
	public void robotInit() {
		motor = new Spark(kMotorPort);
		joystick = new Joystick(kJoystickPort);
	}

	@Override
	public void teleopPeriodic() {
		motor.set(joystick.getY());
	}
}
