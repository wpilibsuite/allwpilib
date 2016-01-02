/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2016. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

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
