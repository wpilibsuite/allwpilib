/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2016. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.simulation;

import java.io.IOException;
import java.util.logging.Logger;

import org.gazebosim.transport.Node;
import org.gazebosim.transport.Publisher;
import org.gazebosim.transport.Subscriber;
import org.gazebosim.transport.SubscriberCallback;

import com.google.protobuf.Message;

public class MainNode {

	private MainNode() {
	}

	private static final Logger LOG = Logger.getLogger("Simulation MainNode");
	private static Node mainNode;

	public static synchronized void openGazeboConnection() throws IOException, InterruptedException {
		if (mainNode != null) {
			LOG.warning("MainNode.openGazeboConnection() was already called!");
			return;
		}
		mainNode = new Node("frc");
		mainNode.waitForConnection();
	}

	public static <T extends Message> Publisher<T> advertise(String topic, T defaultMessage) {
		if (mainNode == null) {
			throw new IllegalStateException("MainNode.openGazeboConnection() should have already been called by RobotBase.main()!");
		}
		return mainNode.advertise(topic, defaultMessage);
	}

	public static <T extends Message> Subscriber<T> subscribe(String topic, T defaultMessage, SubscriberCallback<T> cb) {
		if (mainNode == null) {
			throw new IllegalStateException("MainNode.openGazeboConnection() should have already been called by RobotBase.main()!");
		}
		return mainNode.subscribe(topic, defaultMessage, cb);
	}
}
