
#include "Robot.h"
#include "Commands/DriveAndShootAutonomous.h"
#include "Commands/DriveForward.h"

DriveTrain* Robot::drivetrain = NULL;
Pivot* Robot::pivot = NULL;
Collector* Robot::collector = NULL;
Shooter* Robot::shooter = NULL;
Pneumatics* Robot::pneumatics = NULL;

OI* Robot::oi = NULL;

void Robot::RobotInit() {
	drivetrain = new DriveTrain();
	pivot = new Pivot();
	collector = new Collector();
	shooter = new Shooter();
	pneumatics = new Pneumatics();

    // Show what command your subsystem is running on the SmartDashboard
    SmartDashboard::PutData(drivetrain);
    SmartDashboard::PutData(pivot);
    SmartDashboard::PutData(collector);
    SmartDashboard::PutData(shooter);
    SmartDashboard::PutData(pneumatics);

	oi = new OI();

	autonomousCommand = new DriveAndShootAutonomous();
	lw = LiveWindow::GetInstance();

	// instantiate the command used for the autonomous period
	autoChooser = new SendableChooser();
	autoChooser->AddDefault("Drive and Shoot", new DriveAndShootAutonomous());
	autoChooser->AddObject("Drive Forward", new DriveForward());
	SmartDashboard::PutData("Auto Mode", autoChooser);

	pneumatics->Start(); // Pressurize the pneumatics.
}

void Robot::AutonomousInit() {
	autonomousCommand = (Command*) autoChooser->GetSelected();
	autonomousCommand->Start();
}

void Robot::AutonomousPeriodic() {
	Scheduler::GetInstance().Run();
	Log();
}

void Robot::TeleopInit() {
	// This makes sure that the autonomous stops running when
	// teleop starts running. If you want the autonomous to
	// continue until interrupted by another command, remove
	// this line or comment it out.
	if (autonomousCommand != NULL) {
		autonomousCommand->Cancel();
	}
	std::cout << "Starting Teleop" << std::endl;
}

void Robot::TeleopPeriodic() {
	Scheduler::GetInstance().Run();
	Log();
}

void Robot::TestPeriodic() {
	lw->Run();
}

void Robot::DisabledInit() {
	shooter->Unlatch();
}

void Robot::DisabledPeriodic() {
	Log();
}

/**
 * Log interesting values to the SmartDashboard.
 */
void Robot::Log() {
	Robot::pneumatics->WritePressure();
	SmartDashboard::PutNumber("Pivot Pot Value", pivot->GetAngle());
	SmartDashboard::PutNumber("Left Distance", drivetrain->GetLeftEncoder()->GetDistance());
	SmartDashboard::PutNumber("Right Distance", drivetrain->GetRightEncoder()->GetDistance());
}

START_ROBOT_CLASS(Robot);
