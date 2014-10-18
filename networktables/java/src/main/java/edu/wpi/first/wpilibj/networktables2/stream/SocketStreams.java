package edu.wpi.first.wpilibj.networktables2.stream;

import java.io.*;

public final class SocketStreams {
	private SocketStreams(){}
	
	public static IOStreamFactory newStreamFactory(String host, int port) throws IOException{
		return new SocketStreamFactory(host, port);
	}

	public static IOStreamProvider newStreamProvider(int port) throws IOException {
		return new SocketServerStreamProvider(port);
	}
}
