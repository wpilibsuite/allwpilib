package $package.subsystems;

import $package.Robot;
import $package.commands.DriveWithJoystick;

import edu.wpi.first.wpilibj.AnalogGyro;
import edu.wpi.first.wpilibj.CounterBase.EncodingType;
import edu.wpi.first.wpilibj.Encoder;
import edu.wpi.first.wpilibj.Joystick;
import edu.wpi.first.wpilibj.PIDSource.PIDSourceParameter;
import edu.wpi.first.wpilibj.RobotDrive;
import edu.wpi.first.wpilibj.SpeedController;
import edu.wpi.first.wpilibj.Victor;
import edu.wpi.first.wpilibj.command.Subsystem;
import edu.wpi.first.wpilibj.livewindow.LiveWindow;

/**
 * The DriveTrain subsystem controls the robot's chassis and reads in
 * information about it's speed and position.
 */
public class DriveTrain extends Subsystem {
	// Subsystem devices
	private SpeedController frontLeftCIM, frontRightCIM;
	private SpeedController backLeftCIM, backRightCIM;
	private RobotDrive drive;
	private Encoder rightEncoder, leftEncoder;
	private AnalogGyro gyro;

	public DriveTrain() {
		// Configure drive motors
		frontLeftCIM = new Victor(1);
		frontRightCIM = new Victor(2);
		backLeftCIM = new Victor(3);
		backRightCIM = new Victor(4);
		LiveWindow.addActuator("DriveTrain", "Front Left CIM", (Victor) frontLeftCIM);
		LiveWindow.addActuator("DriveTrain", "Front Right CIM", (Victor) frontRightCIM);
		LiveWindow.addActuator("DriveTrain", "Back Left CIM", (Victor) backLeftCIM);
		LiveWindow.addActuator("DriveTrain", "Back Right CIM", (Victor) backRightCIM);

		// Configure the RobotDrive to reflect the fact that all our motors are
		// wired backwards and our drivers sensitivity preferences.
		drive = new RobotDrive(frontLeftCIM, backLeftCIM, frontRightCIM, backRightCIM);
		drive.setSafetyEnabled(true);
		drive.setExpiration(0.1);
		drive.setSensitivity(0.5);
		drive.setMaxOutput(1.0);
		drive.setInvertedMotor(RobotDrive.MotorType.kFrontLeft, true);
		drive.setInvertedMotor(RobotDrive.MotorType.kRearLeft, true);
		drive.setInvertedMotor(RobotDrive.MotorType.kFrontRight, true);
		drive.setInvertedMotor(RobotDrive.MotorType.kRearRight, true);

		// Configure encoders
		rightEncoder = new Encoder(1, 2, true, EncodingType.k4X);
		leftEncoder = new Encoder(3, 4, false, EncodingType.k4X);
		rightEncoder.setPIDSourceParameter(PIDSourceParameter.kDistance);
		leftEncoder.setPIDSourceParameter(PIDSourceParameter.kDistance);

		if (Robot.isReal()) { // Converts to feet
			rightEncoder.setDistancePerPulse(0.0785398);
			leftEncoder.setDistancePerPulse(0.0785398);
		} else { // Convert to feet 4in diameter wheels with 360 tick simulated encoders
			rightEncoder.setDistancePerPulse((4.0/*in*/*Math.PI)/(360.0*12.0/*in/ft*/));
			leftEncoder.setDistancePerPulse((4.0/*in*/*Math.PI)/(360.0*12.0/*in/ft*/));
		}

		LiveWindow.addSensor("DriveTrain", "Right Encoder", rightEncoder);
		LiveWindow.addSensor("DriveTrain", "Left Encoder", leftEncoder);

		// Configure gyro
		gyro = new AnalogGyro(2);
		if (Robot.isReal()) {
			gyro.setSensitivity(0.007); // TODO: Handle more gracefully?
		}
		LiveWindow.addSensor("DriveTrain", "Gyro", gyro);
	}

	/**
	 * When other commands aren't using the drivetrain, allow tank drive with
	 * the joystick.
	 */
	public void initDefaultCommand() {
		setDefaultCommand(new DriveWithJoystick());
	}

	/**
	 * @param joy PS3 style joystick to use as the input for tank drive.  
	 */
	public void tankDrive(Joystick joy) {
		drive.tankDrive(joy.getY(), joy.getRawAxis(4));
	}

	/**
	 * @param leftAxis Left sides value
	 * @param rightAxis Right sides value
	 */
	public void tankDrive(double leftAxis, double rightAxis) {
		drive.tankDrive(leftAxis, rightAxis);
	}

	/**
	 * Stop the drivetrain from moving.
	 */
	public void stop() {
		drive.tankDrive(0, 0);
	}

	/**
	 * @return The encoder getting the distance and speed of left side of the drivetrain.
	 */
	public Encoder getLeftEncoder() {
		return leftEncoder;
	}

	/**
	 * @return The encoder getting the distance and speed of right side of the drivetrain.
	 */
	public Encoder getRightEncoder() {
		return rightEncoder;
	}

	/**
	 * @return The current angle of the drivetrain.
	 */
	public double getAngle() {
		return gyro.getAngle();
	}
}
