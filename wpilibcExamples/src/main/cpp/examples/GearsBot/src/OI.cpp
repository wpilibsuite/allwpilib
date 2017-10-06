#include "OI.h"

#include <SmartDashboard/SmartDashboard.h>

#include "Commands/Autonomous.h"
#include "Commands/CloseClaw.h"
#include "Commands/OpenClaw.h"
#include "Commands/Pickup.h"
#include "Commands/Place.h"
#include "Commands/PrepareToPickup.h"
#include "Commands/SetElevatorSetpoint.h"

OI::OI() {
	frc::SmartDashboard::PutData("Open Claw", new OpenClaw());
	frc::SmartDashboard::PutData("Close Claw", new CloseClaw());

	// Connect the buttons to commands
	d_up.WhenPressed(new SetElevatorSetpoint(0.2));
	d_down.WhenPressed(new SetElevatorSetpoint(-0.2));
	d_right.WhenPressed(new CloseClaw());
	d_left.WhenPressed(new OpenClaw());

	r1.WhenPressed(new PrepareToPickup());
	r2.WhenPressed(new Pickup());
	l1.WhenPressed(new Place());
	l2.WhenPressed(new Autonomous());
}

frc::Joystick* OI::GetJoystick() {
	return &joy;
}
