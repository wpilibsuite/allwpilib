package edu.wpi.first.table_viewer;

import java.io.*;

import edu.wpi.first.wpilibj.networktables.*;

public class TestTableServer {
	static int numVars = 40;

	static {
		try {
			NetworkTable.setServerMode();
			NetworkTable.initialize();
		} catch (IOException ex) {
			ex.printStackTrace();
		}
	}
	static NetworkTable table = NetworkTable.getTable("SmartDashboard");

	static String[] vars = new String[numVars];
	static {
		for (int i = 0; i < numVars; i++) {
			vars[i] = "Variable" + i;
		}
	}

	static boolean value = false;

	static int counter = 0;
	static long loop;

	/**
	 * @param args
	 * @throws IOException
	 * @throws InterruptedException
	 */
	public static void main(String[] args) throws IOException,
			InterruptedException {
		while (true) {
			counter++;
			value = !value;
			loop = System.currentTimeMillis();
			for (int v = 0; v < numVars; v++) {
				table.putBoolean(vars[v], value);
			}
			loop = System.currentTimeMillis() - loop;
			try {
				Thread.sleep(20 - loop);
			} catch (Exception e) {
			}
		}
	}

}
