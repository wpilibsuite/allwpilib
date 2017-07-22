/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2017. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.examples.potentiometerpid;

import edu.wpi.first.wpilibj.AnalogInput;
import edu.wpi.first.wpilibj.IterativeRobot;
import edu.wpi.first.wpilibj.PIDController;
import edu.wpi.first.wpilibj.Spark;
import edu.wpi.first.wpilibj.SpeedController;
import edu.wpi.first.wpilibj.Joystick;

/**
 * This is a sample program to demonstrate how to use a soft potentiometer and a
 * PID controller to reach and maintain position setpoints on an elevator
 * mechanism.
 */
public class Robot extends IterativeRobot {

	private static final int kPotChannel = 1;
	private static final int kMotorChannel = 7;
	private static final int kJoystickChannel = 0;

	// bottom, middle, and top elevator setpoints
	private static final double kSetPoints[] = { 1.0, 2.6, 4.3 };

	// proportional, integral, and derivative speed constants; motor inverted
	// DANGER: when tuning PID constants, high/inappropriate values for kP, kI,
	// and kD may cause dangerous, uncontrollable, or undesired behavior!
	// these may need to be positive for a non-inverted motor
	private static final double kP = -5.0;
	private static final double kI = -0.02;
	private static final double kD = -2.0;

	private PIDController pidController;
	private AnalogInput potentiometer;
	private SpeedController elevatorMotor;
	private Joystick joystick;

	private int index = 0;
	private boolean previousButtonValue = false;

	@Override
	public void robotInit() {
		potentiometer = new AnalogInput(kPotChannel);
		elevatorMotor = new Spark(kMotorChannel);
		joystick = new Joystick(kJoystickChannel);

		pidController = new PIDController(kP, kI, kD, potentiometer, elevatorMotor);
		pidController.setInputRange(0, 5);
	}

	@Override
	public void teleopInit() {
		pidController.enable();
	}

	@Override
	public void teleopPeriodic() {
		// when the button is pressed once, the selected elevator setpoint
		// is incremented
		boolean currentButtonValue = joystick.getTrigger();
		if (currentButtonValue && !previousButtonValue) {
			// index of the elevator setpoint wraps around.
			index = (index + 1) % kSetPoints.length;
		}
		previousButtonValue = currentButtonValue;

		pidController.setSetpoint(kSetPoints[index]);
	}
}
