package edu.wpi.first.wpilibj.networktables2.system;

import edu.wpi.first.testing.*;
import edu.wpi.first.wpilibj.networktables2.client.*;
import edu.wpi.first.wpilibj.networktables2.stream.*;
import edu.wpi.first.wpilibj.tables.*;
import java.util.HashMap;
import java.util.LinkedList;
import java.util.List;
import java.util.Map;
import java.util.Map.Entry;
import java.util.Random;
import org.junit.*;
import static org.junit.Assert.*;
import org.junit.runner.*;

@RunWith(sUnitTestRunner.class)
public class SystemTest {

	@Test
	public void testSimpleBidirectionalPut() throws Exception{
		sUnitTestRunner.server.waitMessage();//Server Up
		
		IOStreamFactory streamFactory = SocketStreams.newStreamFactory("10.1.90.2", 1735);
		NetworkTableClient client = new NetworkTableClient(streamFactory);
		client.reconnect();
		Thread.sleep(500);
		sUnitTestRunner.server.sendMessage("client connected");
		
		client.putString("ClientString1", "CValue1-1");
		client.putString("ClientString2", "CValue2-1");
		
		sUnitTestRunner.server.waitMessage();//Server sent
		Thread.sleep(100);
		assertEquals("SValue1-1", client.getString("ServerString1"));
		assertEquals("SValue2-1", client.getString("ServerString2"));
		client.putString("ClientString1", "CValue1-2");
		client.putString("ClientString2", "CValue2-2");
		sUnitTestRunner.server.sendMessage("client sent");
		
		sUnitTestRunner.server.waitMessage();//Server sent
		Thread.sleep(100);
		assertEquals("SValue1-2", client.getString("ServerString1"));
		assertEquals("SValue2-2", client.getString("ServerString2"));
		
		client.stop();
		
		sUnitTestRunner.server.sendMessage("test complete");
	}
	
	@Test
	public void testRapidServerPutSingleKey() throws Exception{
		sUnitTestRunner.server.waitMessage();//Server Up
		
		IOStreamFactory streamFactory = SocketStreams.newStreamFactory("10.1.90.2", 1735);
		NetworkTableClient client = new NetworkTableClient(streamFactory);
		client.reconnect();
		Thread.sleep(100);
		sUnitTestRunner.server.sendMessage("client connected");
		
		sUnitTestRunner.server.waitMessage();//Server done
		assertEquals("SValue9999", client.getString("ServerKey"));
		client.stop();
		
		
		sUnitTestRunner.server.sendMessage("test complete");
	}
	
	@Test
	public void testRapidServerPutMultiKey() throws Exception{
		sUnitTestRunner.server.waitMessage();//Server Up
		
		IOStreamFactory streamFactory = SocketStreams.newStreamFactory("10.1.90.2", 1735);
		NetworkTableClient client = new NetworkTableClient(streamFactory);
		client.reconnect();
		Thread.sleep(100);
		sUnitTestRunner.server.sendMessage("client connected");
		
		sUnitTestRunner.server.waitMessage();//Server done
		client.stop();
		
		for(int i = 0; i<100; ++i)
			assertEquals("SValue999", client.getString("ServerKey"+i));
		
		sUnitTestRunner.server.sendMessage("test complete");
	}
	
	@Test
	public void testPeriodicServerPutMultiKey() throws Exception{
		sUnitTestRunner.server.waitMessage();//Server Up
		
		IOStreamFactory streamFactory = SocketStreams.newStreamFactory("10.1.90.2", 1735);
		NetworkTableClient client = new NetworkTableClient(streamFactory);
		client.reconnect();
		Thread.sleep(100);
		sUnitTestRunner.server.sendMessage("client connected");
		
		sUnitTestRunner.server.waitMessage();//Server done
		client.stop();
		
		for(int i = 0; i<100; ++i)
			assertEquals("SValue99", client.getString("ServerKey"+i));
		
		sUnitTestRunner.server.sendMessage("test complete");
	}
        
        @Test
        public void testBiDirectionalStress() throws Exception {
            sUnitTestRunner.server.waitMessage(); // Server up
            
            IOStreamFactory streamFactory = SocketStreams.newStreamFactory("10.1.90.2", 1735);
            NetworkTableClient client = new NetworkTableClient(streamFactory);
            client.reconnect();
            Thread.sleep(100);
            sUnitTestRunner.server.sendMessage("Client connected");
            
            Map<String, Double> valueRecord = new HashMap<String, Double>();
            
            // Add and send 1 million values
            try {
                // Seed the RNG from a build property; otherwise, generate a random seed
                final String seedPropertyName = "edu.wpi.first.wpilibj.test.randomSeed";
                String seedStr = System.getProperty(seedPropertyName);
                boolean seedLoaded = false;
                
                Random rand;
                long seed;
                if(seedStr != null) {
                    seed = Long.parseLong(seedStr);
                    System.out.println("Seed loaded from property '" + seedPropertyName + "'");
                    seedLoaded = true;
                } else {
                    System.out.println("Property '" + seedPropertyName + "' not set; generating random seed");
                    Random seedRand = new Random();
                    seed = seedRand.nextLong();
                }
                System.out.println("Using seed: "+ seed);
                rand = new Random(seed);
                
                // Write values to the robot; try 10000 runs
                System.out.println("Writing values to the robot...");
                for(int i = 0; i < 10000; i++) {
                    // Pick a random name out of ascii characters
                    int nameLen = rand.nextInt(99) + 1;
                    StringBuilder sb = new StringBuilder(nameLen);
                    for(int j = 0; j < nameLen; j++) {
                        
                        // Pick a random human-readable ascii character
                        int c = rand.nextInt(94) + 32;
                        sb.append((char) c);
                    }
                    
                    // Pick a value to send from the range of +/- Double.MAX_VALUE
                    double value = rand.nextDouble() * Double.MAX_VALUE;
                    if(!rand.nextBoolean())
                        value *= -1;
                    
                    // If a seed was specified (i.e. we are probably debugging),
                    // print out the value sequence
                    if(seedLoaded) {
                        System.out.println(String.format("Setting %s to value %f", sb.toString(), value));
                    }
                    
                    valueRecord.put(sb.toString(), value);
                    client.putDouble("/client/" + sb.toString(), value);
                    
                    Thread.sleep(rand.nextInt(19) + 1);
                }
                
                System.out.println("Waiting for 10 seconds...");
                Thread.sleep(10000);
                List<String> incorrectEntries = new LinkedList<String>();
                
                // Compare the current value of every entry with the expected value
                for(Entry<String, Double> e : valueRecord.entrySet()) {
                    try {
                        Double d = client.getDouble("/server/" + e.getKey());

                        if(!e.getValue().equals(d))
                            incorrectEntries.add(e.getKey());
                    } catch (TableKeyNotDefinedException tknde) {
                        incorrectEntries.add(e.getKey());
                    }
                }
                
                // Note any/all failures
                if(incorrectEntries.size() > 0) {
                    for(String s : incorrectEntries) {
                        try {
                            Double d = client.getDouble("/server/" + s);
                            
                            System.err.println(String.format("For key '%s' expected %f, got %f", s, valueRecord.get(s), d));
                        } catch (TableKeyNotDefinedException tknde) {
                            System.err.println(String.format("No key %s; expected %f", s, valueRecord.get(s)));
                        }
                    }
                    
                    fail("Server namespace in network tables never became consistent with expected values");
                }
                sUnitTestRunner.server.sendMessage("Test complete");
            } catch(Exception e) {
                sUnitTestRunner.server.sendMessage("Test complete (exception occurred)");
                throw e;
            }
        }
}