package edu.wpi.first.wpilibj.networktables2.server;

import edu.wpi.first.wpilibj.networktables2.*;
import java.io.*;
import org.jmock.*;
import org.jmock.integration.junit4.*;
import org.jmock.lib.legacy.*;
import org.junit.*;
import org.junit.runner.*;

@RunWith(JMock.class)
public class ConnectionListTest {
	Mockery context = new JUnit4Mockery(){{
        setImposteriser(ClassImposteriser.INSTANCE);
    }};

	@Test public void testSendTransaction() throws IOException {
		final ServerConnectionAdapter adapter1 = context.mock(ServerConnectionAdapter.class, "adapter 1");
		final ServerConnectionAdapter adapter2 = context.mock(ServerConnectionAdapter.class, "adapter 2");
		final NetworkTableEntry entry1 = context.mock(NetworkTableEntry.class, "entry1");
		final NetworkTableEntry entry2 = context.mock(NetworkTableEntry.class, "entry2");
		
		
		ServerConnectionList connectionList = new ServerConnectionList();
		connectionList.add(adapter1);
		connectionList.add(adapter2);
		
		context.checking(new Expectations() {{
			oneOf(adapter1).offerOutgoingAssignment(entry1);
			oneOf(adapter2).offerOutgoingAssignment(entry1);
		}});
		connectionList.offerOutgoingAssignment(entry1);
		context.assertIsSatisfied();
		
		context.checking(new Expectations() {{
			oneOf(adapter1).offerOutgoingAssignment(entry2);
			oneOf(adapter2).offerOutgoingAssignment(entry2);
		}});
		connectionList.offerOutgoingAssignment(entry2);
	}

	@Test public void testClose() throws IOException {
		final ServerConnectionAdapter adapter1 = context.mock(ServerConnectionAdapter.class, "adapter 1");
		final ServerConnectionAdapter adapter2 = context.mock(ServerConnectionAdapter.class, "adapter 2");
		final NetworkTableEntry entry1 = context.mock(NetworkTableEntry.class, "entry1");
		final NetworkTableEntry entry2 = context.mock(NetworkTableEntry.class, "entry2");
		
		
		ServerConnectionList connectionList = new ServerConnectionList();
		connectionList.add(adapter1);
		connectionList.add(adapter2);
		
		context.checking(new Expectations() {{
			oneOf(adapter1).offerOutgoingAssignment(entry1);
			oneOf(adapter2).offerOutgoingAssignment(entry1);
		}});
		connectionList.offerOutgoingAssignment(entry1);
		context.assertIsSatisfied();
		
		context.checking(new Expectations() {{
			oneOf(adapter1).shutdown(true);
		}});
		connectionList.close(adapter1, true);
		context.assertIsSatisfied();
		
		context.checking(new Expectations() {{
			oneOf(adapter2).offerOutgoingAssignment(entry2);
		}});
		connectionList.offerOutgoingAssignment(entry2);
	}
}
