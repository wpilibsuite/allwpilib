package edu.wpi.first.wpilibj.networktables2.connection;

import static org.junit.Assert.*;

import java.io.*;

import org.jmock.*;
import org.jmock.integration.junit4.*;
import org.jmock.lib.legacy.*;
import org.junit.*;
import org.junit.runner.*;

import test.util.*;
import edu.wpi.first.wpilibj.networktables2.*;
import edu.wpi.first.wpilibj.networktables2.stream.*;
import edu.wpi.first.wpilibj.networktables2.type.*;

@RunWith(JMock.class)
public class NetworkTableConnectionTest {
	Mockery context = new JUnit4Mockery() {{
        setImposteriser(ClassImposteriser.INSTANCE);
    }};


	@Test public void testProperties() {
		final IOStream stream = context.mock(IOStream.class);
		context.checking(new Expectations() {{
			ignoring(stream);
		}});
		@SuppressWarnings("unused")
		NetworkTableConnection connection = new NetworkTableConnection(stream, new NetworkTableEntryTypeManager());
	}

	@Test public void testClose() {
		final IOStream stream = context.mock(IOStream.class);
		context.checking(new Expectations() {{
			allowing(stream).getInputStream();
			allowing(stream).getOutputStream();
			oneOf(stream).close();
		}});

		NetworkTableConnection connection = new NetworkTableConnection(stream, new NetworkTableEntryTypeManager());

		connection.close();
	}

	@Test public void testCloseTwice() {
		final IOStream stream = context.mock(IOStream.class);
		context.checking(new Expectations() {{
			allowing(stream).getInputStream();
			allowing(stream).getOutputStream();
			oneOf(stream).close();
		}});

		NetworkTableConnection connection = new NetworkTableConnection(stream, new NetworkTableEntryTypeManager());

		connection.close();
		connection.close();
	}





	@Test public void testKeepAliveSend() throws IOException {
		ByteArrayStream stream = new ByteArrayStream();
		NetworkTableConnection connection = new NetworkTableConnection(stream, new NetworkTableEntryTypeManager());

		connection.sendKeepAlive();

		assertArrayEquals(new byte[]{0}, stream.getOutput());
	}
	@Test public void testKeepAliveReceive() throws IOException {
		ByteArrayStream stream = new ByteArrayStream(new byte[]{0});
		final ConnectionAdapter receiver = context.mock(ConnectionAdapter.class);
		NetworkTableConnection connection = new NetworkTableConnection(stream, new NetworkTableEntryTypeManager());

		context.checking(new Expectations() {{
			oneOf(receiver).keepAlive();
		}});

		connection.read(receiver);
	}




	@Test public void testClientHelloSend() throws IOException {
		ByteArrayStream stream = new ByteArrayStream();
		NetworkTableConnection connection = new NetworkTableConnection(stream, new NetworkTableEntryTypeManager());

		connection.sendClientHello();

		assertArrayEquals(new byte[]{1, 2, 00}, stream.getOutput());
	}
	@Test public void testClientHelloReceive() throws IOException {
		ByteArrayStream stream = new ByteArrayStream(new byte[]{1, 0, 4});
		final ConnectionAdapter receiver = context.mock(ConnectionAdapter.class);
		NetworkTableConnection connection = new NetworkTableConnection(stream, new NetworkTableEntryTypeManager());

		context.checking(new Expectations() {{
			oneOf(receiver).clientHello((char)4);
		}});

		connection.read(receiver);
	}





	@Test public void testProtocolVersionUnsupportedSend() throws IOException {
		ByteArrayStream stream = new ByteArrayStream();
		NetworkTableConnection connection = new NetworkTableConnection(stream, new NetworkTableEntryTypeManager());


		connection.sendProtocolVersionUnsupported();

		assertArrayEquals(new byte[]{2, 2, 00}, stream.getOutput());
	}
	@Test public void testProtocolVersionUnsupportedReceive() throws IOException {
		ByteArrayStream stream = new ByteArrayStream(new byte[]{2, 0, 4});
		final ConnectionAdapter receiver = context.mock(ConnectionAdapter.class);
		NetworkTableConnection connection = new NetworkTableConnection(stream, new NetworkTableEntryTypeManager());

		context.checking(new Expectations() {{
			oneOf(receiver).protocolVersionUnsupported((char)4);
		}});

		connection.read(receiver);
	}



	@Test public void testBadMessageTypeReceive() throws IOException {
		ByteArrayStream stream = new ByteArrayStream(new byte[]{127});
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
