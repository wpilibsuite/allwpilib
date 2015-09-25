package edu.wpi.first.wpilibj.simulation;


import org.gazebosim.transport.Msgs;
import org.gazebosim.transport.SubscriberCallback;

import gazebo.msgs.GzFloat64.Float64;

public class SimFloatInput {
	private double value;
	
	public SimFloatInput(String topic) {
    	MainNode.subscribe(topic, Msgs.Float64(),
			new SubscriberCallback<Float64>() {
    			@Override public void callback(Float64 msg) {
    				value = msg.getData();
    			}
			}
		);
	}

	public double get() {
		return value;
	}
}
