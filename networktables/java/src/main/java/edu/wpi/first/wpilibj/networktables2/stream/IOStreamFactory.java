package edu.wpi.first.wpilibj.networktables2.stream;

import java.io.*;

/**
 * A factory that will create the same IOStream. A stream returned by this factory should be closed before calling createStream again
 * 
 * @author Mitchell
 *
 */
public interface IOStreamFactory {
	/**
	 * @return create a new stream
	 * @throws IOException
	 */
	public IOStream createStream() throws IOException;
}
