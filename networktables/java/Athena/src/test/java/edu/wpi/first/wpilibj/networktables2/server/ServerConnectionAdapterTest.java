package edu.wpi.first.wpilibj.networktables2.server;

import static org.junit.Assert.*;

import java.io.*;

import org.jmock.*;
import org.jmock.integration.junit4.*;
import org.jmock.lib.legacy.*;
import org.junit.*;
import org.junit.runner.*;

import test.util.*;
import edu.wpi.first.wpilibj.networktables2.*;
import edu.wpi.first.wpilibj.networktables2.connection.BadMessageException;
import edu.wpi.first.wpilibj.networktables2.stream.*;
import edu.wpi.first.wpilibj.networktables2.thread.*;
import edu.wpi.first.wpilibj.networktables2.type.*;

@RunWith(JMock.class)
public class ServerConnectionAdapterTest {
	Mockery context = new JUnit4Mockery() {{
        setImposteriser(ClassImposteriser.INSTANCE);
    }};

	@Test public void testServerConnectionState() throws IOException {
		assertEquals("GOT_CONNECTION_FROM_CLIENT", ServerConnectionState.GOT_CONNECTION_FROM_CLIENT.toString());
		assertEquals("CONNECTED_TO_CLIENT", ServerConnectionState.CONNECTED_TO_CLIENT.toString());
		assertEquals("CLIENT_DISCONNECTED", ServerConnectionState.CLIENT_DISCONNECTED.toString());
		assertEquals("SERVER_ERROR: class java.io.IOException: Something happended", new ServerConnectionState.Error(new IOException("Something happended")).toString());
	}
	@Test public void testKeepAlive() throws IOException {
		final ServerNetworkTableEntryStore entryStore = context.mock(ServerNetworkTableEntryStore.class);
		final IncomingEntryReceiver transactionReceiver = context.mock(IncomingEntryReceiver.class);
		final ServerAdapterManager connectionListener = context.mock(ServerAdapterManager.class);
		final NTThreadManager threadManager = context.mock(NTThreadManager.class);
		final IOStream stream = context.mock(IOStream.class);
		
		context.checking(new Expectations() {{
			ignoring(threadManager).newBlockingPeriodicThread(with(any(PeriodicRunnable.class)), with("Server Connection Reader Thread"));
			allowing(stream).getInputStream();
			allowing(stream).getOutputStream();
		}});
		
		ServerConnectionAdapter serverAdapter = new ServerConnectionAdapter(stream, entryStore, transactionReceiver, connectionListener, new NetworkTableEntryTypeManager(), threadManager);
		
		serverAdapter.keepAlive();
	}

	@Test public void testClientHello() throws IOException {
		final ServerNetworkTableEntryStore entryStore = context.mock(ServerNetworkTableEntryStore.class);
		final IncomingEntryReceiver transactionReceiver = context.mock(IncomingEntryReceiver.class);
		final ServerAdapterManager connectionListener = context.mock(ServerAdapterManager.class);
		final NTThreadManager threadManager = context.mock(NTThreadManager.class);
		final IOStream stream = context.mock(IOStream.class);
		
		context.checking(new Expectations() {{
			ignoring(threadManager).newBlockingPeriodicThread(with(any(PeriodicRunnable.class)), with("Server Connection Reader Thread"));
			allowing(stream).getInputStream();
			allowing(stream).getOutputStream();
		}});
		
		final ServerConnectionAdapter serverAdapter = new ServerConnectionAdapter(stream, entryStore, transactionReceiver, connectionListener, new NetworkTableEntryTypeManager(), threadManager);
		context.assertIsSatisfied();
		
		context.checking(new Expectations() {{
			ignoring(threadManager).newBlockingPeriodicThread(with(any(PeriodicRunnable.class)), with("Server Connection Reader Thread"));
			oneOf(entryStore).sendServerHello(serverAdapter.connection);
		}});
		
		assertEquals(ServerConnectionState.GOT_CONNECTION_FROM_CLIENT, serverAdapter.getConnectionState());
		serverAdapter.clientHello((char)0x0200);
		assertEquals(ServerConnectionState.CONNECTED_TO_CLIENT, serverAdapter.getConnectionState());
	}
	
	@Test public void testUnsupportedProtocol() throws IOException {
		final ServerNetworkTableEntryStore entryStore = context.mock(ServerNetworkTableEntryStore.class);
		final IncomingEntryReceiver transactionReceiver = context.mock(IncomingEntryReceiver.class);
		final ServerAdapterManager connectionListener = context.mock(ServerAdapterManager.class);
		final NTThreadManager threadManager = context.mock(NTThreadManager.class);
		final IOStream stream = context.mock(IOStream.class);

		context.checking(new Expectations() {{
			ignoring(threadManager).newBlockingPeriodicThread(with(any(PeriodicRunnable.class)), with("Server Connection Reader Thread"));
			allowing(stream).getInputStream();
			allowing(stream).getOutputStream();
		}});
		
		final ServerConnectionAdapter serverAdapter = new ServerConnectionAdapter(stream, entryStore, transactionReceiver, connectionListener, new NetworkTableEntryTypeManager(), threadManager);
		
		try{
			serverAdapter.protocolVersionUnsupported((char)0);
			fail();
		} catch(BadMessageException e){
		}
	}
	
	@Test public void testUnsupportedClientProtocol() throws IOException {
		final ServerNetworkTableEntryStore entryStore = context.mock(ServerNetworkTableEntryStore.class);
		final IncomingEntryReceiver transactionReceiver = context.mock(IncomingEntryReceiver.class);
		final ServerAdapterManager connectionListener = context.mock(ServerAdapterManager.class);
		final NTThreadManager threadManager = context.mock(NTThreadManager.class);
		final ByteArrayStream stream = new ByteArrayStream();

		context.checking(new Expectations() {{
			ignoring(threadManager).newBlockingPeriodicThread(with(any(PeriodicRunnable.class)), with("Server Connection Reader Thread"));
		}});
		
		final ServerConnectionAdapter serverAdapter = new ServerConnectionAdapter(stream, entryStore, transactionReceiver, connectionListener, new NetworkTableEntryTypeManager(), threadManager);
		
		try{
			serverAdapter.clientHello((char)0);
			fail();
		} catch(BadMessageException e){
		}
		assertArrayEquals(new byte[]{0x02, 0x02, (byte)0x00}, stream.getOutput());
	}
	
	@Test public void testConnectionErrorBadMessage() throws IOException {
		final ServerNetworkTableEntryStore entryStore = context.mock(ServerNetworkTableEntryStore.class);
		final IncomingEntryReceiver transactionReceiver = context.mock(IncomingEntryReceiver.class);
		final ServerAdapterManager connectionListener = context.mock(ServerAdapterManager.class);
		final IOStream stream = context.mock(IOStream.class);
		final NTThreadManager threadManager = context.mock(NTThreadManager.class);
		
		context.checking(new Expectations() {{
			ignoring(threadManager).newBlockingPeriodicThread(with(any(PeriodicRunnable.class)), with("Server Connection Reader Thread"));
			allowing(stream).getInputStream();
			allowing(stream).getOutputStream();
		}});
		
		final ServerConnectionAdapter serverAdapter = new ServerConnectionAdapter(stream, entryStore, transactionReceiver, connectionListener, new NetworkTableEntryTypeManager(), threadManager);
		context.assertIsSatisfied();
		
		context.checking(new Expectations() {{
			ignoring(threadManager).newBlockingPeriodicThread(with(any(PeriodicRunnable.class)), with("Server Connection Reader Thread"));
			oneOf(connectionListener).close(serverAdapter, true);
		}});

		serverAdapter.badMessage(new BadMessageException(""));
	}
	
	@Test public void testConnectionErrorIOException() throws IOException {
		final ServerNetworkTableEntryStore entryStore = context.mock(ServerNetworkTableEntryStore.class);
		final IncomingEntryReceiver transactionReceiver = context.mock(IncomingEntryReceiver.class);
		final ServerAdapterManager connectionListener = context.mock(ServerAdapterManager.class);
		final NTThreadManager threadManager = context.mock(NTThreadManager.class);
		final IOStream stream = context.mock(IOStream.class);
		
		
		context.checking(new Expectations() {{
			ignoring(threadManager).newBlockingPeriodicThread(with(any(PeriodicRunnable.class)), with("Server Connection Reader Thread"));
			allowing(stream).getInputStream();
			allowing(stream).getOutputStream();
		}});
		
		final ServerConnectionAdapter serverAdapter = new ServerConnectionAdapter(stream, entryStore, transactionReceiver, connectionListener, new NetworkTableEntryTypeManager(), threadManager);
		context.assertIsSatisfied();

		context.checking(new Expectations() {{
			oneOf(connectionListener).close(serverAdapter, false);
		}});

		serverAdapter.ioException(new IOException(""));
	}
	@Test public void testClientDisconnect() throws IOException {
		final ServerNetworkTableEntryStore entryStore = context.mock(ServerNetworkTableEntryStore.class);
		final IncomingEntryReceiver transactionReceiver = context.mock(IncomingEntryReceiver.class);
		final ServerAdapterManager connectionListener = context.mock(ServerAdapterManager.class);
		final NTThreadManager threadManager = context.mock(NTThreadManager.class);
		final IOStream stream = context.mock(IOStream.class);
		
		
		context.checking(new Expectations() {{
			ignoring(threadManager).newBlockingPeriodicThread(with(any(PeriodicRunnable.class)), with("Server Connection Reader Thread"));
			allowing(stream).getInputStream();
			allowing(stream).getOutputStream();
		}});
		
		final ServerConnectionAdapter serverAdapter = new ServerConnectionAdapter(stream, entryStore, transactionReceiver, connectionListener, new NetworkTableEntryTypeManager(), threadManager);
		context.assertIsSatisfied();

		context.checking(new Expectations() {{
			oneOf(connectionListener).close(serverAdapter, false);
		}});

		serverAdapter.ioException(new EOFException());
		assertEquals(ServerConnectionState.CLIENT_DISCONNECTED, serverAdapter.getConnectionState());
	}
	
	@Test public void testGetEntryType() throws IOException {
		final ServerNetworkTableEntryStore entryStore = context.mock(ServerNetworkTableEntryStore.class);
		final IncomingEntryReceiver transactionReceiver = context.mock(IncomingEntryReceiver.class);
		final ServerAdapterManager connectionListener = context.mock(ServerAdapterManager.class);
		final NTThreadManager threadManager = context.mock(NTThreadManager.class);
		final IOStream stream = context.mock(IOStream.class);
		context.checking(new Expectations() {{
			ignoring(threadManager).newBlockingPeriodicThread(with(any(PeriodicRunnable.class)), with("Server Connection Reader Thread"));
			allowing(stream).getInputStream();
			allowing(stream).getOutputStream();
		}});
		
		
		
		final NetworkTableEntry entry1 = NetworkTableEntryUtil.newBooleanEntry("MyEntry", true);
		final NetworkTableEntry entry2 = NetworkTableEntryUtil.newDoubleEntry("MyEntry", 12);
		final NetworkTableEntry entry3 = NetworkTableEntryUtil.newStringEntry("MyEntry", "Value");

		context.checking(new Expectations() {{
			allowing(entryStore).getEntry((char)16);will(returnValue(entry1));
			allowing(entryStore).getEntry((char)1);will(returnValue(entry2));
			allowing(entryStore).getEntry((char)22);will(returnValue(entry3));
		}});
		
		final ServerConnectionAdapter serverAdapter = new ServerConnectionAdapter(stream, entryStore, transactionReceiver, connectionListener, new NetworkTableEntryTypeManager(), threadManager);

		assertEquals(DefaultEntryTypes.BOOLEAN, serverAdapter.getEntry((char)16).getType());
		assertEquals(DefaultEntryTypes.DOUBLE, serverAdapter.getEntry((char)1).getType());
		assertEquals(DefaultEntryTypes.STRING, serverAdapter.getEntry((char)22).getType());
	}


}
