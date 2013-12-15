package edu.wpi.first.table_viewer;

import java.io.*;

import edu.wpi.first.wpilibj.networktables.*;


public class TestServer {

	/**
	 * @param args
	 * @throws IOException 
	 * @throws InterruptedException 
	 */
	public static void main(String[] args) throws IOException, InterruptedException {
		NetworkTable server = NetworkTable.getTable("SmartDashboard");
		//IOStreamProvider streamProvider = SocketStreams.newStreamProvider(1735);
		//NetworkTableServer server = new NetworkTableServer(streamProvider, new DefaultThreadManager(), new NetworkTableTransactionPool());
		long last = System.currentTimeMillis();
		int num = 40;
		String[] keys = new String[num];
		for(int i = 0; i<num; ++i)
			keys[i] = "MyKey"+i;
		
		for(int i = 0; true; ++i){
			
			for(int j = 0; j<num; ++j)
				server.putBoolean(keys[j], i%2==0);


			//server.putNumber("Time", System.currentTimeMillis()-last);
			last = System.currentTimeMillis();
			Thread.sleep(20);
		}
	}

}
