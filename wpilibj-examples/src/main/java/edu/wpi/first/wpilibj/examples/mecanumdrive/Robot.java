/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2017. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.examples.mecanumdrive;

import edu.wpi.first.wpilibj.IterativeRobot;
import edu.wpi.first.wpilibj.Joystick;
import edu.wpi.first.wpilibj.RobotDrive;
import edu.wpi.first.wpilibj.RobotDrive.MotorType;

/**
 * This is a demo program showing how to use Mecanum control with the RobotDrive
 * class.
 */
public class Robot extends IterativeRobot {

	private static final int kFrontLeftChannel = 2;
	private static final int kRearLeftChannel = 3;
	private static final int kFrontRightChannel = 1;
	private static final int kRearRightChannel = 0;

	private static final int kJoystickChannel = 0;

	private RobotDrive m_robotDrive;
	private Joystick m_stick;

	@Override
	public void robotInit() {
		m_robotDrive = new RobotDrive(kFrontLeftChannel,
				kRearLeftChannel,
				kFrontRightChannel,
				kRearRightChannel);

		// Invert the left side motors. You may need to change or remove this to match your robot.
		m_robotDrive.setInvertedMotor(MotorType.kFrontLeft, true);
		m_robotDrive.setInvertedMotor(MotorType.kRearLeft, true);

		m_stick = new Joystick(kJoystickChannel);
	}

	@Override
	public void teleopPeriodic() {
		// Use the joystick X axis for lateral movement, Y axis for forward
		// movement, and Z axis for rotation.
		m_robotDrive.mecanumDrive_Cartesian(m_stick.getX(), m_stick.getY(), m_stick.getZ(), 0.0);
	}
}
