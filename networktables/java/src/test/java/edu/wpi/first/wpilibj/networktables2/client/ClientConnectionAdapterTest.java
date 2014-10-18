package edu.wpi.first.wpilibj.networktables2.client;

import static org.junit.Assert.*;
import static org.hamcrest.core.IsInstanceOf.*;

import java.io.*;

import org.hamcrest.*;
import org.hamcrest.Description;
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
public class ClientConnectionAdapterTest {
	Mockery context = new JUnit4Mockery() {{
        setImposteriser(ClassImposteriser.INSTANCE);
    }};
	
	@Test public void testKeepAlive() {
		final ClientConnectionListenerManager connectionListenerManager = context.mock(ClientConnectionListenerManager.class);
		final ClientNetworkTableEntryStore entryStore = context.mock(ClientNetworkTableEntryStore.class);
		final NTThreadManager threadManager = context.mock(NTThreadManager.class);
		final IOStreamFactory streamFactory = context.mock(IOStreamFactory.class);
		
		ClientConnectionAdapter clientAdapter = new ClientConnectionAdapter(entryStore, threadManager, streamFactory, connectionListenerManager, new NetworkTableEntryTypeManager());
		try{
                    clientAdapter.keepAlive();
		} catch(BadMessageException e){
                    fail();
		} catch(IOException e){
                    fail();
		}
	}

	@Test public void testClientHello() {
		final ClientConnectionListenerManager connectionListenerManager = context.mock(ClientConnectionListenerManager.class);
		final ClientNetworkTableEntryStore entryStore = context.mock(ClientNetworkTableEntryStore.class);
		final NTThreadManager threadManager = context.mock(NTThreadManager.class);
		final IOStreamFactory streamFactory = context.mock(IOStreamFactory.class);
		
		ClientConnectionAdapter clientAdapter = new ClientConnectionAdapter(entryStore, threadManager, streamFactory, connectionListenerManager, new NetworkTableEntryTypeManager());
		try{
			clientAdapter.clientHello((char)0);
			fail();
		} catch(BadMessageException e){
		} catch(IOException e){
			fail();
		}
	}
	
	@Test public void testUnsupportedProtocol() {
		final ClientConnectionListenerManager connectionListenerManager = context.mock(ClientConnectionListenerManager.class);
		final ClientNetworkTableEntryStore entryStore = context.mock(ClientNetworkTableEntryStore.class);
		final NTThreadManager threadManager = context.mock(NTThreadManager.class);
		final IOStreamFactory streamFactory = context.mock(IOStreamFactory.class);
		final IOStream stream = context.mock(IOStream.class);
		final NTThread thread = context.mock(NTThread.class);

		
		ClientConnectionAdapter clientAdapter = new ClientConnectionAdapter(entryStore, threadManager, streamFactory, connectionListenerManager, new NetworkTableEntryTypeManager());
		try {
			context.checking(new Expectations() {{
				allowing(stream).getInputStream();
				allowing(stream).getOutputStream();
				oneOf(streamFactory).createStream();will(returnValue(stream));
				oneOf(threadManager).newBlockingPeriodicThread(with(any(PeriodicRunnable.class)), with("Client Connection Reader Thread"));will(returnValue(thread));
				oneOf(entryStore).clearIds();
			}});
		} catch (IOException e1) {}
		clientAdapter.reconnect();
		context.checking(new Expectations() {{
			oneOf(stream).close();
			oneOf(thread).stop();
			oneOf(entryStore).clearIds();
		}});
		clientAdapter.protocolVersionUnsupported((char)10);
                assertThat(clientAdapter.getConnectionState(), instanceOf(ClientConnectionState.ProtocolUnsuppotedByServer.class));
                assertEquals((char)10, ((ClientConnectionState.ProtocolUnsuppotedByServer)clientAdapter.getConnectionState()).getServerVersion());
	}
	
	@Test public void testConnectionErrorBadMessage() {
		final ClientConnectionListenerManager connectionListenerManager = context.mock(ClientConnectionListenerManager.class);
		final ClientNetworkTableEntryStore entryStore = context.mock(ClientNetworkTableEntryStore.class);
		final NTThreadManager threadManager = context.mock(NTThreadManager.class);
		final IOStreamFactory streamFactory = context.mock(IOStreamFactory.class);
		final IOStream stream = context.mock(IOStream.class);
		final NTThread thread = context.mock(NTThread.class);

		
		ClientConnectionAdapter clientAdapter = new ClientConnectionAdapter(entryStore, threadManager, streamFactory, connectionListenerManager, new NetworkTableEntryTypeManager());
		try {
			context.checking(new Expectations() {{
				allowing(stream).getInputStream();
				allowing(stream).getOutputStream();
				oneOf(streamFactory).createStream();will(returnValue(stream));
				oneOf(threadManager).newBlockingPeriodicThread(with(any(PeriodicRunnable.class)), with("Client Connection Reader Thread"));will(returnValue(thread));
				oneOf(entryStore).clearIds();
			}});
		} catch (IOException e1) {}
		clientAdapter.reconnect();
		context.checking(new Expectations() {{
			oneOf(stream).close();
			oneOf(thread).stop();
			oneOf(entryStore).clearIds();
		}});
		BadMessageException e = new BadMessageException("");
		clientAdapter.badMessage(e);
		assertThat(clientAdapter.getConnectionState(), isClientConnectionErrorState(e));
	}

	@Test public void testConnectionErrorIOException() {
		final ClientConnectionListenerManager connectionListenerManager = context.mock(ClientConnectionListenerManager.class);
		final ClientNetworkTableEntryStore entryStore = context.mock(ClientNetworkTableEntryStore.class);
		final NTThreadManager threadManager = context.mock(NTThreadManager.class);
		final IOStreamFactory streamFactory = context.mock(IOStreamFactory.class);
		final IOStream stream1 = context.mock(IOStream.class, "stream1");
		final IOStream stream2 = context.mock(IOStream.class, "stream2");
		final NTThread thread1 = context.mock(NTThread.class, "thread1");
		final NTThread thread2 = context.mock(NTThread.class, "thread2");

		final ClientConnectionAdapter clientAdapter = new ClientConnectionAdapter(entryStore, threadManager, streamFactory, connectionListenerManager, new NetworkTableEntryTypeManager());
		try {
			context.checking(new Expectations() {{
				allowing(stream1).getInputStream();
				allowing(stream1).getOutputStream();
				oneOf(streamFactory).createStream();will(returnValue(stream1));
				oneOf(threadManager).newBlockingPeriodicThread(with(any(PeriodicRunnable.class)), with("Client Connection Reader Thread"));will(returnValue(thread1));
				oneOf(entryStore).clearIds();
			}});
		} catch (IOException e1) {}
		clientAdapter.reconnect();
		try {
			context.checking(new Expectations() {{
				oneOf(stream1).close();
				oneOf(thread1).stop();
				oneOf(entryStore).clearIds();
				allowing(stream2).getInputStream();
				allowing(stream2).getOutputStream();
				oneOf(streamFactory).createStream();will(returnValue(stream2));
				oneOf(threadManager).newBlockingPeriodicThread(with(any(PeriodicRunnable.class)), with("Client Connection Reader Thread"));will(returnValue(thread2));
			}});
		} catch (IOException e1) {}
		IOException e = new IOException("");
		clientAdapter.ioException(e);
		assertEquals(clientAdapter.getConnectionState(), ClientConnectionState.CONNECTED_TO_SERVER);
	}

	@Test public void testGetEntryType() {
		final ClientConnectionListenerManager connectionListenerManager = context.mock(ClientConnectionListenerManager.class);
		final ClientNetworkTableEntryStore entryStore = context.mock(ClientNetworkTableEntryStore.class);
		final IOStreamFactory streamFactory = context.mock(IOStreamFactory.class);
		final NTThreadManager threadManager = context.mock(NTThreadManager.class);
		
		final NetworkTableEntry entry1 = NetworkTableEntryUtil.newBooleanEntry("MyEntry", true);
		final NetworkTableEntry entry2 = NetworkTableEntryUtil.newDoubleEntry("MyEntry", 12);
		final NetworkTableEntry entry3 = NetworkTableEntryUtil.newStringEntry("MyEntry", "Value");

		context.checking(new Expectations() {{
			allowing(entryStore).getEntry((char)16);will(returnValue(entry1));
			allowing(entryStore).getEntry((char)1);will(returnValue(entry2));
			allowing(entryStore).getEntry((char)22);will(returnValue(entry3));
		}});
		
		ClientConnectionAdapter clientAdapter = new ClientConnectionAdapter(entryStore, threadManager, streamFactory, connectionListenerManager, new NetworkTableEntryTypeManager());

		assertEquals(DefaultEntryTypes.BOOLEAN, clientAdapter.getEntry((char)16).getType());
		assertEquals(DefaultEntryTypes.DOUBLE, clientAdapter.getEntry((char)1).getType());
		assertEquals(DefaultEntryTypes.STRING, clientAdapter.getEntry((char)22).getType());
	}

	
	private Matcher<ClientConnectionState> isClientConnectionErrorState(final Exception e) {
		return new TypeSafeMatcher<ClientConnectionState>() {

			@Override
			public void describeTo(Description description) {
				description.appendText(" a connection error state with the exception ").appendValue(e);
			}

			@Override
			public boolean matchesSafely(ClientConnectionState item) {
				if(item instanceof ClientConnectionState.Error)
					return ((ClientConnectionState.Error) item).getException()==e;
				return false;
			}
		};
	}
}
