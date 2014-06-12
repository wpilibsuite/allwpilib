package edu.wpi.first.wpilibj.simulation;

import org.gazebosim.transport.Node;
import org.gazebosim.transport.Publisher;
import org.gazebosim.transport.Subscriber;
import org.gazebosim.transport.SubscriberCallback;

import com.google.protobuf.Message;

public class MainNode{
	private Node main;
	
	private MainNode() {
		main = new Node("frc");
	}
	
	private static MainNode instance;
	public static MainNode getInstance() {
		if (instance == null) {
			instance = new MainNode();
		}
		return instance;
	}
	
	public static <T extends Message> Publisher<T> advertise(String topic, T defaultMessage) {
		return getInstance().main.advertise(topic, defaultMessage);
	}

	public static <T extends Message> Subscriber<T>
			subscribe(String topic, T defaultMessage, SubscriberCallback<T> cb) {
		return getInstance().main.subscribe(topic, defaultMessage, cb);
	}
}
