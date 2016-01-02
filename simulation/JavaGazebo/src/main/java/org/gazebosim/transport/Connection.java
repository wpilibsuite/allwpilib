/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2016. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package org.gazebosim.transport;

import gazebo.msgs.GzPacket.Packet;
import gazebo.msgs.GzTime.Time;

import java.io.IOException;
import java.net.ConnectException;
import java.net.InetAddress;
import java.io.InputStream;
import java.io.OutputStream;
import java.net.ServerSocket;
import java.net.Socket;
import java.net.UnknownHostException;
import java.util.logging.Logger;
import java.util.logging.Level;

import com.google.protobuf.ByteString;
import com.google.protobuf.Message;

/**
 * Manages a Gazebo protocol connection.
 *
 * This can connect to either the Gazebo server, or to a data
 * publisher. Additionally, it can act as the TCP client, or as a
 * server. In either case, it provides methods to read and write
 * structured data on the socket.
 */
public class Connection {
	private static int HEADER_SIZE = 8;
	
	public String host;
	public int port;
	
	private Socket socket;
	private ServerSocket ssocket;
	private InputStream is;
	private OutputStream os;
	
	private static final Logger LOG = Logger.getLogger("Gazebo Transport");

	public void connect(String host, int port) throws UnknownHostException, IOException {
		this.host = host;
		this.port = port;
		socket = new Socket(host, port);
		is = socket.getInputStream();
		os = socket.getOutputStream();
	}

	public void connectAndWait(String host, int port) throws IOException, InterruptedException {
		this.host = host;
		this.port = port;
		while (true) {
			try {
				socket = new Socket(host, port);
				break;
			} catch (ConnectException ex) {
				// Retry.
				LOG.log(Level.WARNING, "Cannot connect, retrying in five seconds.", ex);
				Thread.sleep(5000);
			}
		}
		is = socket.getInputStream();
		os = socket.getOutputStream();
	}

	public void serve(final ServerCallback cb) throws IOException {
		ssocket = new ServerSocket(0);
		host = ssocket.getInetAddress().getHostAddress();
		port = ssocket.getLocalPort();

		//enable user to change master uri via environment variable GAZEBO_MASTER_URI
		//TODO : allow for automatic guesing of IP. Look at Connection.cc in gazebo for C++ example
		String user_defined_ip = System.getenv("GAZEBO_IP");
        if (user_defined_ip != null) {
          host = InetAddress.getByName(user_defined_ip).getHostAddress();;
          LOG.warning("Using custom host: "+host);
        }

		new Thread("Gazebo Server Thread") {
			@Override
			public void run() {
				LOG.config("Listening on "+host+":"+port);
				while (true) {
					Connection conn = new Connection();
					try {
						conn.socket = ssocket.accept();
						conn.is = conn.socket.getInputStream();
						conn.os = conn.socket.getOutputStream();
						LOG.info("Handling connect from "+conn.socket.getInetAddress());
						cb.handle(conn);
					} catch (IOException e) {
						LOG.log(Level.WARNING, "Cannot handle client", e);
					}
				}
			}
		}.start();
	}

	public void close() throws IOException {
		LOG.info("Closing connection");
		if (socket != null) {
			socket.close();
			socket = null;
		}
		if (ssocket != null) {
			ssocket.close();
			ssocket = null;
		}
	}
	
	public byte[] rawRead() throws IOException {
		synchronized (is) {
			// Figure out the message size
			byte[] buff= new byte[HEADER_SIZE];
			int n = is.read(buff);
			if (n != HEADER_SIZE) {
				LOG.severe("Only read "+n+" bytes instead of 8 for header.");
				return null;
			}
			int size = Integer.parseInt(new String(buff), 16);
		
			// Read in the actual message
			buff = new byte[size];
			n = is.read(buff);
			if (n != size) {
				throw new IOException("Failed to read whole message");
			}
			
			return buff;
		}
	}
	
	public Packet read() throws IOException {
		byte[] buff = rawRead();
		if (buff == null) {
			return null;
		}
		return Packet.parseFrom(buff);
	}

	public void write(Message msg) throws IOException {
        ByteString data = msg.toByteString();
        ByteString header = ByteString.copyFromUtf8(String.format("%08X", data.size()));
        ByteString bytes = header.concat(data);

        synchronized (os) {
			os.write(bytes.toByteArray());
		}
	}

	public void writePacket(String name, Message req) throws IOException {
		long ms = System.currentTimeMillis();
		Time t = Time.newBuilder().setSec((int) (ms / 1000)).setNsec((int) ((ms%1000)*1000)).build();
		Packet pack = Packet.newBuilder().setType(name).setStamp(t)
							.setSerializedData(req.toByteString()).build();
		write(pack);
	}

}
