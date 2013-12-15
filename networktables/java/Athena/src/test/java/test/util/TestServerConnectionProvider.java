package test.util;

import java.io.*;
import java.util.concurrent.*;

import edu.wpi.first.wpilibj.networktables2.stream.*;

public class TestServerConnectionProvider implements IOStreamProvider{
	private final BlockingQueue<IOStream> connections = new LinkedBlockingQueue<IOStream>();

	public void supply(IOStream stream){
		connections.offer(stream);
	}

	public IOStream accept() {
		try {
			return connections.take();
		} catch (InterruptedException e) {
			return null;
		}
	}

	@Override
	public void close() throws IOException {
	}
}
