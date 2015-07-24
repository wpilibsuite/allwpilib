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
		CameraServer::GetInstance()->SetQuality(50);
		//the camera name (ex "cam0") can be found through the roborio web interface
		CameraServer::GetInstance()->StartAutomaticCapture("cam0");
	}

	void OperatorControl()
	{
		while (IsOperatorControl() && IsEnabled())
		{
			/** robot code here! **/
			Wait(0.005);				// wait for a motor update time
		}
	}
};

START_ROBOT_CLASS(QuickVisionRobot)

