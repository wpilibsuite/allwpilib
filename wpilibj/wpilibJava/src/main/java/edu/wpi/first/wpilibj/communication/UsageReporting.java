package edu.wpi.first.wpilibj.communication;

public class UsageReporting {

	public static void report(int resource, int instanceNumber, int i) {
		report(resource, instanceNumber, i, "");
	}
	
	public static void report(int resource, int instanceNumber) {
		report(resource, instanceNumber, 0, "");
	}

	public static void report(int resource, int instanceNumber, int i,
			String string) {
		FRC_NetworkCommunicationsLibrary.FRC_NetworkCommunication_nUsageReporting_report((byte)resource, (byte) instanceNumber, (byte) i, string);
	}

}
