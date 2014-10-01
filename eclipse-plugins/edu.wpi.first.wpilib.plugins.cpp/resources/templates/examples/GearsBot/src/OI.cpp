/*
 * OI.cpp
 *
 *  Created on: Jun 3, 2014
 *      Author: alex
 */

#include "OI.h"

#include "Commands/SetElevatorSetpoint.h"
#include "Commands/OpenClaw.h"
#include "Commands/CloseClaw.h"
#include "Commands/PrepareToPickup.h"
#include "Commands/Pickup.h"
#include "Commands/Place.h"
#include "Commands/Autonomous.h"

OI::OI() {
	SmartDashboard::PutData("Open Claw", new OpenClaw());
	SmartDashboard::PutData("Close Claw", new CloseClaw());

	joy= new Joystick(0);


    // Create some buttons
    JoystickButton* d_up = new JoystickButton(joy, 5);
    JoystickButton* d_right= new JoystickButton(joy, 6);
    JoystickButton* d_down= new JoystickButton(joy, 7);
    JoystickButton* d_left = new JoystickButton(joy, 8);
    JoystickButton* l2 = new JoystickButton(joy, 9);
    JoystickButton* r2 = new JoystickButton(joy, 10);
    JoystickButton* l1 = new JoystickButton(joy, 11);
    JoystickButton* r1 = new JoystickButton(joy, 12);

    // Connect the buttons to commands
    d_up->WhenPressed(new SetElevatorSetpoint(0.2));
    d_down->WhenPressed(new SetElevatorSetpoint(-0.2));
    d_right->WhenPressed(new CloseClaw());
    d_left->WhenPressed(new OpenClaw());

    r1->WhenPressed(new PrepareToPickup());
    r2->WhenPressed(new Pickup());
    l1->WhenPressed(new Place());
    l2->WhenPressed(new Autonomous());
}


Joystick* OI::GetJoystick() {
	return joy;
}
