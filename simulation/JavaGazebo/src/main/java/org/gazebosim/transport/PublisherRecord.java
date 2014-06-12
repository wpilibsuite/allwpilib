package org.gazebosim.transport;

public interface PublisherRecord {
	String getTopic();
	String getHost();
	int getPort();
	String getMsgType();
	void connect(Connection conn);
}
