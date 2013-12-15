package edu.wpi.first.wpilibj.networktables2.connection;

import static org.junit.Assert.*;
import static test.util.NetworkTableEntryUtil.*;

import java.io.*;

import org.jmock.*;
import org.jmock.integration.junit4.*;
import org.jmock.lib.legacy.*;
import org.junit.*;
import org.junit.runner.*;

import test.util.*;
import edu.wpi.first.wpilibj.networktables2.*;
import edu.wpi.first.wpilibj.networktables2.type.*;

@RunWith(JMock.class)
public class NetworkTableConnectionEntryUpdateTest {
	Mockery context = new JUnit4Mockery() {{
        setImposteriser(ClassImposteriser.INSTANCE);
    }};


	@Test public void testBooleanEntryUpdateSend() throws IOException {
		ByteArrayStream stream = new ByteArrayStream();
		NetworkTableConnection connection = new NetworkTableConnection(stream, new NetworkTableEntryTypeManager());

		connection.sendEntryUpdate(newBooleanEntry((char)16, "My Entry", (char)0, true));
		connection.flush();

		assertArrayEquals(new byte[]{17, 0, 16, 0, 0, 1}, stream.getOutput());
	}
	@Test public void testBooleanEntryUpdateReceive() throws IOException {
		ByteArrayStream stream = new ByteArrayStream(new byte[]{17, 0, 16, 0, 0, 1});
		final ConnectionAdapter receiver = context.mock(ConnectionAdapter.class);
		NetworkTableConnection connection = new NetworkTableConnection(stream, new NetworkTableEntryTypeManager());
		final NetworkTableEntry entry = newBooleanEntry((char)16, "MyEntry", (char)10, false);

		context.checking(new Expectations() {{
			oneOf(receiver).offerIncomingUpdate(entry, (char)0, true);
			allowing(receiver).getEntry((char)16);will(returnValue(entry));
		}});

		connection.read(receiver);
	}



	@Test public void testDoubleEntryUpdateSend() throws IOException {
		ByteArrayStream stream = new ByteArrayStream();
		NetworkTableConnection connection = new NetworkTableConnection(stream, new NetworkTableEntryTypeManager());

		connection.sendEntryUpdate(newDoubleEntry((char)16, "My Entry", (char)0, 12.5));
		connection.flush();

		assertArrayEquals(new byte[]{17, 0, 16, 0, 0, 64, 41, 0, 0, 0, 0, 0, 0}, stream.getOutput());
	}
	@Test public void testDoubleEntryUpdateReceive() throws IOException {
		ByteArrayStream stream = new ByteArrayStream(new byte[]{17, 0, 16, 0, 0, 64, 41, 0, 0, 0, 0, 0, 0});
		final ConnectionAdapter receiver = context.mock(ConnectionAdapter.class);
		NetworkTableConnection connection = new NetworkTableConnection(stream, new NetworkTableEntryTypeManager());
		final NetworkTableEntry entry = newDoubleEntry((char)16, "MyEntry", (char)10, 3);

		context.checking(new Expectations() {{
			allowing(receiver).getEntry((char)16);will(returnValue(entry));
			oneOf(receiver).offerIncomingUpdate(entry, (char)0, 12.5);
		}});

		connection.read(receiver);
	}




	@Test public void testStringEntryUpdateSend() throws IOException {
		ByteArrayStream stream = new ByteArrayStream();
		NetworkTableConnection connection = new NetworkTableConnection(stream, new NetworkTableEntryTypeManager());

		connection.sendEntryUpdate(NetworkTableEntryUtil.newStringEntry((char)16, "My Entry", (char)0, "VaLuE"));
		connection.flush();

		assertArrayEquals(new byte[]{17, 0, 16, 0, 0, 0, 5, 86, 97, 76, 117, 69}, stream.getOutput());
	}
	@Test public void testStringEntryUpdateReceive() throws IOException {
		ByteArrayStream stream = new ByteArrayStream(new byte[]{17, 0, 16, 0, 0, 0, 5, 86, 97, 76, 117, 69});
		final ConnectionAdapter receiver = context.mock(ConnectionAdapter.class);
		NetworkTableConnection connection = new NetworkTableConnection(stream, new NetworkTableEntryTypeManager());
		final NetworkTableEntry entry = newStringEntry((char)16, "MyEntry", (char)10, "HI");

		context.checking(new Expectations() {{
			allowing(receiver).getEntry((char)16);will(returnValue(entry));
			oneOf(receiver).offerIncomingUpdate(entry, (char)0, "VaLuE");
		}});

		connection.read(receiver);
	}


}
