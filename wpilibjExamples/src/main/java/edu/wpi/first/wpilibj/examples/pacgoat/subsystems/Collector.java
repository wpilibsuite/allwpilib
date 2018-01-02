/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.examples.pacgoat.subsystems;

import edu.wpi.first.wpilibj.DigitalInput;
import edu.wpi.first.wpilibj.Solenoid;
import edu.wpi.first.wpilibj.SpeedController;
import edu.wpi.first.wpilibj.Victor;
import edu.wpi.first.wpilibj.command.Subsystem;

/**
 * The Collector subsystem has one motor for the rollers, a limit switch for
 * ball detection, a piston for opening and closing the claw, and a reed switch
 * to check if the piston is open.
 */
public class Collector extends Subsystem {
	// Constants for some useful speeds
	public static final double kForward = 1;
	public static final double kStop = 0;
	public static final double kReverse = -1;

	// Subsystem devices
	private SpeedController m_rollerMotor = new Victor(6);
	private DigitalInput m_ballDetector = new DigitalInput(10);
	private DigitalInput m_openDetector = new DigitalInput(6);
	private Solenoid m_piston = new Solenoid(1, 1);

	public Collector() {
		// Put everything to the LiveWindow for testing.
		addChild("Roller Motor", (Victor) m_rollerMotor);
		addChild("Ball Detector", m_ballDetector);
		addChild("Claw Open Detector", m_openDetector);
		addChild("Piston", m_piston);
	}

	/**
	 * Whether or not the robot has the ball.
	 *
	 * <p>NOTE: The current simulation model uses the the lower part of the claw
	 * since the limit switch wasn't exported. At some point, this will be
	 * updated.
	 *
	 * @return Whether or not the robot has the ball.
	 */
	public boolean hasBall() {
		return m_ballDetector.get(); // TODO: prepend ! to reflect real robot
	}

	/**
	 * Set the speed to spin the collector rollers.
	 *
	 * @param speed The speed to spin the rollers.
	 */
	public void setSpeed(double speed) {
		m_rollerMotor.set(-speed);
	}

	/**
	 * Stop the rollers from spinning.
	 */
	public void stop() {
		m_rollerMotor.set(0);
	}

	/**
	 * Wether or not the claw is open.
	 *
	 * @return Whether or not the claw is open.
	 */
	public boolean isOpen() {
		return m_openDetector.get(); // TODO: prepend ! to reflect real robot
	}

	/**
	 * Open the claw up (For shooting).
	 */
	public void open() {
		m_piston.set(true);
	}

	/**
	 * Close the claw (For collecting and driving).
	 */
	public void close() {
		m_piston.set(false);
	}

	/**
	 * No default command.
	 */
	@Override
	protected void initDefaultCommand() {
	}
}
