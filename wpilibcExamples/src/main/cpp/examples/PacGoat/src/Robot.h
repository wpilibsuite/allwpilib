#ifndef MY_ROBOT_H_
#define MY_ROBOT_H_

#include <memory>

#include <Commands/Command.h>
#include <IterativeRobot.h>
#include <SmartDashboard/SendableChooser.h>

#include "Commands/DriveAndShootAutonomous.h"
#include "Commands/DriveForward.h"
#include "OI.h"
#include "Subsystems/Collector.h"
#include "Subsystems/DriveTrain.h"
#include "Subsystems/Pivot.h"
#include "Subsystems/Pneumatics.h"
#include "Subsystems/Shooter.h"

class Robot: public IterativeRobot {
public:
	static std::shared_ptr<DriveTrain> drivetrain;
	static std::shared_ptr<Pivot> pivot;
	static std::shared_ptr<Collector> collector;
	static std::shared_ptr<Shooter> shooter;
	static std::shared_ptr<Pneumatics> pneumatics;
	static std::unique_ptr<OI> oi;

private:
	frc::Command* autonomousCommand = nullptr;
	std::unique_ptr<frc::Command> driveAndShootAuto {
		new DriveAndShootAutonomous() };
	std::unique_ptr<frc::Command> driveForwardAuto { new DriveForward() };
	SendableChooser<frc::Command*> autoChooser;

	void RobotInit() override;
	void AutonomousInit() override;
	void AutonomousPeriodic() override;
	void TeleopInit() override;
	void TeleopPeriodic() override;
	void TestPeriodic() override;
	void DisabledInit() override;
	void DisabledPeriodic() override;

	void Log();
};

#endif  // ROBOT_H_
