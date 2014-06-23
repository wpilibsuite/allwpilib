package $package;

import edu.wpi.first.wpilibj.IterativeRobot;
import edu.wpi.first.wpilibj.command.Command;
import edu.wpi.first.wpilibj.command.Scheduler;
import edu.wpi.first.wpilibj.livewindow.LiveWindow;

import $package.commands.DriveAndShootAutonomous;
import $package.commands.DriveForward;
import $package.subsystems.Collector;
import $package.subsystems.DriveTrain;
import $package.subsystems.Pivot;
import $package.subsystems.Pneumatics;
import $package.subsystems.Shooter;

import edu.wpi.first.wpilibj.smartdashboard.SendableChooser;
import edu.wpi.first.wpilibj.smartdashboard.SmartDashboard;

/**
 * This is the main class for running the PacGoat code.
 * 
 * The VM is configured to automatically run this class, and to call the
 * functions corresponding to each mode, as described in the IterativeRobot
 * documentation. If you change the name of this class or the package after
 * creating this project, you must also update the manifest file in the resource
 * directory.
 */
public class Robot extends IterativeRobot {
	Command autonomousCommand;
	public static OI oi;

	public static DriveTrain drivetrain;
	public static Collector collector;
	public static Shooter shooter;
	public static Pneumatics pneumatics;
	public static Pivot pivot;;

	public SendableChooser autoChooser;
	public SendableChooser autonomousDirectionChooser;

	// This function is run when the robot is first started up and should be
	// used for any initialization code.
	public void robotInit() {
		// Initialize the subsystems
		drivetrain = new DriveTrain();
		collector = new Collector();
		shooter = new Shooter();
		pneumatics = new Pneumatics();
		pivot = new Pivot();
		SmartDashboard.putData(drivetrain);
		SmartDashboard.putData(collector);
		SmartDashboard.putData(shooter);
		SmartDashboard.putData(pneumatics);
		SmartDashboard.putData(pivot);

		// This MUST be here. If the OI creates Commands (which it very likely
		// will), constructing it during the construction of CommandBase (from
		// which commands extend), subsystems are not guaranteed to be
		// yet. Thus, their requires() statements may grab null pointers. Bad
		// news. Don't move it.
		oi = new OI();

		// instantiate the command used for the autonomous period
		autoChooser = new SendableChooser();
		autoChooser.addDefault("Drive and Shoot", new DriveAndShootAutonomous());
		autoChooser.addObject("Drive Forward", new DriveForward());
		SmartDashboard.putData("Auto Mode", autoChooser);
		
		pneumatics.start(); // Pressurize the pneumatics.
	}

	public void autonomousInit() {
		Command auto = (Command) autoChooser.getSelected();
		auto.start();
	}

	// This function is called periodically during autonomous
	public void autonomousPeriodic() {
		Scheduler.getInstance().run();
		log();
	}

	public void teleopInit() {
		// This makes sure that the autonomous stops running when
		// teleop starts running. If you want the autonomous to
		// continue until interrupted by another command, remove
		// this line or comment it out.
		if (autonomousCommand != null) {
			autonomousCommand.cancel();
		}
	}

	// This function is called periodically during operator control
	public void teleopPeriodic() {
		Scheduler.getInstance().run();
		log();
	}

	// This function called periodically during test mode
	public void testPeriodic() {
		LiveWindow.run();
	}

	public void disabledInit() {
		Robot.shooter.unlatch();
	}

	// This function is called periodically while disabled
	public void disabledPeriodic() {
		log();
	}
	
	/**
	 * Log interesting values to the SmartDashboard.
	 */
	private void log() {
		Robot.pneumatics.writePressure();
		SmartDashboard.putNumber("Pivot Pot Value", Robot.pivot.getAngle());
		SmartDashboard.putNumber("Left Distance", drivetrain.getLeftEncoder().getDistance());
		SmartDashboard.putNumber("Right Distance", drivetrain.getRightEncoder().getDistance());
	}
}
