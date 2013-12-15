package test.util;

import java.io.*;
import java.util.*;

public class LoggingOutputStream extends OutputStream {

	private final OutputStream backingStream;
	private final ByteArrayOutputStream buffer;

	public LoggingOutputStream(OutputStream backingStream){
		this.backingStream = backingStream;
		buffer = new ByteArrayOutputStream();
	}
	
	public void close() throws IOException {
		backingStream.close();
		System.out.println("Closed "+backingStream+" with "+Arrays.toString(buffer.toByteArray()));
		buffer.reset();
	}

	public void flush() throws IOException {
		backingStream.flush();
		System.out.println("Wrote "+Arrays.toString(buffer.toByteArray())+" to "+backingStream);
		buffer.reset();
	}

	public void write(byte[] b) throws IOException {
		backingStream.write(b);
		buffer.write(b);
	}

	public void write(byte[] b, int off, int len) throws IOException {
		backingStream.write(b, off, len);
		buffer.write(b, off, len);
	}

	public void write(int b) throws IOException {
		backingStream.write(b);
		buffer.write(b);
	}

}
