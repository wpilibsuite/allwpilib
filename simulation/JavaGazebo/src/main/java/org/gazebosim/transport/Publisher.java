/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2016. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package org.gazebosim.transport;

import java.io.IOException;
import java.util.LinkedList;
import java.util.List;
import java.util.logging.Logger;

import com.google.protobuf.Message;

public class Publisher<T extends Message> implements PublisherRecord {
	private String topic, msgType;
	private String localHost;
	private int localPort;
	private List<Connection> listeners;
	private boolean latching = false;
	private T lastMsg = null;
	
	private static final Logger LOG = Logger.getLogger("Gazebo Transport");
	
	public Publisher(String topic, String msgType, String localHost, int localPort) {
		this.topic = topic;
		this.msgType = msgType;
		this.localHost = localHost;
		this.localPort = localPort;
		listeners = new LinkedList<>();
	}

	public synchronized void publish(T msg) {
		lastMsg = msg;
		List<Connection> toRemove = new LinkedList<>();
		for (Connection listener : listeners) {
			try {
				listener.write(msg);
			} catch (IOException e) {
				toRemove.add(listener);
			}
		}
		for (Connection listener : toRemove) {
			LOG.info("Removing listener from topic="+topic);
			try {
				listener.close();
			} catch (IOException e1) { /* Closing failed, probably not a big deal. */}
			listeners.remove(listener);
		}
	}

	@Override
	public String getTopic() {
		return topic;
	}

	@Override
	public String getHost() {
		return localHost;
	}

	@Override
	public int getPort() {
		return localPort;
	}

	@Override
	public String getMsgType() {
		return msgType;
	}

	@Override
	/**
	 * This function is called when another topic requests a subscription to a topic I am publishing
	 * Called in Node.java in the handle() function
	 */
	public synchronized void connect(Connection conn) {
		LOG.fine("Handling subscriber connection for topic: "+topic);
		if (latching && lastMsg != null) {
			try {
				conn.write(lastMsg);
			} catch (IOException e) {
				LOG.warning("Writing latched message failed on topic="+topic);
				try {
					conn.close();
				} catch (IOException e1) { /* Closing failed, probably not a big deal. */}
				return;
			}
		}
		listeners.add(conn);
		this.notifyAll();
	}

	public synchronized void waitForConnection() throws InterruptedException {
		while (this.listeners.isEmpty()) {
			this.wait();
		}
	}

	public synchronized boolean waitForConnection(long timeout_millis) throws InterruptedException {
		long start = System.currentTimeMillis();

		while (this.listeners.isEmpty()) {
			long remain = timeout_millis - (System.currentTimeMillis() - start);
			if (remain <= 0) {
				break;
			}
			this.wait(remain);
		}

		return !this.listeners.isEmpty();
	}

	public void setLatchMode(boolean b) {
		latching = b;
	}
}
