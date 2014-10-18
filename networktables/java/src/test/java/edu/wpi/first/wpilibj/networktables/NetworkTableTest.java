package edu.wpi.first.wpilibj.networktables;

import edu.wpi.first.wpilibj.networktables2.client.NetworkTableClient;
import edu.wpi.first.wpilibj.networktables2.stream.IOStream;
import edu.wpi.first.wpilibj.networktables2.stream.IOStreamFactory;
import edu.wpi.first.wpilibj.tables.*;
import java.io.*;
import org.junit.*;
import static org.junit.Assert.*;


public class NetworkTableTest {
    private NetworkTableClient client;
    private NetworkTableProvider provider;
    private NetworkTable testTable1;
    private NetworkTable testTable2;

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
    }
    @After
    public void cleanup(){
        provider.close();
    }

    @Test
    public void putDoubleTest() throws IOException {
            double testDouble = 43.43;
            testTable1.putNumber("double", 42.42);
            try {
                    testDouble = testTable1.getNumber("double");
            } catch (TableKeyNotDefinedException e) {
                    e.printStackTrace();
            }
            assertEquals(42.42, testDouble, 0.0);
            try {
                    testDouble = testTable1.getNumber("Non-Existant");
                    fail();
            } catch (TableKeyNotDefinedException e) {
            }
            testDouble = testTable1.getNumber("Non-Existant", 44.44);
            assertEquals(44.44, testDouble, 0.0);
    }

    @Test
    public void putBooleanTest() throws IOException {
            boolean testBool = false;
            testTable1.putBoolean("boolean", true);
            try {
                    testBool = testTable1.getBoolean("boolean");
            } catch (TableKeyNotDefinedException e) {
                    e.printStackTrace();
            }
            assertTrue(testBool);
            try {
                    testBool = testTable1.getBoolean("Non-Existant");
                    fail();
            } catch (TableKeyNotDefinedException e) {
            }
            testBool = testTable1.getBoolean("Non-Existant", false);
            assertFalse(testBool);
    }

    @Test
    public void putStringTest() throws IOException {
            String testString = "Initialized Test";
            testTable1.putString("String", "Test 1");
            try {
                    testString = testTable1.getString("String");
            } catch (TableKeyNotDefinedException e) {
                    e.printStackTrace();
            }
            assertEquals("Test 1", testString);
            try {
                    testString = testTable1.getString("Non-Existant");
                    fail();
            } catch (TableKeyNotDefinedException e) {
            }
            testString = testTable1.getString("Non-Existant", "Test 3");
            assertEquals("Test 3", testString);
    }

    @Test
    public void putMultiDataTypeTest() throws TableKeyNotDefinedException, IOException {
            double double1 = 1;
            double double2 = 2;
            double double3 = 3;
            boolean bool1 = false;
            boolean bool2 = true;
            String string1 = "String 1";
            String string2 = "String 2";
            String string3 = "String 3";

            testTable1.putNumber("double1", double1);
            testTable1.putNumber("double2", double2);
            testTable1.putNumber("double3", double3);
            testTable1.putBoolean("bool1", bool1);
            testTable1.putBoolean("bool2", bool2);
            testTable1.putString("string1", string1);
            testTable1.putString("string2", string2);
            testTable1.putString("string3", string3);

            assertEquals(double1, testTable1.getNumber("double1"), 0.0);
            assertEquals(double2, testTable1.getNumber("double2"), 0.0);
            assertEquals(double3, testTable1.getNumber("double3"), 0.0);
            assertEquals(bool1, testTable1.getBoolean("bool1"));
            assertEquals(bool2, testTable1.getBoolean("bool2"));
            assertEquals(string1, testTable1.getString("string1"));
            assertEquals(string2, testTable1.getString("string2"));
            assertEquals(string3, testTable1.getString("string3"));

            double1 = 4;
            double2 = 5;
            double3 = 6;
            bool1 = true;
            bool2 = false;
            string1 = "String 4";
            string2 = "String 5";
            string3 = "String 6";

            testTable1.putNumber("double1", double1);
            testTable1.putNumber("double2", double2);
            testTable1.putNumber("double3", double3);
            testTable1.putBoolean("bool1", bool1);
            testTable1.putBoolean("bool2", bool2);
            testTable1.putString("string1", string1);
            testTable1.putString("string2", string2);
            testTable1.putString("string3", string3);

            assertEquals(double1, testTable1.getNumber("double1"), 0.0);
            assertEquals(double2, testTable1.getNumber("double2"), 0.0);
            assertEquals(double3, testTable1.getNumber("double3"), 0.0);
            assertEquals(bool1, testTable1.getBoolean("bool1"));
            assertEquals(bool2, testTable1.getBoolean("bool2"));
            assertEquals(string1, testTable1.getString("string1"));
            assertEquals(string2, testTable1.getString("string2"));
            assertEquals(string3, testTable1.getString("string3"));

    }

    @Test
    public void multiTableTest() throws IOException {
            double table1double = 1;
            double table2double = 2;
            boolean table1boolean = true;
            boolean table2boolean = false;
            String table1String = "Table 1";
            String table2String = "Table 2";

            testTable1.putNumber("table1double", table1double);
            testTable1.putBoolean("table1boolean", table1boolean);
            testTable1.putString("table1String", table1String);

            try {
                    testTable2.getNumber("table1double");
                    fail();
            } catch(TableKeyNotDefinedException e){}
            try {
                    testTable2.getBoolean("table1boolean");
                    fail();
            } catch(TableKeyNotDefinedException e){}
            try {
                    testTable2.getString("table1String");
                    fail();
            } catch(TableKeyNotDefinedException e){}

            testTable2.putNumber("table2double", table2double);
            testTable2.putBoolean("table2boolean", table2boolean);
            testTable2.putString("table2String", table2String);

            try {
                    testTable1.getNumber("table2double");
                    fail();
            } catch(TableKeyNotDefinedException e){}
            try {
                    testTable1.getBoolean("table2boolean");
                    fail();
            } catch(TableKeyNotDefinedException e){}
            try {
                    testTable1.getString("table2String");
                    fail();
            } catch(TableKeyNotDefinedException e){}
    }

    @Test
    public void getTableTest() throws IOException{
            assertSame(testTable1, provider.getTable("/test1"));
            assertSame(testTable2, provider.getTable("/test2"));
            assertNotSame(testTable1, provider.getTable("/test2"));
            ITable testTable3 = provider.getTable("/test3");
            assertNotSame(testTable1, testTable3);
            assertNotSame(testTable2, testTable3);
    }
}