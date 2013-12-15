package edu.wpi.first.wpilibj.networktables2;

import static org.junit.Assert.*;

import java.io.*;

import org.jmock.*;
import org.jmock.integration.junit4.*;
import org.jmock.lib.legacy.*;
import org.junit.*;
import org.junit.runner.*;

import test.util.*;
import edu.wpi.first.wpilibj.networktables2.AbstractNetworkTableEntryStore.TableListenerManager;
import edu.wpi.first.wpilibj.networktables2.connection.*;
import edu.wpi.first.wpilibj.networktables2.type.*;

@RunWith(JMock.class)
public class NetworkTableEntryTest {
	Mockery context = new JUnit4Mockery() {{
        setImposteriser(ClassImposteriser.INSTANCE);
    }};

	@Test public void testSendValue() throws IOException {
		final NetworkTableEntryType type = context.mock(NetworkTableEntryType.class);
		final Object value = "MyValue";
		final NetworkTableEntry entry = new NetworkTableEntry((char)0, "MyKey", (char)0, type, value);
		final DataOutputStream os = context.mock(DataOutputStream.class);

		context.checking(new Expectations() {{
			oneOf(type).sendValue(value, os);
		}});

		entry.sendValue(os);
	}

	@Test public void testToString() throws IOException {
		final NetworkTableEntry entry = NetworkTableEntryUtil.newBooleanEntry("MyKey", false);
		assertEquals( "Network Table Boolean entry: MyKey: 65535 - 0 - false", entry.toString());
	}

	@Test public void testSend() throws IOException {
		final NetworkTableEntry entry = NetworkTableEntryUtil.newBooleanEntry((char)0, "MyBoolean", (char)0, true);
		final NetworkTableConnection connection = context.mock(NetworkTableConnection.class);

		context.checking(new Expectations() {{
			oneOf(connection).sendEntryAssignment(entry);
		}});

		entry.send(connection);
	}

	@Test public void testConstructor() throws IOException {
		final NetworkTableEntry entry = new NetworkTableEntry((char)10, "MyNotBoolean", (char)2, DefaultEntryTypes.STRING, "Test1");
		assertEquals((char)10, entry.getId());
		assertEquals("MyNotBoolean", entry.name);
		assertEquals((char)2, entry.getSequenceNumber());
		assertEquals(DefaultEntryTypes.STRING, entry.getType());
		assertEquals("Test1", entry.getValue());
	}


	@Test public void testPut() throws IOException {
		final NetworkTableEntry entry = NetworkTableEntryUtil.newStringEntry((char)10, "MyString", (char)2, "Test1");
		assertTrue(entry.putValue((char)3, "Test5"));
		assertEquals("Test5", entry.getValue());
		assertEquals((char)3, entry.getSequenceNumber());

		assertTrue(entry.putValue((char)4, "Test2"));
		assertEquals("Test2", entry.getValue());
		assertEquals((char)4, entry.getSequenceNumber());

		assertFalse(entry.putValue((char)3, "Test3"));
		assertEquals("Test2", entry.getValue());
		assertEquals((char)4, entry.getSequenceNumber());

		assertFalse(entry.putValue((char)40000, "Test22"));
		assertEquals("Test2", entry.getValue());
		assertEquals((char)4, entry.getSequenceNumber());

		assertTrue(entry.putValue((char)30000, "Test10"));
		assertEquals("Test10", entry.getValue());
		assertEquals((char)30000, entry.getSequenceNumber());

		assertTrue(entry.putValue((char)40000, "Test23"));
		assertEquals("Test23", entry.getValue());
		assertEquals((char)40000, entry.getSequenceNumber());

		assertFalse(entry.putValue((char)30000, "Test100"));
		assertEquals("Test23", entry.getValue());
		assertEquals((char)40000, entry.getSequenceNumber());

		assertTrue(entry.putValue((char)0, "Test0"));
		assertEquals("Test0", entry.getValue());
		assertEquals((char)0, entry.getSequenceNumber());
	}

	@Test public void testSetWhenValid() throws IOException {
		final NetworkTableEntry entry = NetworkTableEntryUtil.newStringEntry("MyString", "Test1");
		entry.setId((char)100);
	}
	@Test public void testSetWhenNotValid() throws IOException {
		final NetworkTableEntry entry = NetworkTableEntryUtil.newStringEntry((char)10, "MyString", (char)2, "Test1");
		try{
			entry.setId((char)100);
			fail();
		} catch(RuntimeException e){
		}
	}
	@Test public void testClearId() throws IOException {
		final NetworkTableEntry entry = NetworkTableEntryUtil.newDoubleEntry("MyString", 203.2);
		entry.setId((char)10);
		assertEquals((char)10, entry.getId());
		entry.clearId();
		assertEquals(NetworkTableEntry.UNKNOWN_ID, entry.getId());
		entry.setId((char)22);
		assertEquals((char)22, entry.getId());
	}
	
	@Test public void testDirtyness() throws IOException {
		final NetworkTableEntry entry = NetworkTableEntryUtil.newDoubleEntry("MyString", 203.2);
		assertEquals(false, entry.isDirty());
		entry.makeClean();
		assertEquals(false, entry.isDirty());
		entry.makeDirty();
		assertEquals(true, entry.isDirty());
		entry.makeClean();
		assertEquals(false, entry.isDirty());
	}
	
	@Test public void testForcePut() throws IOException {
		final NetworkTableEntry entry = NetworkTableEntryUtil.newStringEntry((char)10, "MyString", (char)2, "Test1");
		entry.forcePut((char)3, "Test5");
		assertEquals("Test5", entry.getValue());
		assertEquals((char)3, entry.getSequenceNumber());

		entry.forcePut((char)4, "Test2");
		assertEquals("Test2", entry.getValue());
		assertEquals((char)4, entry.getSequenceNumber());

		entry.forcePut((char)3, "Test3");
		assertEquals("Test3", entry.getValue());
		assertEquals((char)3, entry.getSequenceNumber());

		entry.forcePut((char)40000, "Test22");
		assertEquals("Test22", entry.getValue());
		assertEquals((char)40000, entry.getSequenceNumber());

		entry.forcePut((char)30000, "Test10");
		assertEquals("Test10", entry.getValue());
		assertEquals((char)30000, entry.getSequenceNumber());
	}
	
	@Test public void testForcePutWithType() throws IOException {
		final NetworkTableEntry entry = NetworkTableEntryUtil.newStringEntry((char)10, "MyString", (char)2, "Test1");
		entry.forcePut((char)3, DefaultEntryTypes.BOOLEAN, true);
		assertEquals(DefaultEntryTypes.BOOLEAN, entry.getType());
		assertEquals(true, entry.getValue());
		assertEquals((char)3, entry.getSequenceNumber());

		entry.forcePut((char)4, DefaultEntryTypes.STRING, "HELLO");
		assertEquals("HELLO", entry.getValue());
		assertEquals(DefaultEntryTypes.STRING, entry.getType());
		assertEquals((char)4, entry.getSequenceNumber());

		entry.forcePut((char)3, DefaultEntryTypes.DOUBLE, 11.5);
		assertEquals(DefaultEntryTypes.DOUBLE, entry.getType());
		assertEquals(11.5, entry.getValue());
		assertEquals((char)3, entry.getSequenceNumber());
		
		entry.forcePut((char)3, DefaultEntryTypes.STRING, "HI");
		assertEquals(DefaultEntryTypes.STRING, entry.getType());
		assertEquals("HI", entry.getValue());
		assertEquals((char)3, entry.getSequenceNumber());
	}
	
	@Test public void testFireListener() throws IOException {
		final NetworkTableEntry entry = NetworkTableEntryUtil.newStringEntry((char)10, "MyString", (char)2, "Test1");
		final TableListenerManager listenerManager = context.mock(TableListenerManager.class);
		
		context.checking(new Expectations() {{
			oneOf(listenerManager).fireTableListeners("MyString", "Test1", true);
		}});
		entry.fireListener(listenerManager);
		context.assertIsSatisfied();
		
		context.checking(new Expectations() {{
			oneOf(listenerManager).fireTableListeners("MyString", "Test1", false);
		}});
		entry.fireListener(listenerManager);
		context.assertIsSatisfied();
		
		entry.forcePut((char)0, "TEST3");
		context.checking(new Expectations() {{
			oneOf(listenerManager).fireTableListeners("MyString", "TEST3", false);
		}});
		entry.fireListener(listenerManager);
		context.assertIsSatisfied();
	}
	
}
