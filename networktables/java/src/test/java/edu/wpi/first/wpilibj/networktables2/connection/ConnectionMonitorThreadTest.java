package edu.wpi.first.wpilibj.networktables2.connection;

import java.io.*;

import org.jmock.*;
import org.jmock.auto.*;
import org.jmock.integration.junit4.*;
import org.jmock.lib.legacy.*;
import org.junit.*;
import org.junit.runner.*;

import edu.wpi.first.wpilibj.networktables2.*;

@RunWith(JMock.class)
public class ConnectionMonitorThreadTest {
	Mockery context = new JUnit4Mockery() {{
        setImposteriser(ClassImposteriser.INSTANCE);
    }};
    
	@Mock ConnectionAdapter adapter;
	@Mock NetworkTableConnection connection;
	ConnectionMonitorThread thread;
	@Before public void before(){
		thread = new ConnectionMonitorThread(adapter, connection);
	}
	
	@Test public void testSimpleRead() throws Exception {
		context.checking(new Expectations() {{
			oneOf(connection).read(adapter);
		}});
		thread.run();
	}
	@Test public void testReadIOException() throws Exception {
		final IOException e = new IOException();
		context.checking(new Expectations() {{
			oneOf(connection).read(adapter);will(throwException(e));
			oneOf(adapter).ioException(e);
		}});
		thread.run();
	}
	@Test public void testReadBadMessageException() throws Exception {
		final BadMessageException e = new BadMessageException("");
		context.checking(new Expectations() {{
			oneOf(connection).read(adapter);will(throwException(e));
			oneOf(adapter).badMessage(e);
		}});
		thread.run();
	}

}
