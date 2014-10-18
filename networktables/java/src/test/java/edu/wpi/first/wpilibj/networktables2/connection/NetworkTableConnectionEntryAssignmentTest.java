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
public class NetworkTableConnectionEntryAssignmentTest {
	Mockery context = new JUnit4Mockery() {{
        setImposteriser(ClassImposteriser.INSTANCE);
    }};


	@Test public void testBooleanEntryAssignmentSend() throws IOException {
		ByteArrayStream stream = new ByteArrayStream();
		NetworkTableConnection connection = new NetworkTableConnection(stream, new NetworkTableEntryTypeManager());

		connection.sendEntryAssignment(NetworkTableEntryUtil.newBooleanEntry("My Entry", true));
		connection.flush();

		assertArrayEquals(new byte[]{16, 0, 8, 77, 121, 32, 69, 110, 116, 114, 121, 0, -1, -1, 0, 0, 1}, stream.getOutput());
	}
	@Test public void testBooleanEntryAssignmentReceive() throws IOException {
		ByteArrayStream stream = new ByteArrayStream(new byte[]{16, 0, 8, 77, 121, 32, 69, 110, 116, 114, 121, 0, -1, -1, 0, 0, 1});
		final ConnectionAdapter receiver = context.mock(ConnectionAdapter.class);
		NetworkTableConnection connection = new NetworkTableConnection(stream, new NetworkTableEntryTypeManager());

		context.checking(new Expectations() {{
			oneOf(receiver).offerIncomingAssignment(with(aBooleanEntry(0xFFFF, "My Entry", 0, true)));
		}});

		connection.read(receiver);
	}







	@Test public void testDoubleEntryAssignmentSend() throws IOException {
		ByteArrayStream stream = new ByteArrayStream();
		NetworkTableConnection connection = new NetworkTableConnection(stream, new NetworkTableEntryTypeManager());

		connection.sendEntryAssignment(newDoubleEntry("My Entry", 12.5));
		connection.flush();

		assertArrayEquals(new byte[]{16, 0, 8, 77, 121, 32, 69, 110, 116, 114, 121, 1, -1, -1, 0, 0, 64, 41, 0, 0, 0, 0, 0, 0}, stream.getOutput());
	}
	@Test public void testDoubleEntryAssignmentReceive() throws IOException {
		ByteArrayStream stream = new ByteArrayStream(new byte[]{16, 0, 8, 77, 121, 32, 69, 110, 116, 114, 121, 1, -1, -1, 0, 0, 64, 41, 0, 0, 0, 0, 0, 0});
		final ConnectionAdapter receiver = context.mock(ConnectionAdapter.class);
		NetworkTableConnection connection = new NetworkTableConnection(stream, new NetworkTableEntryTypeManager());

		context.checking(new Expectations() {{
			oneOf(receiver).offerIncomingAssignment(with(aDoubleEntry(0xFFFF, "My Entry", 0, 12.5)));
		}});

		connection.read(receiver);
	}




	@Test public void testStringEntryAssignmentSend() throws IOException {
		ByteArrayStream stream = new ByteArrayStream();
		NetworkTableConnection connection = new NetworkTableConnection(stream, new NetworkTableEntryTypeManager());

		connection.sendEntryAssignment(NetworkTableEntryUtil.newStringEntry("My Entry", "VaLuE"));
		connection.flush();

		assertArrayEquals(new byte[]{16, 0, 8, 77, 121, 32, 69, 110, 116, 114, 121, 2, -1, -1, 0, 0, 0, 5, 86, 97, 76, 117, 69}, stream.getOutput());
	}
	@Test public void testStringEntryAssignmentReceive() throws IOException {
		ByteArrayStream stream = new ByteArrayStream(new byte[]{16, 0, 8, 77, 121, 32, 69, 110, 116, 114, 121, 2, -1, -1, 0, 0, 0, 5, 86, 97, 76, 117, 69});
		final ConnectionAdapter receiver = context.mock(ConnectionAdapter.class);
		NetworkTableConnection connection = new NetworkTableConnection(stream, new NetworkTableEntryTypeManager());

		context.checking(new Expectations() {{
			oneOf(receiver).offerIncomingAssignment(with(aStringEntry(0xFFFF, "My Entry", 0, "VaLuE")));
		}});

		connection.read(receiver);
	}



	@Test public void testBadTypeEntryAssignmentReceive() throws IOException {
		ByteArrayStream stream = new ByteArrayStream(new byte[]{16, 0, 8, 77, 121, 32, 69, 110, 116, 114, 121, 127});
		final ConnectionAdapter receiver = context.mock(ConnectionAdapter.class);
		NetworkTableConnection connection = new NetworkTableConnection(stream, new NetworkTableEntryTypeManager());

		try{
			connection.read(receiver);
			fail();
		} catch(BadMessageException e){
		} catch(IOException e){
			fail();
		}
	}
}