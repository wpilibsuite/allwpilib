package test.util;

import java.io.*;

import edu.wpi.first.wpilibj.networktables2.stream.*;

public class ByteArrayStream implements IOStream {

	private final ByteArrayOutputStream os;
	private final ByteArrayInputStream is;

	public ByteArrayStream()  {
		this(new byte[0]);
	}
	public ByteArrayStream(final byte[] inputData) {
		this(new ByteArrayOutputStream(), new ByteArrayInputStream(inputData));
	}
	public ByteArrayStream(final ByteArrayOutputStream os, final byte[] inputData) {
		this(os, new ByteArrayInputStream(inputData));
	}
	public ByteArrayStream(final ByteArrayOutputStream os, final ByteArrayInputStream is) {
		this.is = is;
		this.os = os;
	}
	
	public byte[] getOutput(){
		return os.toByteArray();
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
		} catch(IOException e){}
		try {
			os.close();
		} catch (IOException e){}
	}

}
