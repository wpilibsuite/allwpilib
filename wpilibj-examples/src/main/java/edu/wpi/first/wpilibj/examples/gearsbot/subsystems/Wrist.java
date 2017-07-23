/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2017. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.examples.gearsbot.subsystems;

import edu.wpi.first.wpilibj.AnalogPotentiometer;
import edu.wpi.first.wpilibj.SpeedController;
import edu.wpi.first.wpilibj.Victor;
import edu.wpi.first.wpilibj.command.PIDSubsystem;
import edu.wpi.first.wpilibj.examples.gearsbot.Robot;
import edu.wpi.first.wpilibj.interfaces.Potentiometer;
import edu.wpi.first.wpilibj.livewindow.LiveWindow;
import edu.wpi.first.wpilibj.smartdashboard.SmartDashboard;

/**
 * The wrist subsystem is like the elevator, but with a rotational joint instead
 * of a linear joint.
 */
public class Wrist extends PIDSubsystem {

	private SpeedController m_motor;
	private Potentiometer m_pot;

	private static final double kP_real = 1;
	private static final double kP_simulation = 0.05;

	public Wrist() {
		super(kP_real, 0, 0);
		if (Robot.isSimulation()) { // Check for simulation and update PID values
			getPIDController().setPID(kP_simulation, 0, 0, 0);
		}
		setAbsoluteTolerance(2.5);

		m_motor = new Victor(6);

		// Conversion value of potentiometer varies between the real world and
		// simulation
		if (Robot.isReal()) {
			m_pot = new AnalogPotentiometer(3, -270.0 / 5);
		} else {
			m_pot = new AnalogPotentiometer(3); // Defaults to degrees
		}

		// Let's show everything on the LiveWindow
		LiveWindow.addActuator("Wrist", "Motor", (Victor) m_motor);
		LiveWindow.addSensor("Wrist", "Pot", (AnalogPotentiometer) m_pot);
		LiveWindow.addActuator("Wrist", "PID", getPIDController());
	}

	@Override
	public void initDefaultCommand() {
	}

	/**
	 * The log method puts interesting information to the SmartDashboard.
	 */
	public void log() {
		SmartDashboard.putData("Wrist Angle", (AnalogPotentiometer) m_pot);
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
