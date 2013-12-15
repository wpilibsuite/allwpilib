package edu.wpi.first.wpilibj.networktables2.type;

import edu.wpi.first.wpilibj.networktables2.TableKeyExistsWithDifferentTypeException;
import static org.junit.Assert.*;

import java.io.*;

import org.jmock.*;
import org.jmock.integration.junit4.*;
import org.jmock.lib.legacy.*;
import org.junit.*;
import org.junit.runner.*;

import test.util.*;
import edu.wpi.first.wpilibj.networktables2.client.*;
import edu.wpi.first.wpilibj.networktables2.server.*;
import edu.wpi.first.wpilibj.tables.*;

/**
 *
 * @author Mitchell
 */
@RunWith(JMock.class)
public class ArrayTypeTest {
    Mockery context = new JUnit4Mockery(){{
        setImposteriser(ClassImposteriser.INSTANCE);
    }};
    
    @Test
    public void testPutArrayValue() throws Exception{
        NetworkTableTestNode node = new NetworkTableTestNode();
        
        final NetworkTableEntryType type = context.mock(NetworkTableEntryType.class);
        final ArrayEntryType arrayType = new ArrayEntryType((byte)10, type, ArrayData.class);
        final ArrayData data = new ArrayData(arrayType);
        data.setSize(9);
        final Object value1 = new Object();
        final Object value2 = new Object();
        data._set(0, value1);
        data._set(1, value2);
        
        node.putValue("MyKey", data);
        Object[] internalValue = (Object[])node.getValue("MyKey");
        assertEquals(9, internalValue.length);
        assertSame(internalValue[0], value1);
        assertSame(internalValue[1], value2);
        for(int i = 2; i<9; ++i)
            assertNull(internalValue[i]);
        
        
        data._set(0, value2);
        data._set(1, value1);
        
        Object[] internalValue2 = (Object[])node.getValue("MyKey");
        assertSame(internalValue, internalValue2);
        assertEquals(9, internalValue2.length);
        assertSame(internalValue2[0], value1);
        assertSame(internalValue2[1], value2);
        for(int i = 2; i<9; ++i)
            assertNull(internalValue2[i]);
        
        node.putValue("MyKey", data);
        
        Object[] internalValue3 = (Object[])node.getValue("MyKey");
        assertSame(internalValue2, internalValue3);
        assertEquals(9, internalValue3.length);
        assertSame(internalValue3[0], value2);
        assertSame(internalValue3[1], value1);
        for(int i = 2; i<9; ++i)
            assertNull(internalValue3[i]);
        
        data.setSize(4);
        data._set(0, value1);
        data._set(1, value2);
        data._set(2, value2);
        data._set(3, value1);
        
        Object[] internalValue4 = (Object[])node.getValue("MyKey");
        assertSame(internalValue3, internalValue4);
        assertEquals(9, internalValue4.length);
        assertSame(internalValue4[0], value2);
        assertSame(internalValue4[1], value1);
        for(int i = 2; i<9; ++i)
            assertNull(internalValue4[i]);
        
        node.putValue("MyKey", data);
        
        Object[] internalValue5 = (Object[])node.getValue("MyKey");
        assertNotSame(internalValue4, internalValue5);
        assertEquals(4, internalValue5.length);
        assertSame(internalValue5[0], value1);
        assertSame(internalValue5[1], value2);
        assertSame(internalValue5[2], value2);
        assertSame(internalValue5[3], value1);
    }
    
    @Test
    public void testRetrieveArrayValue() throws Exception{
        NetworkTableTestNode node = new NetworkTableTestNode();
        
        final NetworkTableEntryType type = context.mock(NetworkTableEntryType.class);
        final ArrayEntryType arrayType = new ArrayEntryType((byte)10, type, ArrayData.class);
        final ArrayData data = new ArrayData(arrayType);
        data.setSize(4);
        final Object value1 = new Object();
        final Object value2 = new Object();
        final Object value3 = new Object();
        final Object value4 = new Object();
        data._set(0, value1);
        data._set(1, value2);
        data._set(2, value3);
        data._set(3, value4);
        
        node.putValue("MyKey", data);
        
        final ArrayData targetData = new ArrayData(arrayType);
        node.retrieveValue("MyKey", targetData);
        assertEquals(4, targetData.size());
        assertSame(targetData.getAsObject(0), value1);
        assertSame(targetData.getAsObject(1), value2);
        assertSame(targetData.getAsObject(2), value3);
        assertSame(targetData.getAsObject(3), value4);
        
        Object[] internalValue = (Object[])node.getValue("MyKey");
        assertNotSame(internalValue, targetData.getDataArray());
        
        
        targetData._set(0, value2);
        targetData._set(1, value1);
        
        Object[] internalValue2 = (Object[])node.getValue("MyKey");
        assertSame(internalValue, internalValue2);
        assertEquals(4, internalValue2.length);
        assertSame(internalValue[0], value1);
        assertSame(internalValue[1], value2);
        assertSame(internalValue[2], value3);
        assertSame(internalValue[3], value4);
    }
    
    @Test
    public void testCreateWithNonArrayExternal(){
        final NetworkTableEntryType elementType = context.mock(NetworkTableEntryType.class);
        try{
        	new ArrayEntryType((byte)2, elementType, String.class);
        	fail();
        } catch(Exception e){}
    }
        
    
    @Test
    public void testInternalizeDifferentExternalType(){
        final NetworkTableEntryType type = context.mock(NetworkTableEntryType.class);
        final ArrayEntryType arrayType = new ArrayEntryType((byte)10, type, BooleanArray.class);
        try{
	        arrayType.internalizeValue("MyKey", new ArrayData(arrayType), new Object[0]);
	        fail();
        }
        catch(TableKeyExistsWithDifferentTypeException e){
        }
    }
    
    @Test
    public void testExportDifferentExternalType(){
        final NetworkTableEntryType type = context.mock(NetworkTableEntryType.class);
        final ArrayEntryType arrayType = new ArrayEntryType((byte)10, type, BooleanArray.class);
        try{
	        arrayType.exportValue("MyKey", new Object[0], new ArrayData(arrayType));
	        fail();
        }
        catch(TableKeyExistsWithDifferentTypeException e){
        }
    }
    @Test
    public void testExportDifferentInternalType(){
        final NetworkTableEntryType type = context.mock(NetworkTableEntryType.class);
        final ArrayEntryType arrayType = new ArrayEntryType((byte)10, type, BooleanArray.class);
        try{
	        arrayType.exportValue("MyKey", new boolean[0], new BooleanArray());
	        fail();
        }
        catch(TableKeyExistsWithDifferentTypeException e){
        }
    }
    
    
    
    
    
    
    /*
     * Boolean
     */
    @Test
    public void testSendBooleanArray(){
        try {
            ByteArrayOutputStream output = new ByteArrayOutputStream();
            BooleanArray.TYPE.sendValue(new Object[]{true, false, true, true}, new DataOutputStream(output));
            assertArrayEquals(new byte[]{4, 1, 0, 1, 1}, output.toByteArray());
        } catch (IOException e) {
            fail();
        }
        try {
            ByteArrayOutputStream output = new ByteArrayOutputStream();
            BooleanArray.TYPE.sendValue(new Object[]{false, true, true, true, false, false, false, true}, new DataOutputStream(output));
            assertArrayEquals(new byte[]{8, 0, 1, 1, 1, 0, 0, 0, 1}, output.toByteArray());
        } catch (IOException e) {
            fail();
        }
        try {
            ByteArrayOutputStream output = new ByteArrayOutputStream();
            BooleanArray.TYPE.sendValue(new Object[1000], new DataOutputStream(output));
            fail();
        } catch (IOException e) {}
        try {
            ByteArrayOutputStream output = new ByteArrayOutputStream();
            BooleanArray.TYPE.sendValue(new Object(), new DataOutputStream(output));
            fail();
        } catch (IOException e) {}
    }
    
    @Test
    public void testReadBooleanArray() {
        try {
            ByteArrayInputStream input = new ByteArrayInputStream(new byte[]{4, 0, 1, 1, 0});
            assertArrayEquals(new Object[]{false, true, true, false}, (Object[])BooleanArray.TYPE.readValue(new DataInputStream(input)));
        } catch (IOException e) {
            fail();
        }
        try {
            ByteArrayInputStream input = new ByteArrayInputStream(new byte[]{9, 0, 1, 1, 1, 0, 0, 1, 1, 1});
            assertArrayEquals(new Object[]{false, true, true, true, false, false, true, true, true}, (Object[])BooleanArray.TYPE.readValue(new DataInputStream(input)));
        } catch (IOException e) {
            fail();
        }
    }

    @Test
    public void testReadWriteBooleanArray() throws Exception{
        StreamPipeProvider streamProvider = new StreamPipeProvider();
        NetworkTableServer server = new NetworkTableServer(streamProvider);
        NetworkTableClient client = new NetworkTableClient(streamProvider.getFactory());

        final BooleanArray data = new BooleanArray();
        data.setSize(4);
        data.set(0, true);
        data.set(1, false);
        data.set(2, false);
        data.set(3, true);

        server.putValue("MyKey", data);

        Thread.sleep(1500);

        final BooleanArray targetData = new BooleanArray();
        client.retrieveValue("MyKey", targetData);
        assertEquals(4, targetData.size());
        assertEquals(targetData.get(0), true);
        assertEquals(targetData.get(1), false);
        assertEquals(targetData.get(2), false);
        assertEquals(targetData.get(3), true);
        client.stop();
        server.close();
    }
    
    
    
    
    
    
    /*
     * Double
     */
    @Test
    public void testSendNumberArray(){
        try {
            ByteArrayOutputStream output = new ByteArrayOutputStream();
            NumberArray.TYPE.sendValue(new Object[]{new Double(10), new Double(20), new Double(200.3), new Double(-1.3)}, new DataOutputStream(output));
            assertArrayEquals(new byte[]{4, 64, 36, 0, 0, 0, 0, 0, 0, 64, 52, 0, 0, 0, 0, 0, 0, 64, 105, 9, -103, -103, -103, -103, -102, -65, -12, -52, -52, -52, -52, -52, -51}, output.toByteArray());
        } catch (IOException e) {
            fail();
        }
        try {
            ByteArrayOutputStream output = new ByteArrayOutputStream();
            NumberArray.TYPE.sendValue(new Object[]{new Double(10), new Double(20), new Double(200.3), new Double(-1.3), new Double(11), new Double(-3.3), new Double(11), new Double(66.2)}, new DataOutputStream(output));
            assertArrayEquals(new byte[]{8, 64, 36, 0, 0, 0, 0, 0, 0, 64, 52, 0, 0, 0, 0, 0, 0, 64, 105, 9, -103, -103, -103, -103, -102, -65, -12, -52, -52, -52, -52, -52, -51, 64, 38, 0, 0, 0, 0, 0, 0, -64, 10, 102, 102, 102, 102, 102, 102, 64, 38, 0, 0, 0, 0, 0, 0, 64, 80, -116, -52, -52, -52, -52, -51}, output.toByteArray());
        } catch (IOException e) {
            fail();
        }
        try {
            ByteArrayOutputStream output = new ByteArrayOutputStream();
            NumberArray.TYPE.sendValue(new Object[1000], new DataOutputStream(output));
            fail();
        } catch (IOException e) {}
        try {
            ByteArrayOutputStream output = new ByteArrayOutputStream();
            NumberArray.TYPE.sendValue(new Object(), new DataOutputStream(output));
            fail();
        } catch (IOException e) {}
    }
    
    @Test
    public void testReadNumberArray() {
        try {
            ByteArrayInputStream input = new ByteArrayInputStream(new byte[]{4, 64, 36, 0, 0, 0, 0, 0, 0, 64, 52, 0, 0, 0, 0, 0, 0, 64, 105, 9, -103, -103, -103, -103, -102, -65, -12, -52, -52, -52, -52, -52, -51});
            assertArrayEquals(new Object[]{new Double(10), new Double(20), new Double(200.3), new Double(-1.3)}, (Object[])NumberArray.TYPE.readValue(new DataInputStream(input)));
        } catch (IOException e) {
            fail();
        }
        try {
            ByteArrayInputStream input = new ByteArrayInputStream(new byte[]{8, 64, 36, 0, 0, 0, 0, 0, 0, 64, 52, 0, 0, 0, 0, 0, 0, 64, 105, 9, -103, -103, -103, -103, -102, -65, -12, -52, -52, -52, -52, -52, -51, 64, 38, 0, 0, 0, 0, 0, 0, -64, 10, 102, 102, 102, 102, 102, 102, 64, 38, 0, 0, 0, 0, 0, 0, 64, 80, -116, -52, -52, -52, -52, -51});
            assertArrayEquals(new Object[]{new Double(10), new Double(20), new Double(200.3), new Double(-1.3), new Double(11), new Double(-3.3), new Double(11), new Double(66.2)}, (Object[])NumberArray.TYPE.readValue(new DataInputStream(input)));
        } catch (IOException e) {
            fail();
        }
    }

    @Test
    public void testReadWriteNumberArray() throws Exception{
        StreamPipeProvider streamProvider = new StreamPipeProvider();
        NetworkTableServer server = new NetworkTableServer(streamProvider);
        NetworkTableClient client = new NetworkTableClient(streamProvider.getFactory());

        final NumberArray data = new NumberArray();
        data.setSize(4);
        data.set(0, 10.4);
        data.set(1, 44.3);
        data.set(2, 33.4);
        data.set(3, 233);

        server.putValue("MyKey", data);

        Thread.sleep(1500);

        final NumberArray targetData = new NumberArray();
        client.retrieveValue("MyKey", targetData);
        assertEquals(4, targetData.size());
        assertEquals(targetData.get(0), 10.4, 0);
        assertEquals(targetData.get(1), 44.3, 0);
        assertEquals(targetData.get(2), 33.4, 0);
        assertEquals(targetData.get(3), 233, 0);
        client.stop();
        server.close();
    }
    
    
    
    
    
    
    /*
     * String
     */
    @Test
    public void testSendStringArray(){
        try {
            ByteArrayOutputStream output = new ByteArrayOutputStream();
            StringArray.TYPE.sendValue(new Object[]{"HI", "", "HELlO", "--asdf"}, new DataOutputStream(output));
            assertArrayEquals(new byte[]{4, 0, 2, 72, 73, 0, 0, 0, 5, 72, 69, 76, 108, 79, 0, 6, 45, 45, 97, 115, 100, 102}, output.toByteArray());
        } catch (IOException e) {
            fail();
        }
        try {
            ByteArrayOutputStream output = new ByteArrayOutputStream();
            StringArray.TYPE.sendValue(new Object[]{"", "TEw4390", "00*&(&(*&*", "adsf", "asdfasdf", "asdfai888"}, new DataOutputStream(output));
            assertArrayEquals(new byte[]{6, 0, 0, 0, 7, 84, 69, 119, 52, 51, 57, 48, 0, 10, 48, 48, 42, 38, 40, 38, 40, 42, 38, 42, 0, 4, 97, 100, 115, 102, 0, 8, 97, 115, 100, 102, 97, 115, 100, 102, 0, 9, 97, 115, 100, 102, 97, 105, 56, 56, 56}, output.toByteArray());
        } catch (IOException e) {
            fail();
        }
        try {
            ByteArrayOutputStream output = new ByteArrayOutputStream();
            StringArray.TYPE.sendValue(new Object[1000], new DataOutputStream(output));
            fail();
        } catch (IOException e) {}
        try {
            ByteArrayOutputStream output = new ByteArrayOutputStream();
            StringArray.TYPE.sendValue(new Object(), new DataOutputStream(output));
            fail();
        } catch (IOException e) {}
    }
    
    @Test
    public void testReadStringArray() {
        try {
            ByteArrayInputStream input = new ByteArrayInputStream(new byte[]{4, 0, 2, 72, 73, 0, 0, 0, 5, 72, 69, 76, 108, 79, 0, 6, 45, 45, 97, 115, 100, 102});
            assertArrayEquals(new Object[]{"HI", "", "HELlO", "--asdf"}, (Object[])StringArray.TYPE.readValue(new DataInputStream(input)));
        } catch (IOException e) {
            fail();
        }
        try {
            ByteArrayInputStream input = new ByteArrayInputStream(new byte[]{6, 0, 0, 0, 7, 84, 69, 119, 52, 51, 57, 48, 0, 10, 48, 48, 42, 38, 40, 38, 40, 42, 38, 42, 0, 4, 97, 100, 115, 102, 0, 8, 97, 115, 100, 102, 97, 115, 100, 102, 0, 9, 97, 115, 100, 102, 97, 105, 56, 56, 56});
            assertArrayEquals(new Object[]{"", "TEw4390", "00*&(&(*&*", "adsf", "asdfasdf", "asdfai888"}, (Object[])StringArray.TYPE.readValue(new DataInputStream(input)));
        } catch (IOException e) {
            fail();
        }
    }

    @Test
    public void testReadWriteStringArray() throws Exception{
        StreamPipeProvider streamProvider = new StreamPipeProvider();
        NetworkTableServer server = new NetworkTableServer(streamProvider);
        NetworkTableClient client = new NetworkTableClient(streamProvider.getFactory());

        final StringArray data = new StringArray();
        data.setSize(5);
        data.set(0, "HI");
        data.set(1, "ehllos");
        data.set(2, "sdf-9889");
        data.set(3, "hi-----");
        data.set(4, "sfdasdfasdfasdfasf");

        server.putValue("MyKey", data);

        Thread.sleep(1500);

        final StringArray targetData = new StringArray();
        client.retrieveValue("MyKey", targetData);
        assertEquals(5, targetData.size());
        assertEquals(targetData.get(0), "HI");
        assertEquals(targetData.get(1), "ehllos");
        assertEquals(targetData.get(2), "sdf-9889");
        assertEquals(targetData.get(3), "hi-----");
        assertEquals(targetData.get(4), "sfdasdfasdfasdfasf");
        client.stop();
        server.close();
    }
}
