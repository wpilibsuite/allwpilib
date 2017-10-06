#include "OI.h"

#include "Commands/Collect.h"
#include "Commands/DriveForward.h"
#include "Commands/LowGoal.h"
#include "Commands/SetCollectionSpeed.h"
#include "Commands/SetPivotSetpoint.h"
#include "Commands/Shoot.h"
#include "Subsystems/Collector.h"
#include "Subsystems/Pivot.h"

OI::OI() {
	R1.WhenPressed(new LowGoal());
	R2.WhenPressed(new Collect());

	L1.WhenPressed(new SetPivotSetpoint(Pivot::kShoot));
	L2.WhenPressed(new SetPivotSetpoint(Pivot::kShootNear));

	sticks.WhenActive(new Shoot());

	// SmartDashboard Buttons
	frc::SmartDashboard::PutData("Drive Forward", new DriveForward(2.25));
	frc::SmartDashboard::PutData("Drive Backward", new DriveForward(-2.25));
	frc::SmartDashboard::PutData("Start Rollers", new SetCollectionSpeed(Collector::kForward));
	frc::SmartDashboard::PutData("Stop Rollers", new SetCollectionSpeed(Collector::kStop));
	frc::SmartDashboard::PutData("Reverse Rollers", new SetCollectionSpeed(Collector::kReverse));
}

frc::Joystick* OI::GetJoystick() {
	return &joystick;
}
