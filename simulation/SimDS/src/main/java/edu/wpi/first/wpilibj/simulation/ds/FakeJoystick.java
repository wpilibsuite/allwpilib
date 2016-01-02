/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2016. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.simulation.ds;

import org.gazebosim.transport.Node;


public class FakeJoystick implements ISimJoystick {

	public String getName() {
		return "Empty Joystick";
	}
	
	public String toString() {
		return getName();
	}

	@Override public void advertise(Node node, int i) {}

	@Override public void publish() {}
}
