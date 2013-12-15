package edu.wpi.first.wpilibj.networktables2.stream;

import java.io.*;

import javax.microedition.io.*;


/**
 * An object that will provide cRIO socket connections when a client connects to the server on the given port
 * 
 * @author mwills
 *
 */
public class SocketConnectionServerStreamProvider implements IOStreamProvider{

	private final ServerSocketConnection server;
	
	/**
	 * Create a new Stream provider that wraps a Socket Server on the given port
	 * @param port
	 * @throws IOException
	 */
	public SocketConnectionServerStreamProvider(final int port) throws IOException{
		server = (ServerSocketConnection) Connector.open("socket://:" + port);
	}

	public IOStream accept() throws IOException {
		SocketConnection socket = (SocketConnection) server.acceptAndOpen();
		if(socket!=null){
			socket.setSocketOption(SocketConnection.LINGER, 0);
			return new SocketConnectionStream(socket);
		}
		return null;
	}

	public void close() throws IOException {
		server.close();
	}

}
