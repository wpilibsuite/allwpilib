#include "OI.h"

#include "Subsystems/Pivot.h"
#include "Subsystems/Collector.h"
#include "Commands/LowGoal.h"
#include "Commands/Collect.h"
#include "Commands/SetPivotSetpoint.h"
#include "Commands/Shoot.h"
#include "Commands/DriveForward.h"
#include "Commands/SetCollectionSpeed.h"

OI::OI()
    : joystick(0),
      L1(&joystick, 11),
      L2(&joystick, 9),
      R1(&joystick, 12),
      R2(&joystick, 10),
      sticks(&joystick, 2, 3) {

	R1.WhenPressed(new LowGoal());
	R2.WhenPressed(new Collect());

	L1.WhenPressed(new SetPivotSetpoint(Pivot::SHOOT));
	L2.WhenPressed(new SetPivotSetpoint(Pivot::SHOOT_NEAR));

	sticks.WhenActive(new Shoot());


	// SmartDashboard Buttons
	SmartDashboard::PutData("Drive Forward", new DriveForward(2.25));
	SmartDashboard::PutData("Drive Backward", new DriveForward(-2.25));
	SmartDashboard::PutData("Start Rollers", new SetCollectionSpeed(Collector::FORWARD));
	SmartDashboard::PutData("Stop Rollers", new SetCollectionSpeed(Collector::STOP));
	SmartDashboard::PutData("Reverse Rollers", new SetCollectionSpeed(Collector::REVERSE));
}


Joystick* OI::GetJoystick() {
	return &joystick;
}
