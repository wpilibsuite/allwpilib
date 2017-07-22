/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2017. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.examples.tankdrive;

import edu.wpi.first.wpilibj.RobotDrive;
import edu.wpi.first.wpilibj.IterativeRobot;
import edu.wpi.first.wpilibj.Joystick;

/**
 * This is a demo program showing the use of the RobotDrive class, specifically
 * it contains the code necessary to operate a robot with tank drive.
 */
public class Robot extends IterativeRobot {

	private RobotDrive myRobot;
	private Joystick leftStick;
	private Joystick rightStick;

	@Override
	public void robotInit() {
		myRobot = new RobotDrive(0, 1);
		leftStick = new Joystick(0);
		rightStick = new Joystick(1);
	}

	@Override
	public void teleopPeriodic() {
		myRobot.tankDrive(leftStick, rightStick);
	}
}
