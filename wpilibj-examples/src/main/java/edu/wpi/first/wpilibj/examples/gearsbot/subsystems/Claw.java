/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2017. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package edu.wpi.first.wpilibj.examples.gearsbot.subsystems;

import edu.wpi.first.wpilibj.DigitalInput;
import edu.wpi.first.wpilibj.SpeedController;
import edu.wpi.first.wpilibj.Victor;
import edu.wpi.first.wpilibj.command.Subsystem;
import edu.wpi.first.wpilibj.livewindow.LiveWindow;

/**
 * The claw subsystem is a simple system with a motor for opening and closing.
 * If using stronger motors, you should probably use a sensor so that the motors
 * don't stall.
 */
public class Claw extends Subsystem {
	private SpeedController motor = new Victor(7);
	private DigitalInput contact = new DigitalInput(5);

	public Claw() {
		super();

		// Let's show everything on the LiveWindow
		LiveWindow.addActuator("Claw", "Motor", (Victor) motor);
		LiveWindow.addActuator("Claw", "Limit Switch", contact);
	}

	@Override
	public void initDefaultCommand() {
	}

	public void log() {
	}

	/**
	 * Set the claw motor to move in the open direction.
	 */
	public void open() {
		motor.set(-1);
	}

	/**
	 * Set the claw motor to move in the close direction.
	 */
	public void close() {
		motor.set(1);
	}

	/**
	 * Stops the claw motor from moving.
	 */
	public void stop() {
		motor.set(0);
	}

	/**
	 * Return true when the robot is grabbing an object hard enough to trigger
	 * the limit switch.
	 */
	public boolean isGrabbing() {
		return contact.get();
	}
}
