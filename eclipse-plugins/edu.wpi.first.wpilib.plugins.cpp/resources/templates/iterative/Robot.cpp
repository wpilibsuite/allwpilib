#include "WPILib.h"

class Robot: public IterativeRobot
{
private:
	LiveWindow &lw = LiveWindow::GetInstance();

	void RobotInit()
	{
	}

	void AutonomousInit()
	{

	}

	void AutonomousPeriodic()
	{

	}

	void TeleopInit()
	{

	}

	void TeleopPeriodic()
	{

	}

	void TestPeriodic()
	{
		lw.Run();
	}
};

START_ROBOT_CLASS(Robot)
