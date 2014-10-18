package edu.wpi.first.wpilibj.networktables2.stream;

import static org.junit.Assert.*;

import java.io.*;

import org.jmock.*;
import org.jmock.integration.junit4.*;
import org.jmock.lib.legacy.*;
import org.junit.*;
import org.junit.runner.*;


@RunWith(JMock.class)
public class SimpleIOStreamTest {
	Mockery context = new JUnit4Mockery() {{
        setImposteriser(ClassImposteriser.INSTANCE);
    }};
	
	
	@Test
	public void testSingleSocketConnection() throws IOException {
		final InputStream is = context.mock(InputStream.class);
		final OutputStream os = context.mock(OutputStream.class);
		SimpleIOStream stream = new SimpleIOStream(is, os);
		assertSame(is, stream.getInputStream());
		assertSame(os, stream.getOutputStream());
		context.checking(new Expectations() {{
			oneOf(is).close();
			oneOf(os).close();
		}});
		stream.close();
	}
	
	@Test
	public void testExceptionsOnISClose() throws IOException {
		final InputStream is = context.mock(InputStream.class);
		final OutputStream os = context.mock(OutputStream.class);
		SimpleIOStream stream = new SimpleIOStream(is, os);
		context.checking(new Expectations() {{
			oneOf(is).close();will(throwException(new IOException()));
			oneOf(os).close();
		}});
		stream.close();
	}
	
	@Test
	public void testExceptionsOnOSClose() throws IOException {
		final InputStream is = context.mock(InputStream.class);
		final OutputStream os = context.mock(OutputStream.class);
		SimpleIOStream stream = new SimpleIOStream(is, os);
		context.checking(new Expectations() {{
			oneOf(is).close();
			oneOf(os).close();will(throwException(new IOException()));
		}});
		stream.close();
	}

}
