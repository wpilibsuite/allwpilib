package edu.wpi.first.wpilibj.networktables2.stream;

import java.io.*;

/**
 * 
 * @author mwills
 *
 */
public class SocketConnectionStreamFactory implements IOStreamFactory{

	private final String host;
	private final int port;

	/**
	 * Create a new factory that will create socket connections with the given host and port
	 * @param host
	 * @param port
	 * @throws IOException
	 */
	public SocketConnectionStreamFactory(final String host, final int port) throws IOException {
		this.host = host;
		this.port = port;
	}

	public IOStream createStream() throws IOException {
		return new SocketConnectionStream(host, port);
	}

}
