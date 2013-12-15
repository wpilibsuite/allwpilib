package edu.wpi.first.wpilibj.networktables2.stream;

import java.io.*;

/**
 * An IOStream that wraps an {@link InputStream} and an {@link OutputStream}
 * @author Mitchell
 *
 */
public class SimpleIOStream implements IOStream{

	private final InputStream is;
	private final OutputStream os;

	/**
	 * Create a new SimpleIOStream
	 * @param is
	 * @param os
	 */
	public SimpleIOStream(final InputStream is, final OutputStream os){
		this.is = is;
		this.os = os;
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
			//just ignore the error and close the output stream
		}
		try{
			os.close();
		} catch(IOException e){
			//just ignore the error and assume everything is now closed
		}
	}

}
