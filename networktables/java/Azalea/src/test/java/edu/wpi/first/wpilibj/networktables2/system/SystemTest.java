package edu.wpi.first.wpilibj.networktables2.system;

import edu.wpi.first.testing.*;
import edu.wpi.first.wpilibj.networktables2.server.*;
import edu.wpi.first.wpilibj.networktables2.stream.*;
import edu.wpi.first.wpilibj.tables.ITable;
import edu.wpi.first.wpilibj.tables.ITableListener;
import java.io.*;

public class SystemTest extends TestClass {
        private static NetworkTableServer staticServer;
        static{
            try {
                staticServer = new NetworkTableServer(SocketStreams.newStreamProvider(1735));
            } catch (IOException ex) {
                ex.printStackTrace();
            }
        }
	NetworkTableServer server;
	public void before() throws IOException{
            server = staticServer;
            staticServer.getEntryStore().clearEntries();
	}
	public void after(){
		//server.close();
	}
	
	public void testSimpleBidirectionalPut() throws Exception{
		sendMessage("server up");
				
		waitMessage();//Client connected
		System.out.println("client sent");
		sleep(100);
		assertEquals("CValue1-1", server.getString("ClientString1"));
		assertEquals("CValue2-1", server.getString("ClientString2"));
		server.putString("ServerString1", "SValue1-1");
		server.putString("ServerString2", "SValue2-1");
		sendMessage("server sent");
		System.out.println("sent server");
		
		waitMessage();//Client sent
		System.out.println("client sent");
		sleep(100);
		assertEquals("CValue1-2", server.getString("ClientString1"));
		assertEquals("CValue2-2", server.getString("ClientString2"));
		server.putString("ServerString1", "SValue1-2");
		server.putString("ServerString2", "SValue2-2");
		sendMessage("server sent");
		System.out.println("sent server");
		
		waitMessage();//test complete
	}
	public void testRapidServerPutSingleKey() throws Exception{
		sendMessage("server up");
				
		waitMessage();//Client connected


		for(int i = 0; i<10000; ++i)
			server.putString("ServerKey", "SValue"+i);
		
		sleep(100);
		sendMessage("server done");
		
		waitMessage();//test complete
	}
	public void testRapidServerPutMultiKey() throws Exception{
		sendMessage("server up");
				
		waitMessage();//Client connected


		for(int i = 0; i<1000; ++i)
			for(int j = 0; j<100; ++j)
				server.putString("ServerKey"+j, "SValue"+i);
		
		sleep(100);
		sendMessage("server done");
		
		waitMessage();//test complete
	}
	public void testPeriodicServerPutMultiKey() throws Exception{
		sendMessage("server up");
				
		waitMessage();//Client connected


		for(int i = 0; i<100; ++i){
			for(int j = 0; j<100; ++j)
				server.putString("ServerKey"+j, "SValue"+i);
			sleep(20);
		}
		sleep(100);
		
		sendMessage("server done");
		
		waitMessage();//test complete
	}
        public void testBiDirectionalStress() throws Exception{
            server.addTableListener(new ITableListener() {

                public void valueChanged(ITable source, String key, Object value, boolean isNew) {
                    int prefixIndex = key.indexOf("/client/");
                    if(prefixIndex == 0) {
                        String name = key.substring("/client/".length());
                        server.putDouble("/server/" + name, ((Double) value).doubleValue());
                    }
                }
            }, false);
            sendMessage("Server ready");
            waitMessage(); // Client connected
            
            waitMessage(); // Client done
        }
}
