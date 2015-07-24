/*
 * Robot.h
 *
 *  Created on: Jun 3, 2014
 *      Author: alex
 */

#ifndef MY_ROBOT_H_
#define MY_ROBOT_H_

#include "WPILib.h"
#include "Commands/Command.h"
#include "Commands/Autonomous.h"

#include "Subsystems/DriveTrain.h"
#include "Subsystems/Elevator.h"
#include "Subsystems/Wrist.h"
#include "Subsystems/Claw.h"
#include "OI.h"

class Robot: public IterativeRobot {
public:
	static std::shared_ptr<DriveTrain> drivetrain;
	static std::shared_ptr<Elevator> elevator;
	static std::shared_ptr<Wrist> wrist;
	static std::shared_ptr<Claw> claw;
	static std::unique_ptr<OI> oi;

private:
	Autonomous autonomousCommand;
	LiveWindow *lw = LiveWindow::GetInstance();

	void RobotInit();
	void AutonomousInit();
	void AutonomousPeriodic();
	void TeleopInit();
	void TeleopPeriodic();
	void TestPeriodic();
};


#endif /* ROBOT_H_ */
