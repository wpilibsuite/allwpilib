package edu.wpi.first.wpilibj.networktables2.stream;

import java.io.*;

/**
 * 
 * @author mwills
 *
 */
public class SocketStreamFactory implements IOStreamFactory{

	private final String host;
	private final int port;

	public SocketStreamFactory(String host, int port) throws IOException {
		this.host = host;
		this.port = port;
	}

	public IOStream createStream() throws IOException {
		return new SocketStream(host, port);
	}

}
