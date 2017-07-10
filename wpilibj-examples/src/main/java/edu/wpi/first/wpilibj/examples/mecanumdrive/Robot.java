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

	private final int kFrontLeftChannel = 2;
	private final int kRearLeftChannel = 3;
	private final int kFrontRightChannel = 1;
	private final int kRearRightChannel = 0;

	private final int kJoystickChannel = 0;

	private RobotDrive robotDrive;
	private Joystick stick;

	@Override
	public void robotInit() {
		robotDrive = new RobotDrive(kFrontLeftChannel,
				kRearLeftChannel,
				kFrontRightChannel,
				kRearRightChannel);

		// Invert the left side motors. You may need to change or remove this to match your robot.
		robotDrive.setInvertedMotor(MotorType.kFrontLeft, true);
		robotDrive.setInvertedMotor(MotorType.kRearLeft, true);

		stick = new Joystick(kJoystickChannel);
	}

	@Override
	public void teleopPeriodic() {
		// Use the joystick X axis for lateral movement, Y axis for forward
		// movement, and Z axis for rotation.
		robotDrive.mecanumDrive_Cartesian(stick.getX(), stick.getY(), stick.getZ(), 0.0);
	}
}

