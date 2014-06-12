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
