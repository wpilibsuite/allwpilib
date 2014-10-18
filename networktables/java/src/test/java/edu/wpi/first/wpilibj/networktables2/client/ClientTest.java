package edu.wpi.first.wpilibj.networktables2.client;

import static org.junit.Assert.*;

import org.jmock.*;
import org.jmock.integration.junit4.*;
import org.junit.*;

import edu.wpi.first.wpilibj.networktables2.*;
import edu.wpi.first.wpilibj.networktables2.stream.*;
import edu.wpi.first.wpilibj.networktables2.thread.*;
import edu.wpi.first.wpilibj.networktables2.type.*;
import edu.wpi.first.wpilibj.tables.*;

public class ClientTest  {
	Mockery context = new JUnit4Mockery();
        NetworkTableClient client;
        
        @Before
        public void setup() {
            IOStreamFactory ioStreamFactory = context.mock(IOStreamFactory.class);
            final NTThreadManager threadManager = context.mock(NTThreadManager.class);
            context.checking(new Expectations() {{
                    oneOf(threadManager).newBlockingPeriodicThread(with(any(WriteManager.class)), with("Write Manager Thread"));
            }});
            client = new NetworkTableClient(ioStreamFactory, new NetworkTableEntryTypeManager(), threadManager);
        }
	
	@Test public void testOfflinePutGet() throws TableKeyNotDefinedException {
		client.putBoolean("MyKey", false);
		assertEquals(false, client.getBoolean("MyKey"));
		client.putBoolean("MyKey", true);
		assertEquals(true, client.getBoolean("MyKey"));
		client.putBoolean("MyKey", false);
		assertEquals(false, client.getBoolean("MyKey"));
	}
	
	@Test public void testUndefinedKey() {
		try{
			client.getBoolean("MyKey");
			fail();
		} catch(TableKeyNotDefinedException e){
			assertEquals("Unkown Table Key: MyKey", e.getMessage());
		}
	}
        
        @Test(expected=TableKeyExistsWithDifferentTypeException.class)
        public void testPutToExistingKeyWithDifferentTypeThrowsExeption() {
            client.putDouble("TestKey", 3.0);
            client.putString("TestKey", "abc");
        }
}
