package edu.wpi.first.wpilibj.networktables2.client;

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
import edu.wpi.first.wpilibj.networktables2.*;
import edu.wpi.first.wpilibj.networktables2.AbstractNetworkTableEntryStore.TableListenerManager;
import edu.wpi.first.wpilibj.networktables2.connection.*;
import edu.wpi.first.wpilibj.networktables2.type.*;
import edu.wpi.first.wpilibj.tables.*;

@RunWith(JMock.class)
public class ClientNetworkTableEntryStoreTest {
    Mockery context = new JUnit4Mockery();
    
    TableListenerManager tlm;
    OutgoingEntryReceiver oer;
    ClientNetworkTableEntryStore clientStore;
    
    @Before
    public void setup() {
        tlm = context.mock(TableListenerManager.class);
        oer = context.mock(OutgoingEntryReceiver.class);
        clientStore = new ClientNetworkTableEntryStore(tlm);
        clientStore.setOutgoingReceiver(oer);
    }
    
    @Test(expected=TableKeyExistsWithDifferentTypeException.class)
    public void testPutOfExistingKeyWithDifferentTypeThrowsException() {
        context.checking(new Expectations() {{
            oneOf(tlm).fireTableListeners(with("TestKey"), with(any(Double.class)), with(any(boolean.class)));
        
            oneOf(oer).offerOutgoingAssignment(with(any(NetworkTableEntry.class)));
        }});
        
        clientStore.putOutgoing("TestKey", DefaultEntryTypes.DOUBLE, new Double(3));
        
        // This call should throw an exception, because the entry already exists
        // with type double.
        clientStore.putOutgoing("TestKey", DefaultEntryTypes.STRING, new String("abc"));
    }
}