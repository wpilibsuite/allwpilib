package edu.wpi.first.wpilibj.networktables2.stream;

import java.io.*;

import javax.microedition.io.*;

/**
 * A socket connection on the cRIO
 * 
 * @author mwills
 *
 */
public class SocketConnectionStream implements IOStream{

	private final SocketConnection socket;
	private final InputStream is;
	private final OutputStream os;

	/**
	 * Create a new IOStream for a socket connection with the given host and port
	 * @param host
	 * @param port
	 * @throws IOException
	 */
	public SocketConnectionStream(final String host, final int port) throws IOException {
		this((SocketConnection) Connector.open("socket://"+host+":"+port));
	}
	/**
	 * Create a new IOStream for a socket connection
	 * @param socket
	 * @throws IOException
	 */
	public SocketConnectionStream(final SocketConnection socket) throws IOException {
		this.socket = socket;
		is = socket.openInputStream();
		os = socket.openOutputStream();
	}


	public InputStream getInputStream() {
		return is;
	}
	public OutputStream getOutputStream() {
		return os;
	}
	public void close() {
		try{
			is.close();
		} catch(IOException e){
			//just ignore and close the rest of the stream
		}
		try{
			os.close();
		} catch(IOException e){
			//just ignore and close the rest of the stream
		}
		try{
			socket.close();
		} catch(IOException e){
			//just ignore and assume socket is now closed
		}
	}

}
