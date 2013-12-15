package edu.wpi.first.wpilibj.networktables2;

import java.io.*;

import org.jmock.*;
import org.jmock.auto.*;
import org.jmock.integration.junit4.*;
import org.jmock.lib.legacy.*;
import org.junit.*;
import org.junit.runner.*;

@RunWith(JMock.class)
public class TransactionDirtierTest {
	Mockery context = new JUnit4Mockery() {{
        setImposteriser(ClassImposteriser.INSTANCE);
    }};
    
	@Mock OutgoingEntryReceiver receiver;
	TransactionDirtier dirtier;
	@Before public void before(){
		dirtier = new TransactionDirtier(receiver);
	}
	
	@Test public void testOutgoingCleanAssignment() throws IOException {
		final NetworkTableEntry entry = context.mock(NetworkTableEntry.class);
		
		context.checking(new Expectations() {{
			oneOf(entry).isDirty();will(returnValue(false));
			oneOf(entry).makeDirty();
			oneOf(receiver).offerOutgoingAssignment(entry);
		}});
		
		dirtier.offerOutgoingAssignment(entry);
	}
	@Test public void testOutgoingCleanUpdate() throws IOException {
		final NetworkTableEntry entry = context.mock(NetworkTableEntry.class);
		
		context.checking(new Expectations() {{
			oneOf(entry).isDirty();will(returnValue(false));
			oneOf(entry).makeDirty();
			oneOf(receiver).offerOutgoingUpdate(entry);
		}});
		
		dirtier.offerOutgoingUpdate(entry);
	}
	
	
	@Test public void testOutgoingDirtyAssignment() throws IOException {
		final NetworkTableEntry entry = context.mock(NetworkTableEntry.class);
		
		context.checking(new Expectations() {{
			oneOf(entry).isDirty();will(returnValue(true));
		}});
		
		dirtier.offerOutgoingAssignment(entry);
	}
	@Test public void testOutgoingDirtyUpdate() throws IOException {
		final NetworkTableEntry entry = context.mock(NetworkTableEntry.class);
		
		context.checking(new Expectations() {{
			oneOf(entry).isDirty();will(returnValue(true));
		}});
		
		dirtier.offerOutgoingUpdate(entry);
	}

}
