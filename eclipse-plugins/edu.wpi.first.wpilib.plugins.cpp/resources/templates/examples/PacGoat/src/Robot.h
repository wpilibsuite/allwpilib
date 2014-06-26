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

#include "Subsystems/DriveTrain.h"
#include "Subsystems/Pivot.h"
#include "Subsystems/Collector.h"
#include "Subsystems/Shooter.h"
#include "Subsystems/Pneumatics.h"
#include "OI.h"

class Robot: public IterativeRobot {
public:
	static DriveTrain* drivetrain;
	static Pivot* pivot;
	static Collector* collector;
	static Shooter* shooter;
	static Pneumatics* pneumatics;
	static OI* oi;

private:
	Command *autonomousCommand;
	LiveWindow *lw;
	SendableChooser* autoChooser;

	void RobotInit();
	void AutonomousInit();
	void AutonomousPeriodic();
	void TeleopInit();
	void TeleopPeriodic();
	void TestPeriodic();
	void DisabledInit();
	void DisabledPeriodic();

	void Log();
};


#endif /* ROBOT_H_ */
