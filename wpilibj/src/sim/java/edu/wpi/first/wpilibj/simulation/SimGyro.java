/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2016. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.simulation;

import gazebo.msgs.GzFloat64.Float64;
import gazebo.msgs.GzString;

import org.gazebosim.transport.Msgs;
import org.gazebosim.transport.Publisher;
import org.gazebosim.transport.SubscriberCallback;

public class SimGyro {
	private double position, velocity;
	private Publisher<GzString.String> command_pub;
	
	public SimGyro(String topic) {
		command_pub = MainNode.advertise(topic+"/control", Msgs.String());
		command_pub.setLatchMode(true);

		MainNode.subscribe(topic+"/position", Msgs.Float64(),
			new SubscriberCallback<Float64>() {
				@Override public void callback(Float64 msg) {
					position = msg.getData();
				}
			}
    	);
    	
    	MainNode.subscribe(topic+"/velocity", Msgs.Float64(),
			new SubscriberCallback<Float64>() {
				@Override public void callback(Float64 msg) {
					velocity = msg.getData();
				}
			}
		);
	}
	
	public void reset() {
		sendCommand("reset");
	}

	private void sendCommand(String cmd) {
		command_pub.publish(Msgs.String(cmd));
	}

	public double getAngle() {
		return position;
	}

	public double getVelocity() {
		return velocity;
	}
}
