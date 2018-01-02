/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.examples.gearsbot.subsystems;

import edu.wpi.first.wpilibj.AnalogGyro;
import edu.wpi.first.wpilibj.AnalogInput;
import edu.wpi.first.wpilibj.Encoder;
import edu.wpi.first.wpilibj.Joystick;
import edu.wpi.first.wpilibj.Spark;
import edu.wpi.first.wpilibj.SpeedController;
import edu.wpi.first.wpilibj.SpeedControllerGroup;
import edu.wpi.first.wpilibj.command.Subsystem;
import edu.wpi.first.wpilibj.drive.DifferentialDrive;
import edu.wpi.first.wpilibj.examples.gearsbot.Robot;
import edu.wpi.first.wpilibj.examples.gearsbot.commands.TankDriveWithJoystick;
import edu.wpi.first.wpilibj.smartdashboard.SmartDashboard;

/**
 * The DriveTrain subsystem incorporates the sensors and actuators attached to
 * the robots chassis. These include four drive motors, a left and right encoder
 * and a gyro.
 */
public class DriveTrain extends Subsystem {
	private SpeedController m_leftMotor
			= new SpeedControllerGroup(new Spark(0), new Spark(1));
	private SpeedController m_rightMotor
			= new SpeedControllerGroup(new Spark(2), new Spark(3));

	private DifferentialDrive m_drive
			= new DifferentialDrive(m_leftMotor, m_rightMotor);

	private Encoder m_leftEncoder = new Encoder(1, 2);
	private Encoder m_rightEncoder = new Encoder(3, 4);
	private AnalogInput m_rangefinder = new AnalogInput(6);
	private AnalogGyro m_gyro = new AnalogGyro(1);

	public DriveTrain() {
		super();

		// Encoders may measure differently in the real world and in
		// simulation. In this example the robot moves 0.042 barleycorns
		// per tick in the real world, but the simulated encoders
		// simulate 360 tick encoders. This if statement allows for the
		// real robot to handle this difference in devices.
		if (Robot.isReal()) {
			m_leftEncoder.setDistancePerPulse(0.042);
			m_rightEncoder.setDistancePerPulse(0.042);
		} else {
			// Circumference in ft = 4in/12(in/ft)*PI
			m_leftEncoder.setDistancePerPulse((4.0 / 12.0 * Math.PI) / 360.0);
			m_rightEncoder.setDistancePerPulse((4.0 / 12.0 * Math.PI) / 360.0);
		}

		// Let's name the sensors on the LiveWindow
		addChild("Drive", m_drive);
		addChild("Left Encoder", m_leftEncoder);
		addChild("Right Encoder", m_rightEncoder);
		addChild("Rangefinder", m_rangefinder);
		addChild("Gyro", m_gyro);
	}

	/**
	 * When no other command is running let the operator drive around using the
	 * PS3 joystick.
	 */
	@Override
	public void initDefaultCommand() {
		setDefaultCommand(new TankDriveWithJoystick());
	}

	/**
	 * The log method puts interesting information to the SmartDashboard.
	 */
	public void log() {
		SmartDashboard.putNumber("Left Distance", m_leftEncoder.getDistance());
		SmartDashboard.putNumber("Right Distance", m_rightEncoder.getDistance());
		SmartDashboard.putNumber("Left Speed", m_leftEncoder.getRate());
		SmartDashboard.putNumber("Right Speed", m_rightEncoder.getRate());
		SmartDashboard.putNumber("Gyro", m_gyro.getAngle());
	}

	/**
	 * Tank style driving for the DriveTrain.
	 *
	 * @param left
	 *            Speed in range [-1,1]
	 * @param right
	 *            Speed in range [-1,1]
	 */
	public void drive(double left, double right) {
		m_drive.tankDrive(left, right);
	}

	/**
	 * Tank style driving for the DriveTrain.
	 *
	 * @param joy The ps3 style joystick to use to drive tank style.
	 */
	public void drive(Joystick joy) {
		drive(-joy.getY(), -joy.getThrottle());
	}

	/**
	 * Get the robot's heading.
	 *
	 * @return The robots heading in degrees.
	 */
	public double getHeading() {
		return m_gyro.getAngle();
	}

	/**
	 * Reset the robots sensors to the zero states.
	 */
	public void reset() {
		m_gyro.reset();
		m_leftEncoder.reset();
		m_rightEncoder.reset();
	}

	/**
	 * Get the average distance of the encoders since the last reset.
	 *
	 * @return The distance driven (average of left and right encoders).
	 */
	public double getDistance() {
		return (m_leftEncoder.getDistance() + m_rightEncoder.getDistance()) / 2;
	}

	/**
	 * Get the distance to the obstacle.
	 *
	 * @return The distance to the obstacle detected by the rangefinder.
	 */
	public double getDistanceToObstacle() {
		// Really meters in simulation since it's a rangefinder...
		return m_rangefinder.getAverageVoltage();
	}
}
