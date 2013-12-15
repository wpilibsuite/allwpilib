package edu.wpi.first.wpilibj.networktables2.stream;

import java.io.*;

/**
 * An abstraction for a bidirectional stream that a network table can connect through
 * 
 * @author mwills
 *
 */
public interface IOStream {
	
	/**
	 * @return the input stream for this IOStream
	 */
	public InputStream getInputStream();
	/**
	 * @return the output stream for this IOStream
	 */
	public OutputStream getOutputStream();
	/**
	 * completely close the stream
	 */
	public void close();
}
