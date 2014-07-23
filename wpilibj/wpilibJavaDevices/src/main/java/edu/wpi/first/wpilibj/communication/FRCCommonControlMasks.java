package edu.wpi.first.wpilibj.communication;

public class FRCCommonControlMasks {
	public static byte CHECK_VERSIONS = 1 << 0;
	public static byte TEST = 1 << 1;
	public static byte RESYNC = 1 << 2;
	public static byte FMS_ATTATCHED = 1 << 3;
	public static byte AUTONOMOUS = 1 << 4;
	public static byte ENABLED = 1 << 5;
	public static byte NOT_ESTOP = 1 << 6;
	public static byte RESET = (byte) (1 << 7);
}
