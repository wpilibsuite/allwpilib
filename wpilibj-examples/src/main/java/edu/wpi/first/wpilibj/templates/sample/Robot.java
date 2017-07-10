package edu.wpi.first.wpilibj.templates.sample;

import edu.wpi.first.wpilibj.RobotDrive;
import edu.wpi.first.wpilibj.SampleRobot;
import edu.wpi.first.wpilibj.Joystick;
import edu.wpi.first.wpilibj.Timer;
import edu.wpi.first.wpilibj.smartdashboard.SendableChooser;
import edu.wpi.first.wpilibj.smartdashboard.SmartDashboard;

/**
 * This is a demo program showing the use of the RobotDrive class. The
 * SampleRobot class is the base of a robot application that will automatically
 * call your Autonomous and OperatorControl methods at the right time as
 * controlled by the switches on the driver station or the field controls.
 *
 * The VM is configured to automatically run this class, and to call the
 * functions corresponding to each mode, as described in the SampleRobot
 * documentation. If you change the name of this class or the package after
 * creating this project, you must also update the build.properties file in the
 * project.
 *
 * WARNING: While it may look like a good choice to use for your code if you're
 * inexperienced, don't. Unless you know what you are doing, complex code will
 * be much more difficult under this system. Use IterativeRobot or Command-Based
 * instead if you're new.
 */
public class Robot extends SampleRobot {
	RobotDrive myRobot = new RobotDrive(0, 1);
	Joystick stick = new Joystick(0);
	final String defaultAuto = "Default";
	final String customAuto = "My Auto";
	SendableChooser<String> chooser = new SendableChooser<>();

	public Robot() {
		myRobot.setExpiration(0.1);
	}

	@Override
	public void robotInit() {
		chooser.addDefault("Default Auto", defaultAuto);
		chooser.addObject("My Auto", customAuto);
		SmartDashboard.putData("Auto modes", chooser);
	}

	/**
	 * This autonomous (along with the chooser code above) shows how to select
	 * between different autonomous modes using the dashboard. The sendable
	 * chooser code works with the Java SmartDashboard. If you prefer the
	 * LabVIEW Dashboard, remove all of the chooser code and uncomment the
	 * getString line to get the auto name from the text box below the Gyro
	 *
	 * You can add additional auto modes by adding additional comparisons to the
	 * if-else structure below with additional strings. If using the
	 * SendableChooser make sure to add them to the chooser code above as well.
	 *
	 * If you wanted to run a similar autonomous mode with an IterativeRobot you would write:
	 *
	 * <blockquote><pre>{@code
	 * Timer timer = new Timer();
	 *
	 * // This function is run once each time the robot enters autonomous mode
	 * public void autonomousInit() {
	 *     timer.reset();
	 *     timer.start();
	 * }
	 *
	 * // This function is called periodically during autonomous
	 * public void autonomousPeriodic() {
	 * // Drive for 2 seconds
	 *     if (timer.get() < 2.0) {
	 *         myRobot.drive(-0.5, 0.0); // drive forwards half speed
	 *     } else if (timer.get() < 5.0) {
	 *         myRobot.drive(-1.0, 0.0); // drive forwards full speed
	 *     } else {
	 *         myRobot.drive(0.0, 0.0); // stop robot
	 *     }
	 * }
	 * }</pre></blockquote>
	 */
	@Override
	public void autonomous() {
		String autoSelected = chooser.getSelected();
		// String autoSelected = SmartDashboard.getString("Auto Selector",
		// defaultAuto);
		System.out.println("Auto selected: " + autoSelected);

		switch (autoSelected) {
		case customAuto:
			myRobot.setSafetyEnabled(false);
			myRobot.drive(-0.5, 1.0); // spin at half speed
			Timer.delay(2.0); // for 2 seconds
			myRobot.drive(0.0, 0.0); // stop robot
			break;
		case defaultAuto:
		default:
			myRobot.setSafetyEnabled(false);
			myRobot.drive(-0.5, 0.0); // drive forwards half speed
			Timer.delay(2.0); // for 2 seconds
			myRobot.drive(0.0, 0.0); // stop robot
			break;
		}
	}

	/**
	 * Runs the motors with arcade steering.
	 *
	 * If you wanted to run a similar teleoperated mode with an IterativeRobot you would write:
	 * <blockquote><pre>{@code
	 * // This function is called periodically during operator control
	 * public void teleopPeriodic() {
	 *     myRobot.arcadeDrive(stick);
	 * }
	 * }
	 */
	@Override
	public void operatorControl() {
		myRobot.setSafetyEnabled(true);
		while (isOperatorControl() && isEnabled()) {
			myRobot.arcadeDrive(stick); // drive with arcade style (use right
										// stick)
			Timer.delay(0.005); // wait for a motor update time
		}
	}

	/**
	 * Runs during test mode
	 */
	@Override
	public void test() {
	}
}
