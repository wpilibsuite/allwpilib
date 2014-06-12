package edu.wpi.first.wpilibj.simulation.ds;

import org.gazebosim.transport.Node;

public interface ISimJoystick {
	String getName();
	void advertise(Node node, int i);
	void publish();
}
