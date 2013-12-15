package edu.wpi.first.wpilibj.networktables2.stream;

import java.io.*;

/**
 * An object that will provide the IOStream of clients to a NetworkTable Server
 * 
 * @author mwills
 *
 */
public interface IOStreamProvider {
	/**
	 * 
	 * @return a new IOStream normally from a server
	 * @throws IOException
	 */
	IOStream accept() throws IOException;
	/**
	 * Close the source of the IOStreams. {@link #accept()} should not be called after this is called
	 * @throws IOException
	 */
	void close() throws IOException;
}
