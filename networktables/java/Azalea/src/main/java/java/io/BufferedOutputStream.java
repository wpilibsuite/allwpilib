package java.io;

public class BufferedOutputStream extends OutputStream{
	
	private final byte[] buffer;
	private int pos;
	private final OutputStream out;
	
	public BufferedOutputStream(OutputStream out){
		this(out, 8192);
	}
	public BufferedOutputStream(OutputStream out, int size){
		this.out = out;
		buffer = new byte[size];
		pos = 0;
	}
	private void flushBuffer() throws IOException{
		if(pos>0){
			out.write(buffer, 0, pos);
			pos = 0;
		}
	}

	public void write(int b) throws IOException {
		buffer[pos++] = (byte)b;
		if(pos>=buffer.length)
			flushBuffer();
	}
	
	public void write(byte b[], int off, int len) throws IOException {
    	if(len>=buffer.length){
    		flushBuffer();
    		out.write(b, off, len);
    	}
    	else{
    		if(len>=(buffer.length-pos))
    			flushBuffer();
    		
    		System.arraycopy(b, off, buffer, pos, len);
    		pos += len;
    	}
    }

	public void flush() throws IOException{
		flushBuffer();
		out.flush();
	}
	
	public void close() throws IOException{
		flushBuffer();
		out.close();
	}

}
