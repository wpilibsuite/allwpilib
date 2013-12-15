package java.io;

public class BufferedInputStream extends InputStream{
	private final InputStream source;
	private byte[] buffer;
	private int pos;//the next position to be read
	private int maxPos;//the maximum valid position in the buffer + 1

	public BufferedInputStream(InputStream source){
		this(source, 8192);
	}
	public BufferedInputStream(InputStream source, int size){
		this.source = source;
		buffer = new byte[size];
		pos = 0;
		maxPos = 0;
	}
	
	
	private void fillBuffer() throws IOException {
		int numRemaining = maxPos-pos;
		System.arraycopy(buffer, pos, buffer, 0, numRemaining);
		pos = 0;
		maxPos = numRemaining;
		int numRead = source.read(buffer, numRemaining, buffer.length-numRemaining);
		maxPos += numRead;
	}

	public int read() throws IOException {
		if(pos<maxPos)
			return buffer[pos++]&0xFF;//cast to make sure not to return -1 and instead just return lowest byte of int
		fillBuffer();
		if(pos<maxPos)
			return buffer[pos++]&0xFF;
		return -1;
	}
	

	public int available() throws IOException{
		return maxPos-pos+source.available();
	}

	public void close() throws IOException{
		source.close();
	}

}
