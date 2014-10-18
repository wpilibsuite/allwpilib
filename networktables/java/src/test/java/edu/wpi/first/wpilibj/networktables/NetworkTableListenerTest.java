package edu.wpi.first.wpilibj.networktables;

import edu.wpi.first.wpilibj.networktables2.client.NetworkTableClient;
import edu.wpi.first.wpilibj.networktables2.stream.IOStream;
import edu.wpi.first.wpilibj.networktables2.stream.IOStreamFactory;
import edu.wpi.first.wpilibj.tables.*;
import java.io.*;
import org.jmock.Expectations;
import org.jmock.Mockery;
import org.jmock.auto.Mock;
import org.jmock.integration.junit4.JMock;
import org.jmock.integration.junit4.JUnit4Mockery;
import org.jmock.lib.legacy.ClassImposteriser;
import org.junit.*;
import org.junit.runner.RunWith;


@RunWith(JMock.class)
public class NetworkTableListenerTest {
	Mockery context = new JUnit4Mockery() {{
        setImposteriser(ClassImposteriser.INSTANCE);
    }};
        
    private NetworkTableClient client;
    private NetworkTableProvider provider;
    private NetworkTable testTable1;
    private NetworkTable testTable2;
    private NetworkTable testTable3;
    private NetworkTable testSubTable1;
    private NetworkTable testSubTable2;
    private NetworkTable testSubTable3;
    private NetworkTable testSubTable4;

    
    
    @Before
    public void init() throws IOException{
        client = new NetworkTableClient(new IOStreamFactory(){
            public IOStream createStream() throws IOException {
                return null;
            }
        });
        provider = new NetworkTableProvider(client);
        testTable1 = (NetworkTable)provider.getTable("/test1");
        testTable2 = (NetworkTable)provider.getTable("/test2");
        testSubTable1 = (NetworkTable)provider.getTable("/test2/sub1");
        testSubTable2 = (NetworkTable)provider.getTable("/test2/sub2");
        testTable3 = (NetworkTable)provider.getTable("/test3");
        testSubTable3 = (NetworkTable)provider.getTable("/test3/suba");
        testSubTable4 = (NetworkTable)provider.getTable("/test3/suba/subb");
    }
    @After
    public void cleanup(){
        provider.close();
    }

    @Mock ITableListener listener1;
    @Mock ITableListener listener2;
    
    
    @Test
    public void keyListenerImediateNotifyTest() throws IOException {
        testTable1.putBoolean("MyKey1", true);
        testTable1.putBoolean("MyKey1", false);
        testTable1.putBoolean("MyKey2", true);
        testTable1.putBoolean("MyKey4", false);
        context.checking(new Expectations() {{
            oneOf(listener1).valueChanged(testTable1, "MyKey1", false, true);
            oneOf(listener1).valueChanged(testTable1, "MyKey2", true, true);
            oneOf(listener1).valueChanged(testTable1, "MyKey4", false, true);
        }});
        testTable1.addTableListener(listener1, true);
        context.assertIsSatisfied();
        context.checking(new Expectations() {{
            oneOf(listener1).valueChanged(testTable1, "MyKey", false, true);
        }});
        testTable1.putBoolean("MyKey", false);
        context.assertIsSatisfied();
        context.checking(new Expectations() {{
            oneOf(listener1).valueChanged(testTable1, "MyKey1", true, false);
        }});
        testTable1.putBoolean("MyKey1", true);
        context.assertIsSatisfied();
        context.checking(new Expectations() {{
            oneOf(listener1).valueChanged(testTable1, "MyKey1", false, false);
        }});
        testTable1.putBoolean("MyKey1", false);
        context.assertIsSatisfied();
        context.checking(new Expectations() {{
            oneOf(listener1).valueChanged(testTable1, "MyKey4", true, false);
        }});
        testTable1.putBoolean("MyKey4", true);
        context.assertIsSatisfied();
    }
    
    @Test
    public void keyListenerNotImediateNotifyTest() throws IOException {
        testTable1.putBoolean("MyKey1", true);
        testTable1.putBoolean("MyKey1", false);
        testTable1.putBoolean("MyKey2", true);
        testTable1.putBoolean("MyKey4", false);
        testTable1.addTableListener(listener1, false);
        context.assertIsSatisfied();
        context.checking(new Expectations() {{
            oneOf(listener1).valueChanged(testTable1, "MyKey", false, true);
        }});
        testTable1.putBoolean("MyKey", false);
        context.assertIsSatisfied();
        /*context.checking(new Expectations() {{Should not fire because value was previously false
            oneOf(listener1).valueChanged(testTable1, "MyKey1", false, false);
        }});*/
        testTable1.putBoolean("MyKey1", false);
        context.assertIsSatisfied();
        context.checking(new Expectations() {{
            oneOf(listener1).valueChanged(testTable1, "MyKey1", true, false);
        }});
        testTable1.putBoolean("MyKey1", true);
        context.assertIsSatisfied();
        context.checking(new Expectations() {{
            oneOf(listener1).valueChanged(testTable1, "MyKey4", true, false);
        }});
        testTable1.putBoolean("MyKey4", true);
        context.assertIsSatisfied();
    }
    
    @Test
    public void subTableListenerTest() throws IOException {
        testTable2.putBoolean("MyKey1", true);
        testTable2.putBoolean("MyKey2", true);
        testTable2.addSubTableListener(listener1);
        testTable2.putBoolean("MyKey1", false);
        testTable2.putBoolean("MyKey4", false);
        
        
        context.checking(new Expectations() {{
            oneOf(listener1).valueChanged(testTable2, "sub1", testSubTable1, true);
        }});
        testSubTable1.putBoolean("MyKey1", false);
        context.assertIsSatisfied();
        testSubTable1.putBoolean("MyKey2", true);
        testSubTable1.putBoolean("MyKey1", true);
        
        context.checking(new Expectations() {{
            oneOf(listener1).valueChanged(testTable2, "sub2", testSubTable2, true);
        }});
        testSubTable2.putBoolean("MyKey1", false);
        context.assertIsSatisfied();
    }
    
    @Test
    public void subSubTableListenerTest() throws IOException {
        testTable3.addSubTableListener(listener1);
        testSubTable3.addSubTableListener(listener1);
        testSubTable4.addTableListener(listener1, true);
        
        
        context.checking(new Expectations() {{
            oneOf(listener1).valueChanged(testTable3, "suba", testSubTable3, true);
            oneOf(listener1).valueChanged(testSubTable3, "subb", testSubTable4, true);
            oneOf(listener1).valueChanged(testSubTable4, "MyKey1", false, true);
        }});
        testSubTable4.putBoolean("MyKey1", false);
        context.assertIsSatisfied();
        
        context.checking(new Expectations() {{
            oneOf(listener1).valueChanged(testSubTable4, "MyKey1", true, false);
        }});
        testSubTable4.putBoolean("MyKey1", true);
        context.assertIsSatisfied();
        
        
        context.checking(new Expectations() {{
            oneOf(listener2).valueChanged(testTable3, "suba", testSubTable3, true);
            oneOf(listener2).valueChanged(testSubTable3, "subb", testSubTable4, true);
            oneOf(listener2).valueChanged(testSubTable4, "MyKey1", true, true);
        }});
        testTable3.addSubTableListener(listener2);
        testSubTable3.addSubTableListener(listener2);
        testSubTable4.addTableListener(listener2, true);
        context.assertIsSatisfied();
    }

}