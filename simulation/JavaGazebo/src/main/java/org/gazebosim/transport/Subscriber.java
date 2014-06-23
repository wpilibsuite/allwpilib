package org.gazebosim.transport;

import gazebo.msgs.GzSubscribe.Subscribe;

import java.io.IOException;
import java.util.LinkedList;
import java.util.List;
import java.util.logging.Logger;

import com.google.protobuf.Message;
import com.google.protobuf.Parser;

public class Subscriber<T extends Message> {
	private String topic, msgType;
	private SubscriberCallback<T> cb;
	private Parser<T> deserializer;
	@SuppressWarnings("unused")
	private String localHost;
	@SuppressWarnings("unused")
	private int localPort;
	private List<Connection> connections;
	private static final Logger LOG = Logger.getLogger("Gazebo Transport");

	@SuppressWarnings("unchecked")
	public Subscriber(String topic, String msgType, SubscriberCallback<T> cb,
			T deserializer, String localHost, int localPort) {
		this.topic = topic;
		this.msgType = msgType;
		this.cb = cb;
		this.deserializer = (Parser<T>) deserializer.getParserForType();
		this.localHost = localHost;
		this.localPort = localPort;
		connections = new LinkedList<>();
	}

	public void connect(final PublisherRecord pub) {
		new Thread(new Runnable() {
			@Override
			public void run() {
				handle_connect(pub);
			}
		}).start();
	}

	private void handle_connect(PublisherRecord pub) {
		LOG.fine(String.format("CONN for %s from %s:%s\n", topic, pub.getHost(),
									pub.getPort()));
		Connection conn = new Connection();
		try {
			conn.connect(pub.getHost(), pub.getPort());
			connections.add(conn);

			Subscribe sub = Subscribe.newBuilder().setTopic(topic)
					.setMsgType(msgType).setHost(pub.getHost())
					.setPort(pub.getPort()).setLatching(false).build();
			conn.writePacket("sub", sub);

			while (true) {
				byte[] data = conn.rawRead();
				if (data == null) {
					connections.remove(conn);
					return;
				}
				T msg = deserializer.parseFrom(data);
				cb.callback(msg);
			}
		} catch (IOException e) {
			// FIXME: Connection lost, let's make sure it's closed and complain.
			// Hopefully they try to reconnect, maybe we should try to recover better?
			try {
				conn.close();
			} catch (IOException e1) {
			}
			e.printStackTrace();
		}
	}
}
