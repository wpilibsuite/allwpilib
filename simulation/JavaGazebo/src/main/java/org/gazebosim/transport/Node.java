/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2016. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package org.gazebosim.transport;

import gazebo.msgs.GzPacket.Packet;
import gazebo.msgs.GzPublish.Publish;
import gazebo.msgs.GzPublishers.Publishers;
import gazebo.msgs.GzString;
import gazebo.msgs.GzSubscribe.Subscribe;
import gazebo.msgs.StringV.String_V;

import java.io.IOException;
import java.net.SocketException;
import java.net.UnknownHostException;
import java.util.HashMap;
import java.util.LinkedList;
import java.util.List;
import java.util.Map;
import java.util.logging.ConsoleHandler;
import java.util.logging.Formatter;
import java.util.logging.Handler;
import java.util.logging.Level;
import java.util.logging.LogRecord;
import java.util.logging.Logger;

import com.google.protobuf.InvalidProtocolBufferException;
import com.google.protobuf.Message;

public class Node implements Runnable, ServerCallback {
	private final String name;
	private final Connection master = new Connection();
	private final Connection server = new Connection();
	private final List<String> namespaces = new LinkedList<>();
	private final Map<String, PublisherRecord> publishers = new HashMap<>();
	@SuppressWarnings("rawtypes")
	private final Map<String, Subscriber> subscriptions = new HashMap<>();
	private static final Logger LOG = Logger.getLogger("Gazebo Transport");
	static {
		// Get rid of the excess information
		LOG.setLevel(Level.WARNING);
		Handler[] handlers = LOG.getParent().getHandlers();
		if (handlers[0] instanceof ConsoleHandler) {
			((ConsoleHandler) handlers[0]).setFormatter(new Formatter() {
				@Override
				public String format(LogRecord record) {
					return String.format("%s|%s: %s\n", record.getLevel(), record.getLoggerName(), record.getMessage());
				}
			});
		}
	}

	public Node(String name) {
		this.name = name;
	}

	public void waitForConnection() throws IOException, InterruptedException {
		//enable user to change master uri via environment variable GAZEBO_MASTER_URI
		String user_defined_uri = System.getenv("GAZEBO_MASTER_URI");
		String gazebo_master_uri = "localhost";
		int port = 11345;
		if (user_defined_uri != null) {
			String[] parts = user_defined_uri.split(":");
			if (parts.length != 2){
				LOG.severe("invalid GAZEBO_MASTER_URI " + user_defined_uri+ ". URI must be of the form HOSTNAME:PORT");
				LOG.warning("using default GAZEBO_MASTER_URI=localhost:11345");
			}
			else {
				gazebo_master_uri = parts[0];
				port = Integer.parseInt(parts[1]);
			}
		}

		server.serve(this);

		LOG.info("GAZEBO_MASTER_URI is host=" + gazebo_master_uri + " port="+port);

		master.connectAndWait(gazebo_master_uri, port);

		initializeConnection();

		new Thread(this).start();
		LOG.info("Serving on: "+server.host+":"+server.port);
	}

	public synchronized <T extends Message> Publisher<T> advertise(String topic, T defaultMessage) {
		topic = fixTopic(topic);
		LOG.info("ADV "+topic);
		String type = defaultMessage.getDescriptorForType().getFullName();
		Publisher<T> pub = new Publisher<T>(topic, type, server.host, server.port);
		publishers.put(topic, pub);

		Publish req = Publish.newBuilder().setTopic(topic).setMsgType(type)
						.setHost(server.host).setPort(server.port).build();
		try {
			master.writePacket("advertise", req);
		} catch (IOException e) {
			e.printStackTrace(); // FIXME: Shouldn't happen, should probably complain louder
		}
		return pub;
	}

	public synchronized <T extends Message> Subscriber<T>
			subscribe(String topic, T defaultMessage, SubscriberCallback<T> cb) {
		topic = fixTopic(topic);
		LOG.info("SUB "+topic);
		if (subscriptions.containsKey(topic)) {
			throw new RuntimeException("Multiple subscribers for: "+topic);
		}

		String type = defaultMessage.getDescriptorForType().getFullName();
		Subscribe req = Subscribe.newBuilder().setTopic(topic).setMsgType(type)
							.setHost(server.host).setPort(server.port).setLatching(false).build();
		try {
			master.writePacket("subscribe", req);
		} catch (IOException e) {
			e.printStackTrace(); // FIXME: Shouldn't happen, should probably complain louder
		}

		Subscriber<T> s = new Subscriber<>(topic, type, cb, defaultMessage,
				server.host, server.port);
		subscriptions.put(topic, s);
		for (PublisherRecord p : publishers.values()) {
			if (p.getTopic().equals(topic)) {
				s.connect(p);
			}
		}
		return s;
	}

	@Override
	public void run() {
		try {
			while (true) {
				Packet packet = master.read();
				if (packet == null) {
					LOG.severe("Received null packet, shutting down connection to master.");
					master.close();
					return;
				}
				processPacket(packet);
			}
		} catch (IOException e ) {
			LOG.severe("I/O error: " + e);
			e.printStackTrace(); // FIXME: Log
		}
	}

	private synchronized void initializeConnection() throws IOException {
		Packet initData = master.read();
		if (!initData.getType().equals("version_init")) {
			throw new IOException("Expected 'version_init' packet, got '"+initData.getType()+"'.");
		}
		GzString.String version = GzString.String.parseFrom(initData.getSerializedData());
		LOG.info("Version: "+version.getData()); // TODO: Check version

		Packet namespaceData = master.read();
		String_V ns = String_V.parseFrom(namespaceData.getSerializedData());
		namespaces.addAll(ns.getDataList());
		LOG.info(namespaces.toString());

		Packet publisherData = master.read();
		if (publisherData.getType().equals("publishers_init")) {
			Publishers pubs = Publishers.parseFrom(publisherData.getSerializedData());
			for (Publish pub : pubs.getPublisherList()) {
				PublisherRecord record = new RemotePublisherRecord(pub);
				publishers.put(record.getTopic(), record);
			}
			LOG.info(publishers.toString());
		} else {
			LOG.severe("No publisher data received.");
		}
	}

	public synchronized void processPacket(Packet packet) throws InvalidProtocolBufferException {
		if (packet.getType().equals("publisher_add")) {
			PublisherRecord pub = new RemotePublisherRecord(Publish.parseFrom(packet.getSerializedData()));

			if (pub.getHost().equals(server.host) && pub.getPort() == server.port) {
				LOG.info("ACK "+pub.getTopic());
				return; // This is us
			}

			LOG.info("New Publisher: "+pub.getTopic());
			LOG.fine("Publisher: "+Publish.parseFrom(packet.getSerializedData()));
			publishers.put(pub.getTopic(), pub);
		} else if (packet.getType().equals("publisher_subscribe") ||
				   packet.getType().equals("publisher_advertise")) {
			PublisherRecord pub = new RemotePublisherRecord(Publish.parseFrom(packet.getSerializedData()));

			if (pub.getHost().equals(server.host) && pub.getPort() == server.port) {
				LOG.info("Ignoring subscription request on (local) "+pub.getTopic());
				return; // This is us
			}

			LOG.info("PUBSUB found for "+pub.getTopic());
			LOG.fine("Publisher: "+Publish.parseFrom(packet.getSerializedData()));
			subscriptions.get(pub.getTopic()).connect(pub);
		} else if (packet.getType().equals("topic_namespace_add")) {
			namespaces.add(GzString.String.parseFrom(packet.getSerializedData()).getData());
			LOG.info("New Namespace: "+namespaces.get(namespaces.size()-1));
		} else if (packet.getType().equals("unsubscribe")) {
			Subscribe sub = Subscribe.parseFrom(packet.getSerializedData());
			LOG.warning("Ignoring unsubscribe: "+sub);
		} else {
			LOG.warning("Can't handle "+packet.getType());
		}
	}

	@Override
	/**
	 * This is called when another node requests subscription to a topic we are publishing
	 */
	public void handle(Connection conn) throws IOException {
		LOG.fine("Handling new connection");
		Packet msg = conn.read();
		if (msg == null) {
			LOG.warning("Read null message.");
			return;
		}

		if (msg.getType().equals("sub")) {
			Subscribe sub = Subscribe.parseFrom(msg.getSerializedData());
			if (!publishers.containsKey(sub.getTopic())) {
				LOG.severe("Subscription for unknown topic: "
						+ sub.getTopic());
				return;
			}
			LOG.fine("New connection for topic="+sub.getTopic());

			PublisherRecord pub = publishers.get(sub.getTopic());
			if (!pub.getMsgType().equals(sub.getMsgType())) {
				LOG.severe(String.format("Message type mismatch requested=%d publishing=%s\n",
										 pub.getMsgType(), sub.getMsgType()));
				return;
			}

			LOG.info("CONN " + sub.getTopic());
			//Tell the publisher that it has recieved a connection from a subscriver
			pub.connect(conn);
		} else {
			LOG.warning("Unknown message type: " + msg.getType());
		}
	}

	private String fixTopic(String topic) {
		return "/gazebo/" + name + "/" + topic;
	}
}
