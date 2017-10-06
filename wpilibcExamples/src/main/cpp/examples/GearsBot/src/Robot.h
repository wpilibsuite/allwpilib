#ifndef ROBOT_H_
#define ROBOT_H_

#include <memory>

#include <Commands/Command.h>
#include <IterativeRobot.h>
#include <LiveWindow/LiveWindow.h>

#include "Commands/Autonomous.h"
#include "OI.h"
#include "Subsystems/Claw.h"
#include "Subsystems/DriveTrain.h"
#include "Subsystems/Elevator.h"
#include "Subsystems/Wrist.h"

class Robot: public frc::IterativeRobot {
public:
	static std::shared_ptr<DriveTrain> drivetrain;
	static std::shared_ptr<Elevator> elevator;
	static std::shared_ptr<Wrist> wrist;
	static std::shared_ptr<Claw> claw;
	static std::unique_ptr<OI> oi;

private:
	Autonomous autonomousCommand;
	frc::LiveWindow* lw = frc::LiveWindow::GetInstance();

	void RobotInit() override;
	void AutonomousInit() override;
	void AutonomousPeriodic() override;
	void TeleopInit() override;
	void TeleopPeriodic() override;
	void TestPeriodic() override;
};

#endif  // ROBOT_H_
