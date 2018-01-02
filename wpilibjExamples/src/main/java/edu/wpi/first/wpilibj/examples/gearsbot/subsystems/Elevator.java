/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.examples.gearsbot.subsystems;

import edu.wpi.first.wpilibj.AnalogPotentiometer;
import edu.wpi.first.wpilibj.Victor;
import edu.wpi.first.wpilibj.command.PIDSubsystem;
import edu.wpi.first.wpilibj.examples.gearsbot.Robot;
import edu.wpi.first.wpilibj.smartdashboard.SmartDashboard;

/**
 * The elevator subsystem uses PID to go to a given height. Unfortunately, in
 * it's current state PID values for simulation are different than in the real
 * world do to minor differences.
 */
public class Elevator extends PIDSubsystem {
	private Victor m_motor;
	private AnalogPotentiometer m_pot;

	private static final double kP_real = 4;
	private static final double kI_real = 0.07;
	private static final double kP_simulation = 18;
	private static final double kI_simulation = 0.2;

	public Elevator() {
		super(kP_real, kI_real, 0);
		if (Robot.isSimulation()) { // Check for simulation and update PID values
			getPIDController().setPID(kP_simulation, kI_simulation, 0, 0);
		}
		setAbsoluteTolerance(0.005);

		m_motor = new Victor(5);

		// Conversion value of potentiometer varies between the real world and
		// simulation
		if (Robot.isReal()) {
			m_pot = new AnalogPotentiometer(2, -2.0 / 5);
		} else {
			m_pot = new AnalogPotentiometer(2); // Defaults to meters
		}

		// Let's name everything on the LiveWindow
		addChild("Motor", m_motor);
		addChild("Pot", m_pot);
	}

	@Override
	public void initDefaultCommand() {
	}

	/**
	 * The log method puts interesting information to the SmartDashboard.
	 */
	public void log() {
		SmartDashboard.putData("Elevator Pot", (AnalogPotentiometer) m_pot);
	}

	/**
	 * Use the potentiometer as the PID sensor. This method is automatically
	 * called by the subsystem.
	 */
	@Override
	protected double returnPIDInput() {
		return m_pot.get();
	}

	/**
	 * Use the motor as the PID output. This method is automatically called by
	 * the subsystem.
	 */
	@Override
	protected void usePIDOutput(double power) {
		m_motor.set(power);
	}
}
