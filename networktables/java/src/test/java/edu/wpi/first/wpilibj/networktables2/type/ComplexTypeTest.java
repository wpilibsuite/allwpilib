package edu.wpi.first.wpilibj.networktables2.type;

import edu.wpi.first.wpilibj.networktables2.TableKeyExistsWithDifferentTypeException;
import edu.wpi.first.wpilibj.tables.TableKeyNotDefinedException;
import org.jmock.Expectations;
import org.jmock.Mockery;
import org.jmock.integration.junit4.JMock;
import org.jmock.integration.junit4.JUnit4Mockery;
import org.jmock.lib.legacy.ClassImposteriser;
import static org.junit.Assert.*;
import org.junit.Test;
import org.junit.runner.RunWith;
import test.util.NetworkTableTestNode;

/**
 *
 * @author Mitchell
 */
@RunWith(JMock.class)
public class ComplexTypeTest {
    Mockery context = new JUnit4Mockery(){{
        setImposteriser(ClassImposteriser.INSTANCE);
    }};
    
    @Test
    public void testPutComplexValue() throws Exception{
        NetworkTableTestNode node = new NetworkTableTestNode();
        
        final ComplexEntryType dataType = context.mock(ComplexEntryType.class);
        final ComplexData data1 = context.mock(ComplexData.class, "data1");
        final Object value1 = new Object();
        final ComplexData data2 = context.mock(ComplexData.class, "data2");
        final Object value2 = new Object();
        
        context.checking(new Expectations() {{
                allowing(data1).getType();will(returnValue(dataType));
                oneOf(dataType).internalizeValue("MyKey", data1, null);will(returnValue(value1));
        }});
        node.putValue("MyKey", data1);
        assertSame(value1, node.getValue("MyKey"));
        context.assertIsSatisfied();
        
        
        context.checking(new Expectations() {{
                allowing(data2).getType();will(returnValue(dataType));
                oneOf(dataType).internalizeValue("MyKey", data2, value1);will(returnValue(value2));
        }});
        node.putValue("MyKey", data2);
        assertSame(value2, node.getValue("MyKey"));
        context.assertIsSatisfied();
    }
    
    @Test
    public void testRetrieveComplexValue() throws Exception{
        NetworkTableTestNode node = new NetworkTableTestNode();
        
        final ComplexEntryType dataType = context.mock(ComplexEntryType.class);
        final ComplexData data = context.mock(ComplexData.class, "data");
        final Object value = new Object();
        
        context.checking(new Expectations() {{
                oneOf(dataType).internalizeValue("MyKey", value, null);will(returnValue(value));
        }});
        node.putValue("MyKey", dataType, value);
        context.assertIsSatisfied();
        
        context.checking(new Expectations() {{
                oneOf(dataType).exportValue("MyKey", value, data);
        }});
        node.retrieveValue("MyKey", data);
    }
    
    @Test
    public void testRetrieveNonExistantValue(){
        NetworkTableTestNode node = new NetworkTableTestNode();
        final ComplexData data = context.mock(ComplexData.class, "data");
        
        try{
            node.retrieveValue("MyKey", data);
            fail();
        } catch(TableKeyNotDefinedException e){}
    }
    
    @Test
    public void testRetrieveNonComplexValue() throws Exception{
        NetworkTableTestNode node = new NetworkTableTestNode();
        final ComplexData data = context.mock(ComplexData.class, "data");
        
        node.putString("MyKey", "SomeValue");
        try{
            node.retrieveValue("MyKey", data);
            fail();
        } catch(TableKeyExistsWithDifferentTypeException e){}
    }
}
