package edu.wpi.first.wpilibj.networktables2.client;

import java.io.*;

import org.jmock.*;
import org.jmock.integration.junit4.*;
import org.jmock.lib.legacy.*;
import org.junit.*;
import org.junit.runner.*;

import edu.wpi.first.wpilibj.networktables2.stream.*;
import edu.wpi.first.wpilibj.networktables2.thread.*;
import edu.wpi.first.wpilibj.networktables2.type.*;

@RunWith(JMock.class)
public class ClientConnectionManagementTest {
	Mockery context = new JUnit4Mockery() {{
        setImposteriser(ClassImposteriser.INSTANCE);
    }};
	
	@Test public void testReconnect() {
		final ClientConnectionListenerManager connectionListenerManager = context.mock(ClientConnectionListenerManager.class);
		final ClientNetworkTableEntryStore entryStore = context.mock(ClientNetworkTableEntryStore.class);
		final IOStreamFactory streamFactory = context.mock(IOStreamFactory.class);
		final IOStream stream1 = context.mock(IOStream.class, "stream1");
		final IOStream stream2 = context.mock(IOStream.class, "stream2");
		final NTThread thread1 = context.mock(NTThread.class, "thread1");
		final NTThread thread2 = context.mock(NTThread.class, "thread2");
		final NTThreadManager threadManager = context.mock(NTThreadManager.class);

		context.checking(new Expectations() {{
			try{
				allowing(stream1).getInputStream();
				allowing(stream1).getOutputStream();
				
				oneOf(entryStore).clearIds();
				oneOf(streamFactory).createStream();will(returnValue(stream1));
				oneOf(threadManager).newBlockingPeriodicThread(with(any(PeriodicRunnable.class)), with("Client Connection Reader Thread"));will(returnValue(thread1));
			} catch(IOException e){}
		}});
		
		ClientConnectionAdapter client = new ClientConnectionAdapter(entryStore, threadManager, streamFactory, connectionListenerManager, new NetworkTableEntryTypeManager());
		client.reconnect();
		
		context.checking(new Expectations() {{
			try{
				allowing(stream2).getInputStream();
				allowing(stream2).getOutputStream();
				
				oneOf(entryStore).clearIds();
				oneOf(thread1).stop();
				oneOf(stream1).close();
				oneOf(streamFactory).createStream();will(returnValue(stream2));
				oneOf(threadManager).newBlockingPeriodicThread(with(any(PeriodicRunnable.class)), with("Client Connection Reader Thread"));will(returnValue(thread2));
			} catch(IOException e){}
		}});
		client.reconnect();
	}
	
	@Test public void testClose() {
		final ClientConnectionListenerManager connectionListenerManager = context.mock(ClientConnectionListenerManager.class);
		final ClientNetworkTableEntryStore entryStore = context.mock(ClientNetworkTableEntryStore.class);
		final IOStreamFactory streamFactory = context.mock(IOStreamFactory.class);
		final IOStream stream1 = context.mock(IOStream.class);
		final NTThread thread1 = context.mock(NTThread.class, "thread1");
		final NTThreadManager threadManager = context.mock(NTThreadManager.class);

		context.checking(new Expectations() {{
			try{
				allowing(stream1).getInputStream();
				allowing(stream1).getOutputStream();
				
				oneOf(entryStore).clearIds();
				oneOf(streamFactory).createStream();will(returnValue(stream1));
				oneOf(threadManager).newBlockingPeriodicThread(with(any(PeriodicRunnable.class)), with("Client Connection Reader Thread"));will(returnValue(thread1));
			}catch(IOException e){}
		}});
		
		ClientConnectionAdapter client = new ClientConnectionAdapter(entryStore, threadManager, streamFactory, connectionListenerManager, new NetworkTableEntryTypeManager());
		client.reconnect();
		context.checking(new Expectations() {{
			oneOf(entryStore).clearIds();
			oneOf(thread1).stop();
			oneOf(stream1).close();
		}});
		client.close();
	}
	

	@Test public void testFailCreateStream() {
		final ClientNetworkTableEntryStore entryStore = context.mock(ClientNetworkTableEntryStore.class);
		final IOStreamFactory streamFactory = context.mock(IOStreamFactory.class);
		final IOStream stream1 = context.mock(IOStream.class, "stream1");
		final NTThread thread1 = context.mock(NTThread.class, "thread1");
		final NTThreadManager threadManager = context.mock(NTThreadManager.class);
		final ClientConnectionListenerManager connectionListenerManager = context.mock(ClientConnectionListenerManager.class);

		context.checking(new Expectations() {{
			try{
				allowing(stream1).getInputStream();
				allowing(stream1).getOutputStream();
				
				oneOf(entryStore).clearIds();
				oneOf(streamFactory).createStream();will(returnValue(stream1));
				oneOf(threadManager).newBlockingPeriodicThread(with(any(PeriodicRunnable.class)), with("Client Connection Reader Thread"));will(returnValue(thread1));
			} catch(IOException e){}
		}});
		
		ClientConnectionAdapter client = new ClientConnectionAdapter(entryStore, threadManager, streamFactory, connectionListenerManager, new NetworkTableEntryTypeManager());
		client.reconnect();
		context.checking(new Expectations() {{
			try{
				oneOf(entryStore).clearIds();
				oneOf(thread1).stop();
				oneOf(stream1).close();
				oneOf(streamFactory).createStream();will(returnValue(null));
			} catch(IOException e){}
		}});
		client.reconnect();
	}

}
