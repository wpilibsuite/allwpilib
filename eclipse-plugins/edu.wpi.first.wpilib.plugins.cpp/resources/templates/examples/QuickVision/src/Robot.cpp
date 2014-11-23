#include "WPILib.h"

/**
 * Uses the CameraServer class to automatically capture video from a USB webcam
 * and send it to the FRC dashboard without doing any vision processing. This
 * is the easiest way to get camera images to the dashboard. Just add this to the
 * RobotInit() method in your program.
 */
class QuickVisionRobot : public SampleRobot
{
public:
	void RobotInit() override {
		CameraServer::GetInstance()->SetQuality(75);
		CameraServer::GetInstance()->StartAutomaticCapture();
	}

	void OperatorControl() override {
	}
};

START_ROBOT_CLASS(QuickVisionRobot);

