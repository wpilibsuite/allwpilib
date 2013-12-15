//============================================================================
// Name        : Robot.cpp
// Author      :
// Version     :
// Copyright   :
// Description : Hello World in C++, Ansi-style
//============================================================================

//============================================================================
// Name        : Robot.cpp
// Author      :
// Version     :
// Copyright   :
// Description : Hello World in C++, Ansi-style
//============================================================================

#include "WPILib.h"

class Robot : public IterativeRobot {
private:
	LiveWindow *lw;
	
	virtual void RobotInit() {
		lw = LiveWindow::GetInstance();
	}
	
	virtual void AutonomousInit() {

	}
	
	virtual void AutonomousPeriodic() {

	}
	
	virtual void TeleopInit() {
	  
	}
	
	virtual void TeleopPeriodic() {

	}
	
	virtual void TestPeriodic() {
		lw->Run();
	}
};

START_ROBOT_CLASS(Robot);
