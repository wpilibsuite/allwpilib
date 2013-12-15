package edu.wpi.first.wpilibj.networktables2.server;

import static org.junit.Assert.*;

import java.io.*;

import org.hamcrest.*;
import org.hamcrest.Description;
import org.jmock.*;
import org.jmock.integration.junit4.*;
import org.jmock.lib.legacy.*;
import org.junit.*;
import org.junit.runner.*;

import test.util.*;
import edu.wpi.first.wpilibj.networktables2.stream.*;
import edu.wpi.first.wpilibj.networktables2.thread.*;
import edu.wpi.first.wpilibj.networktables2.type.*;

@RunWith(JMock.class)
public class ServerIncomingMonitorTest {
	Mockery context = new JUnit4Mockery() {{
        setImposteriser(ClassImposteriser.INSTANCE);
    }};;

	@Test public void testReceiveConnections() throws IOException {
		final ServerNetworkTableEntryStore entryStore = context.mock(ServerNetworkTableEntryStore.class);
		final ServerAdapterManager adapterListener = context.mock(ServerAdapterManager.class);
		
		final IOStreamProvider streamProvider = context.mock(IOStreamProvider.class);
		final TestThreadManager threadManager = new TestThreadManager();
		final IOStream stream1 = new ByteArrayStream();
		final IOStream stream2 = new ByteArrayStream();
		final ServerIncomingConnectionListener incomingListener = context.mock(ServerIncomingConnectionListener.class);

		final States progress = context.states("progress").startsAs("sendFirst");
		context.checking(new Expectations() {{

			oneOf(streamProvider).accept();will(returnValue(stream1));when(progress.is("sendFirst"));then(progress.is("sentFirst"));
			oneOf(incomingListener).onNewConnection(with(aConnectionAdapterFor(stream1)));
					when(progress.is("sentFirst"));then(progress.is("receivedFirst"));
			oneOf(streamProvider).accept();will(returnValue(stream2));when(progress.is("sendSecond"));then(progress.is("sentSecond"));
			oneOf(incomingListener).onNewConnection(with(aConnectionAdapterFor(stream2)));
					when(progress.is("sentSecond"));then(progress.is("receivedSecond"));
		}});
		
		ServerIncomingStreamMonitor incomingMonitor = new ServerIncomingStreamMonitor(streamProvider, entryStore,
				incomingListener, adapterListener, new NetworkTableEntryTypeManager(), threadManager);
		incomingMonitor.start();

		threadManager.getPeriodicThread(ServerIncomingStreamMonitor.class).run();
		assertThat(progress, hasState("receivedFirst"));
		progress.become("sendSecond");
		
		threadManager.getPeriodicThread(ServerIncomingStreamMonitor.class).run();
		assertThat(progress, hasState("receivedSecond"));
	}

	@Test public void testClose() throws IOException {
		final ServerNetworkTableEntryStore entryStore = context.mock(ServerNetworkTableEntryStore.class);
		final ServerAdapterManager adapterListener = context.mock(ServerAdapterManager.class);
		
		final IOStreamProvider streamProvider = context.mock(IOStreamProvider.class);
		final NTThreadManager threadManager = context.mock(NTThreadManager.class);
		final NTThread thread = context.mock(NTThread.class);
		final ServerIncomingConnectionListener incomingListener = context.mock(ServerIncomingConnectionListener.class);

		final States progress = context.states("progress").startsAs("creating monitor");
		context.checking(new Expectations() {{
			oneOf(threadManager).newBlockingPeriodicThread(with(any(PeriodicRunnable.class)), with("Server Incoming Stream Monitor Thread"));will(returnValue(thread));
					when(progress.is("starting monitor"));then(progress.is("started monitor"));
			oneOf(thread).stop();when(progress.is("stopping monitor"));then(progress.is("stopped monitor"));
		}});
		
		ServerIncomingStreamMonitor incomingMonitor = new ServerIncomingStreamMonitor(streamProvider, entryStore,
				incomingListener, adapterListener, new NetworkTableEntryTypeManager(), threadManager);

		progress.become("starting monitor");
		incomingMonitor.start();

		assertThat(progress, hasState("started monitor"));

		progress.become("stopping monitor");
		
		incomingMonitor.stop();

		assertThat(progress, hasState("stopped monitor"));
	}

	@Test public void testStopWithoutStart() throws IOException {
		final ServerNetworkTableEntryStore entryStore = context.mock(ServerNetworkTableEntryStore.class);
		final ServerAdapterManager adapterListener = context.mock(ServerAdapterManager.class);
		
		final IOStreamProvider streamProvider = context.mock(IOStreamProvider.class);
		final NTThreadManager threadManager = context.mock(NTThreadManager.class);
		final ServerIncomingConnectionListener incomingListener = context.mock(ServerIncomingConnectionListener.class);
		
		ServerIncomingStreamMonitor incomingMonitor = new ServerIncomingStreamMonitor(streamProvider, entryStore,
				incomingListener, adapterListener, new NetworkTableEntryTypeManager(), threadManager);
		
		incomingMonitor.stop();
	}

	@Test public void testNullStream() throws IOException {
		final ServerNetworkTableEntryStore entryStore = context.mock(ServerNetworkTableEntryStore.class);
		final ServerAdapterManager adapterListener = context.mock(ServerAdapterManager.class);
		
		final IOStreamProvider streamProvider = context.mock(IOStreamProvider.class);
		final TestThreadManager threadManager = new TestThreadManager();
		final ServerIncomingConnectionListener incomingListener = context.mock(ServerIncomingConnectionListener.class);

		context.checking(new Expectations() {{
			oneOf(streamProvider).accept();will(returnValue(null));
		}});
		
		ServerIncomingStreamMonitor incomingMonitor = new ServerIncomingStreamMonitor(streamProvider, entryStore,
				incomingListener, adapterListener, new NetworkTableEntryTypeManager(), threadManager);
		incomingMonitor.start();

		threadManager.getPeriodicThread(ServerIncomingStreamMonitor.class).run();
	}

	@Test public void testStreamAcceptException() throws IOException {
		final ServerNetworkTableEntryStore entryStore = context.mock(ServerNetworkTableEntryStore.class);
		final ServerAdapterManager adapterListener = context.mock(ServerAdapterManager.class);
		
		final IOStreamProvider streamProvider = context.mock(IOStreamProvider.class);
		final TestThreadManager threadManager = new TestThreadManager();
		final ServerIncomingConnectionListener incomingListener = context.mock(ServerIncomingConnectionListener.class);

		context.checking(new Expectations() {{
			oneOf(streamProvider).accept();will(throwException(new IOException("Something bad happened")));
		}});
		
		ServerIncomingStreamMonitor incomingMonitor = new ServerIncomingStreamMonitor(streamProvider, entryStore,
				incomingListener, adapterListener, new NetworkTableEntryTypeManager(), threadManager);
		incomingMonitor.start();

		threadManager.getPeriodicThread(ServerIncomingStreamMonitor.class).run();
	}


	
	
	
	
	
	
	
	
	private Matcher<ServerConnectionAdapter> aConnectionAdapterFor(final IOStream stream) {
		return new TypeSafeMatcher<ServerConnectionAdapter>(){

			@Override
			public void describeTo(Description description) {
				description.appendText(" a server connection adapter for ").appendValue(stream);
			}

			@Override
			public boolean matchesSafely(ServerConnectionAdapter adapter) {
				return adapter.connection.stream==stream;
			}

		};
	}
	private Matcher<States> hasState(final String state) {
		return new TypeSafeMatcher<States>(){

			@Override
			public void describeTo(Description description) {
				description.appendText(" has the state: ").appendValue(state);
			}

			@Override
			public boolean matchesSafely(States stateMachine) {
				return stateMachine.is(state).isActive();
			}

		};
	}

}
