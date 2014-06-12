package org.gazebosim.transport;

import java.io.IOException;
import java.util.LinkedList;
import java.util.List;

import com.google.protobuf.Message;

public class Publisher<T extends Message> implements PublisherRecord {
	private String topic, msgType;
	private String localHost;
	private int localPort;
	private List<Connection> listeners;
	private boolean latching = false;
	private T lastMsg = null;

	public Publisher(String topic, String msgType, String localHost, int localPort) {
		this.topic = topic;
		this.msgType = msgType;
		this.localHost = localHost;
		this.localPort = localPort;
		listeners = new LinkedList<>();
	}

	public synchronized void publish(T msg) {
		lastMsg = msg;
		for (Connection listener : listeners) {
			try {
				listener.write(msg);
			} catch (IOException e) {
				try {
					listener.close();
				} catch (IOException e1) { /* Closing failed, probably not a big deal. */}
				listeners.remove(listener);
			}
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
	public synchronized void connect(Connection conn) {
		if (latching && lastMsg != null) {
			try {
				conn.write(lastMsg);
			} catch (IOException e) {
				return; // TODO: Log
			}
		}
		listeners.add(conn);
	}

	public void setLatchMode(boolean b) {
		latching = b;
	}
}
