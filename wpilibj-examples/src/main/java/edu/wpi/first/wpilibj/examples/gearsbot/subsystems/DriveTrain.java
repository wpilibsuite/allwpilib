/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2017. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.examples.gearsbot.subsystems;

import edu.wpi.first.wpilibj.AnalogGyro;
import edu.wpi.first.wpilibj.AnalogInput;
import edu.wpi.first.wpilibj.Encoder;
import edu.wpi.first.wpilibj.Joystick;
import edu.wpi.first.wpilibj.Joystick.AxisType;
import edu.wpi.first.wpilibj.RobotDrive;
import edu.wpi.first.wpilibj.SpeedController;
import edu.wpi.first.wpilibj.Talon;
import edu.wpi.first.wpilibj.command.Subsystem;
import edu.wpi.first.wpilibj.examples.gearsbot.Robot;
import edu.wpi.first.wpilibj.examples.gearsbot.commands.TankDriveWithJoystick;
import edu.wpi.first.wpilibj.livewindow.LiveWindow;
import edu.wpi.first.wpilibj.smartdashboard.SmartDashboard;

/**
 * The DriveTrain subsystem incorporates the sensors and actuators attached to
 * the robots chassis. These include four drive motors, a left and right encoder
 * and a gyro.
 */
public class DriveTrain extends Subsystem {

	private SpeedController m_frontLeftMotor = new Talon(1);
	private SpeedController m_rearLeftMotor = new Talon(2);
	private SpeedController m_frontRightMotor = new Talon(3);
	private SpeedController m_rearRightMotor = new Talon(4);

	private RobotDrive m_drive = new RobotDrive(m_frontLeftMotor, m_rearLeftMotor,
			m_frontRightMotor, m_rearRightMotor);

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

		// Let's show everything on the LiveWindow
		LiveWindow.addActuator("Drive Train", "Front_Left Motor",
				(Talon) m_frontLeftMotor);
		LiveWindow.addActuator("Drive Train", "Back Left Motor",
				(Talon) m_rearLeftMotor);
		LiveWindow.addActuator("Drive Train", "Front Right Motor",
				(Talon) m_frontRightMotor);
		LiveWindow.addActuator("Drive Train", "Back Right Motor",
				(Talon) m_rearRightMotor);
		LiveWindow.addSensor("Drive Train", "Left Encoder", m_leftEncoder);
		LiveWindow.addSensor("Drive Train", "Right Encoder", m_rightEncoder);
		LiveWindow.addSensor("Drive Train", "Rangefinder", m_rangefinder);
		LiveWindow.addSensor("Drive Train", "Gyro", m_gyro);
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
	 * @param joy
	 *            The ps3 style joystick to use to drive tank style.
	 */
	public void drive(Joystick joy) {
		drive(-joy.getY(), -joy.getAxis(AxisType.kThrottle));
	}

	/**
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
	 * @return The distance driven (average of left and right encoders).
	 */
	public double getDistance() {
		return (m_leftEncoder.getDistance() + m_rightEncoder.getDistance()) / 2;
	}

	/**
	 * @return The distance to the obstacle detected by the rangefinder.
	 */
	public double getDistanceToObstacle() {
		// Really meters in simulation since it's a rangefinder...
		return m_rangefinder.getAverageVoltage();
	}
}
