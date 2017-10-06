#include <CameraServer.h>
#include <IterativeRobot.h>

/**
 * Uses the CameraServer class to automatically capture video from a USB webcam
 * and send it to the FRC dashboard without doing any vision processing. This
 * is the easiest way to get camera images to the dashboard. Just add this to the
 * RobotInit() method in your program.
 */
class Robot: public frc::IterativeRobot {
public:
	void RobotInit() {
		CameraServer::GetInstance()->StartAutomaticCapture();
	}
};

START_ROBOT_CLASS(Robot)
