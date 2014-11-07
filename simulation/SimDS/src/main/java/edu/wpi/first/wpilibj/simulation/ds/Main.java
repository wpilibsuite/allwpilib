package edu.wpi.first.wpilibj.simulation.ds;

import gazebo.msgs.GzFloat64.Float64;

import org.gazebosim.transport.Msgs;
import org.gazebosim.transport.Node;
import org.gazebosim.transport.Subscriber;
import org.gazebosim.transport.SubscriberCallback;

public class Main {
	private static double simTime = 0;
	private static Subscriber<Float64> sub;
	
	public static void main(String args[]) {
		Node node = new Node("frc");
		try {
			node.waitForConnection();
		} catch (Throwable thr) {
			System.err.println("Could not connect to Gazebo.");
			thr.printStackTrace();
			System.exit(1);
			return;
		}

		JoystickProvider provider = new JoystickProvider();
		DS ds = new DS(provider);
		ds.advertise(node);

    	sub = node.subscribe("time", Msgs.Float64(),
			new SubscriberCallback<Float64>() {
				@Override
				public void callback(Float64 msg) {
					simTime = msg.getData();
					synchronized(sub) {
						sub.notifyAll();
					}
				}
			}
		);

		while (true) {
	    	final double start = simTime;
			for (int i = 0; i < provider.getJoysticks().size(); i++) {
				ISimJoystick joystick = provider.getJoysticks().get(i);
				joystick.advertise(node, i);
				joystick.publish();
			}
			ds.publish();

	    	while ((simTime - start) < 0.020 /*20ms*/) {
	    		synchronized(sub) {
	    			try {
	    				sub.wait(); // Block until time progresses
	    			} catch (InterruptedException e) {
	    				e.printStackTrace();
	    			}
	    		}
			}
		}
	}
}
