/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2016. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.simulation.ds;

import gazebo.msgs.GzJoystick.Joystick;

import java.util.ArrayList;
import java.util.List;

import org.gazebosim.transport.Node;
import org.gazebosim.transport.Publisher;

import net.java.games.input.Component;
import net.java.games.input.Controller;

public class SimJoystick implements ISimJoystick {
	private Controller controller;
	private List<Component> axes, buttons;
	private Publisher<Joystick> pub = null;
	private int prevI = -1; private Node prevNode = null;

	public SimJoystick(Controller controller) {
		this.controller = controller;
		axes = new ArrayList<>();
		buttons = new ArrayList<>();
        for(Component c : controller.getComponents()) {
        	if (c.getIdentifier() instanceof Component.Identifier.Axis) {
        		axes.add(c);
        	} else if (c.getIdentifier() instanceof Component.Identifier.Button) {
        		buttons.add(c);
        	}
        }
	}

	@Override
	public String getName() {
		return controller.getName();
	}

	@Override	
	public String toString() {
		return getName();
	}

	@Override
	public void advertise(Node node, int i) {
		if (pub == null) {
			// I'm good
		} else if (prevI != i || prevNode != node) {
			// TODO: pub.close();
		} else {
			return; // No change
		}
		pub = node.advertise("ds/joysticks/"+i, Joystick.getDefaultInstance());
		prevNode = node;
		prevI = i;
	}
	
	@Override
	public void publish() {
		controller.poll();
		Joystick.Builder builder = Joystick.newBuilder();
		for (Component a : axes) {
			builder.addAxes(a.getPollData());
		}
		for (Component b : buttons) {
			builder.addButtons(b.getPollData() > 0.5);
		}
		pub.publish(builder.build());
	}
}
