package edu.wpi.first.wpilibj.simulation;

import gazebo.msgs.GzBool.Bool;

import org.gazebosim.transport.Msgs;
import org.gazebosim.transport.SubscriberCallback;

public class SimDigitalInput {
	private boolean value;
	
	public SimDigitalInput(String topic) {
    	MainNode.subscribe(topic, Msgs.Bool(),
			new SubscriberCallback<Bool>() {
    			@Override public void callback(Bool msg) {
    				value = msg.getData();
    			}
			}
		);
	}

	public boolean get() {
		return value;
	}
}
