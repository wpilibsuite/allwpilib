package org.gazebosim.transport;

import gazebo.msgs.GzPublish.Publish;

public class RemotePublisherRecord implements PublisherRecord {
	private Publish pub;

	public RemotePublisherRecord(Publish pub) {
		this.pub = pub;
	}

	@Override
	public String getTopic() {
		return pub.getTopic();
	}

	@Override
	public String getHost() {
		return pub.getHost();
	}

	@Override
	public int getPort() {
		return pub.getPort();
	}

	@Override
	public String getMsgType() {
		return pub.getMsgType();
	}
	
	public String toString() {
		return String.format("%s (%s) %s:%s", getTopic(), getMsgType(), getHost(), getPort()); 
	}

	@Override
	public void connect(Connection conn) {
		throw new RuntimeException("Someone thinks we're a remote server, can't connect.");
	}
}
