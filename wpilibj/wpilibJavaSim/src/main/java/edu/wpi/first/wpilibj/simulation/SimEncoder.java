package edu.wpi.first.wpilibj.simulation;

import org.gazebosim.transport.Msgs;
import org.gazebosim.transport.Publisher;
import org.gazebosim.transport.SubscriberCallback;

import gazebo.msgs.GzFloat64.Float64;
import gazebo.msgs.GzString;

public class SimEncoder {
	private double position, velocity;
	private Publisher<GzString.String> command_pub;
	
	public SimEncoder(String topic) {
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

	public void start() {
		sendCommand("start");
	}

	public void stop() {
		sendCommand("stop");
	}

	private void sendCommand(String cmd) {
		command_pub.publish(Msgs.String(cmd));
	}

	public double getPosition() {
		return position;
	}

	public double getVelocity() {
		return velocity;
	}
}
