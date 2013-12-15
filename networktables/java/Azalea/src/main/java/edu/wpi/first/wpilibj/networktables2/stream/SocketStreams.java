package edu.wpi.first.wpilibj.networktables2.stream;

import java.io.*;

/**
 * Static factory for socket stream factories and providers
 * 
 * @author Mitchell
 *
 */
public class SocketStreams {
	/**
	 * Create a new IOStream factory 
	 * @param host
	 * @param port
	 * @return a IOStreamFactory that will create Socket Connections on the given host and port
	 * @throws IOException
	 */
	public static IOStreamFactory newStreamFactory(final String host, final int port) throws IOException{
		return new SocketConnectionStreamFactory(host, port);
	}

	/**
	 * Create a new IOStream provider
	 * @param port
	 * @return an IOStreamProvider for a socket server on the given port
	 * @throws IOException
	 */
	public static IOStreamProvider newStreamProvider(final int port) throws IOException {
		return new SocketConnectionServerStreamProvider(port);
	}
}
