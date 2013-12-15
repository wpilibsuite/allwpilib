package test.util;

import java.io.*;
import java.util.concurrent.*;

import edu.wpi.first.wpilibj.networktables2.stream.*;

public class StreamPipeProvider implements IOStreamProvider{
	private final StreamPipeFactory factory;
	private boolean hasAccepted = false;
	private final CountDownLatch closedLatch = new CountDownLatch(1);
	public StreamPipeProvider(){
		try {
			factory = new StreamPipeFactory();
		} catch (IOException e) {
			throw new RuntimeException(e);
		}
	}
	@Override
	public IOStream accept() throws IOException {
		if(!hasAccepted){
			hasAccepted = true;
			return factory.getServerStream();
		}
		try {
			closedLatch.await();
			
		} catch (InterruptedException e) {}
		return null;
	}

	@Override
	public void close() throws IOException {
		closedLatch.countDown();
	}
	
	public StreamPipeFactory getFactory(){
		return factory;
	}
	
	
	
	public class StreamPipeFactory implements IOStreamFactory{
		private final PipedOutputStream serverOut;
		private final PipedInputStream serverIn;
		
		private final PipedOutputStream clientOut;
		private final PipedInputStream clientIn;
		
		private final IOStream serverStream;
		private final IOStream clientStream;
		
		public StreamPipeFactory() throws IOException {
			clientIn = new PipedInputStream(serverOut = new PipedOutputStream());
			serverIn = new PipedInputStream(clientOut = new PipedOutputStream());
	
			serverStream = new SimpleIOStream(serverIn, serverOut);
			clientStream = new SimpleIOStream(clientIn, clientOut);
		}
		
		public IOStream getServerStream(){
			return serverStream;
		}
		
		public IOStream getClientStream(){
			return clientStream;
		}
	
		@Override
		public IOStream createStream() throws IOException {
			return getClientStream();
		}
	}
}