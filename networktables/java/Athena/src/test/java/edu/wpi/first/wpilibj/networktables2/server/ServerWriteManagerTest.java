package edu.wpi.first.wpilibj.networktables2.server;

import java.io.*;

import org.jmock.*;
import org.jmock.integration.junit4.*;
import org.jmock.lib.legacy.*;
import org.junit.*;
import org.junit.runner.*;

import test.util.*;
import edu.wpi.first.wpilibj.networktables2.*;
import edu.wpi.first.wpilibj.networktables2.thread.*;

@RunWith(JMock.class)
public class ServerWriteManagerTest {
	Mockery context = new JUnit4Mockery(){{
        setImposteriser(ClassImposteriser.INSTANCE);
    }};

	@Test public void testSendTransaction() throws Exception {
		final TestThreadManager threadManager = new TestThreadManager();
		final ServerConnectionList connectionList = context.mock(ServerConnectionList.class);
		final NetworkTableEntry entry1 = context.mock(NetworkTableEntry.class, "entry1");
		final NetworkTableEntry entry2 = context.mock(NetworkTableEntry.class, "entry2");
		final NetworkTableEntry entry3 = context.mock(NetworkTableEntry.class, "entry3");
		final AbstractNetworkTableEntryStore entryStore = context.mock(AbstractNetworkTableEntryStore.class);
		
		
		WriteManager writeManager = new WriteManager(connectionList, threadManager, entryStore, Long.MAX_VALUE);
		writeManager.start();

		writeManager.offerOutgoingAssignment(entry1);
		
		final Sequence writeSequence1 = context.sequence("writeSequence1");
		context.checking(new Expectations() {{
			oneOf(connectionList).offerOutgoingAssignment(entry1);inSequence(writeSequence1);
			oneOf(connectionList).flush();inSequence(writeSequence1);
			oneOf(entry1).makeClean();
		}});
		threadManager.getPeriodicThread(WriteManager.class).run();
		context.assertIsSatisfied();
		
		
		writeManager.offerOutgoingAssignment(entry2);
		writeManager.offerOutgoingAssignment(entry3);
		final Sequence writeSequence2 = context.sequence("writeSequence2");
		context.checking(new Expectations() {{
			oneOf(connectionList).offerOutgoingAssignment(entry2);inSequence(writeSequence2);
			oneOf(connectionList).offerOutgoingAssignment(entry3);inSequence(writeSequence2);
			oneOf(connectionList).flush();inSequence(writeSequence2);
			oneOf(entry2).makeClean();
			oneOf(entry3).makeClean();
		}});
		threadManager.getPeriodicThread(WriteManager.class).run();
	}

	@Test public void testClose() throws IOException {
		final ServerConnectionList connectionList = context.mock(ServerConnectionList.class);
		final NTThreadManager threadManager = context.mock(NTThreadManager.class);
		final NTThread thread = context.mock(NTThread.class);
		final AbstractNetworkTableEntryStore entryStore = context.mock(AbstractNetworkTableEntryStore.class);
		
		context.checking(new Expectations() {{
			oneOf(threadManager).newBlockingPeriodicThread(with(any(PeriodicRunnable.class)), with("Write Manager Thread"));will(returnValue(thread));
		}});
		
		WriteManager writeManager = new WriteManager(connectionList, threadManager, entryStore, Long.MAX_VALUE);
		
		writeManager.start();
		context.assertIsSatisfied();

		
		context.checking(new Expectations() {{
			oneOf(thread).stop();
		}});
		writeManager.stop();
	}
	
}
