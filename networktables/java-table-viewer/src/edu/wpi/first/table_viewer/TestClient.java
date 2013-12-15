package edu.wpi.first.table_viewer;

import java.io.*;

import edu.wpi.first.wpilibj.networktables2.client.*;
import edu.wpi.first.wpilibj.networktables2.stream.*;


public class TestClient {

	/**
	 * @param args
	 * @throws IOException 
	 * @throws InterruptedException 
	 */
	public static void main(String[] args) throws IOException, InterruptedException {
		IOStreamFactory streamFactory = SocketStreams.newStreamFactory("10.1.90.2", 1735);
		NetworkTableClient client = new NetworkTableClient(streamFactory);
		client.reconnect();
		long last = System.currentTimeMillis();
		int num = 40;
		String[] keys = new String[num];
		for(int i = 0; i<num; ++i)
			keys[i] = "MyClientKey"+i;
		
		for(int i = 0; true; ++i){
			
			for(int j = 0; j<num; ++j)
				client.putBoolean(keys[j], i%2==0);


			//server.putNumber("Time", System.currentTimeMillis()-last);
			last = System.currentTimeMillis();
			Thread.sleep(20);
		}
	}

}
