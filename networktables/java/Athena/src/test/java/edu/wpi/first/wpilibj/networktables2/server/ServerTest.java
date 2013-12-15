package edu.wpi.first.wpilibj.networktables2.server;

import java.io.*;

import org.jmock.*;
import org.jmock.integration.junit4.*;
import org.junit.*;
import org.junit.runner.*;

import edu.wpi.first.wpilibj.networktables2.stream.*;
import edu.wpi.first.wpilibj.networktables2.thread.*;
import edu.wpi.first.wpilibj.networktables2.type.*;

@RunWith(JMock.class)
public class ServerTest {
	Mockery context = new JUnit4Mockery();


	@Test public void testClose() throws IOException {
		final IOStreamProvider streamProvider = context.mock(IOStreamProvider.class);
		final NTThreadManager threadManager = context.mock(NTThreadManager.class);
		final NTThread thread1 = context.mock(NTThread.class, "thread 1");
		final NTThread thread2 = context.mock(NTThread.class, "thread 2");

		context.checking(new Expectations() {{
			oneOf(threadManager).newBlockingPeriodicThread(with(any(PeriodicRunnable.class)), with("Write Manager Thread"));will(returnValue(thread1));
			oneOf(threadManager).newBlockingPeriodicThread(with(any(PeriodicRunnable.class)), with("Server Incoming Stream Monitor Thread"));will(returnValue(thread2));
		}});
		
		NetworkTableServer server = new NetworkTableServer(streamProvider, new NetworkTableEntryTypeManager(), threadManager);
		context.checking(new Expectations() {{
			oneOf(thread1).stop();
			oneOf(thread2).stop();
			oneOf(streamProvider).close();
		}});
		server.close();
	}

}
