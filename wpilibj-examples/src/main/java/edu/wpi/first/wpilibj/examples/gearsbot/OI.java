/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2017. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.examples.gearsbot;

import edu.wpi.first.wpilibj.Joystick;
import edu.wpi.first.wpilibj.buttons.JoystickButton;
import edu.wpi.first.wpilibj.examples.gearsbot.commands.Autonomous;
import edu.wpi.first.wpilibj.examples.gearsbot.commands.CloseClaw;
import edu.wpi.first.wpilibj.examples.gearsbot.commands.OpenClaw;
import edu.wpi.first.wpilibj.examples.gearsbot.commands.Pickup;
import edu.wpi.first.wpilibj.examples.gearsbot.commands.Place;
import edu.wpi.first.wpilibj.examples.gearsbot.commands.PrepareToPickup;
import edu.wpi.first.wpilibj.examples.gearsbot.commands.SetElevatorSetpoint;
import edu.wpi.first.wpilibj.examples.gearsbot.commands.SetWristSetpoint;
import edu.wpi.first.wpilibj.smartdashboard.SmartDashboard;

/**
 * This class is the glue that binds the controls on the physical operator
 * interface to the commands and command groups that allow control of the robot.
 */
public class OI {
	private Joystick joy = new Joystick(0);

	public OI() {
		// Put Some buttons on the SmartDashboard
		SmartDashboard.putData("Elevator Bottom", new SetElevatorSetpoint(0));
		SmartDashboard.putData("Elevator Platform", new SetElevatorSetpoint(0.2));
		SmartDashboard.putData("Elevator Top", new SetElevatorSetpoint(0.3));

		SmartDashboard.putData("Wrist Horizontal", new SetWristSetpoint(0));
		SmartDashboard.putData("Raise Wrist", new SetWristSetpoint(-45));

		SmartDashboard.putData("Open Claw", new OpenClaw());
		SmartDashboard.putData("Close Claw", new CloseClaw());

		SmartDashboard.putData("Deliver Soda", new Autonomous());

		// Create some buttons
		JoystickButton d_up = new JoystickButton(joy, 5);
		JoystickButton d_right = new JoystickButton(joy, 6);
		JoystickButton d_down = new JoystickButton(joy, 7);
		JoystickButton d_left = new JoystickButton(joy, 8);
		JoystickButton l2 = new JoystickButton(joy, 9);
		JoystickButton r2 = new JoystickButton(joy, 10);
		JoystickButton l1 = new JoystickButton(joy, 11);
		JoystickButton r1 = new JoystickButton(joy, 12);

		// Connect the buttons to commands
		d_up.whenPressed(new SetElevatorSetpoint(0.2));
		d_down.whenPressed(new SetElevatorSetpoint(-0.2));
		d_right.whenPressed(new CloseClaw());
		d_left.whenPressed(new OpenClaw());

		r1.whenPressed(new PrepareToPickup());
		r2.whenPressed(new Pickup());
		l1.whenPressed(new Place());
		l2.whenPressed(new Autonomous());
	}

	public Joystick getJoystick() {
		return joy;
	}
}
