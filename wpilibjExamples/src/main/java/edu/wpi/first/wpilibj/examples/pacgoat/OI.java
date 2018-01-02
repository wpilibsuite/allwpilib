/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.examples.pacgoat;

import edu.wpi.first.wpilibj.examples.pacgoat.commands.Collect;
import edu.wpi.first.wpilibj.examples.pacgoat.commands.DriveForward;
import edu.wpi.first.wpilibj.examples.pacgoat.commands.LowGoal;
import edu.wpi.first.wpilibj.examples.pacgoat.commands.SetCollectionSpeed;
import edu.wpi.first.wpilibj.examples.pacgoat.commands.SetPivotSetpoint;
import edu.wpi.first.wpilibj.examples.pacgoat.commands.Shoot;
import edu.wpi.first.wpilibj.examples.pacgoat.subsystems.Collector;
import edu.wpi.first.wpilibj.examples.pacgoat.subsystems.Pivot;
import edu.wpi.first.wpilibj.examples.pacgoat.triggers.DoubleButton;

import edu.wpi.first.wpilibj.Joystick;
import edu.wpi.first.wpilibj.buttons.JoystickButton;
import edu.wpi.first.wpilibj.smartdashboard.SmartDashboard;

/**
 * The operator interface of the robot, it has been simplified from the real
 * robot to allow control with a single PS3 joystick. As a result, not all
 * functionality from the real robot is available.
 */
public class OI {
	public Joystick m_joystick = new Joystick(0);

	public OI() {
		new JoystickButton(m_joystick, 12).whenPressed(new LowGoal());
		new JoystickButton(m_joystick, 10).whenPressed(new Collect());

		new JoystickButton(m_joystick, 11).whenPressed(
				new SetPivotSetpoint(Pivot.kShoot));
		new JoystickButton(m_joystick, 9).whenPressed(
				new SetPivotSetpoint(Pivot.kShootNear));

		new DoubleButton(m_joystick, 2, 3).whenActive(new Shoot());

		// SmartDashboard Buttons
		SmartDashboard.putData("Drive Forward", new DriveForward(2.25));
		SmartDashboard.putData("Drive Backward", new DriveForward(-2.25));
		SmartDashboard.putData("Start Rollers",
				new SetCollectionSpeed(Collector.kForward));
		SmartDashboard.putData("Stop Rollers",
				new SetCollectionSpeed(Collector.kStop));
		SmartDashboard.putData("Reverse Rollers",
				new SetCollectionSpeed(Collector.kReverse));
	}

	public Joystick getJoystick() {
		return m_joystick;
	}
}
