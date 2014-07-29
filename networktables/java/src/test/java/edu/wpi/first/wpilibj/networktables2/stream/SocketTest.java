package edu.wpi.first.wpilibj.networktables2.stream;

import static org.junit.Assert.*;

import java.io.*;
import java.util.concurrent.*;
import java.util.concurrent.atomic.*;

import org.jmock.*;
import org.jmock.integration.junit4.*;
import org.jmock.lib.concurrent.*;
import org.junit.*;

import test.util.*;

public class SocketTest {
	Mockery context = new JUnit4Mockery() {{
		setThreadingPolicy(new Synchroniser());
	}};


	@Test(timeout=1500)
	public void testSingleSocketConnection() throws Throwable {
		doSocketTest(1, 500, 1500, SocketStreams.newStreamProvider(10111), SocketStreams.newStreamFactory("localhost", 10111));
	}
	@Test(timeout=2500)
	public void testManySocketConnection() throws Throwable {
		doSocketTest(10, 500, 2000, SocketStreams.newStreamProvider(10112), SocketStreams.newStreamFactory("localhost", 10112));
	}

	private void doSocketTest(final int count, long threadTimeout, long testTimeout, final IOStreamProvider streamProvider, final IOStreamFactory factory) throws Throwable {
		TestExecutor executor = new TestExecutor(count+1, threadTimeout);
		final AtomicInteger clientCount = new AtomicInteger(0);
		final AtomicInteger serverCount = new AtomicInteger(0);

		for(int i = 0; i<count; ++i){
			executor.execute(new Runnable() {
				@Override
				public void run() {
					try{
						IOStream clientStream = factory.createStream();
						
						PrintWriter output = new PrintWriter(clientStream.getOutputStream());
						output.println("FromClient");
						output.flush();

						BufferedReader input = new BufferedReader(new InputStreamReader(clientStream.getInputStream()));
						assertEquals("FromServer", input.readLine());
						
						clientStream.close();
						clientCount.incrementAndGet();
					} catch(IOException e){
						e.printStackTrace();
					}
				}
			});
		}

		executor.execute(new Runnable() {
			@Override
			public void run() {
				try{
					while(serverCount.get()!=count){
						IOStream serverStream = streamProvider.accept();
						
						BufferedReader input = new BufferedReader(new InputStreamReader(serverStream.getInputStream()));
						assertEquals("FromClient", input.readLine());

						PrintWriter output = new PrintWriter(serverStream.getOutputStream());
						output.println("FromServer");
						output.flush();
						
						serverStream.close();
						serverCount.incrementAndGet();
					}
					streamProvider.close();
				} catch(IOException e){
					e.printStackTrace();
				}
			}
		});
		
		executor.await(testTimeout, TimeUnit.MILLISECONDS);
		assertEquals("server count", count, serverCount.get());
		assertEquals("client count", count, clientCount.get());
	}

}
