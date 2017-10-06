#include <WPILib.h>

/**
 * This is a demo program showing the use of the RobotDrive class.
 * Runs the motors with arcade steering.
 */
class Robot: public frc::IterativeRobot {
	frc::RobotDrive myRobot { 0, 1 };
	frc::Joystick stick { 0 };

public:
	void TeleopPeriodic() {
		myRobot.ArcadeDrive(stick); // drive with arcade style
	}
};

START_ROBOT_CLASS(Robot)
