package edu.wpi.first.wpilibj.hal;
import java.nio.IntBuffer;
import java.nio.ByteBuffer;

public class SerialPortJNI extends JNIWrapper {
	public static native void serialInitializePort(byte port, IntBuffer status);
	public static native void serialSetBaudRate(byte port, int baud, IntBuffer status);
	public static native void serialSetDataBits(byte port, byte bits, IntBuffer status);
	public static native void serialSetParity(byte port, byte parity, IntBuffer status);
	public static native void serialSetStopBits(byte port, byte stopBits, IntBuffer status);
	public static native void serialSetWriteMode(byte port, byte mode, IntBuffer status);
	public static native void serialSetFlowControl(byte port, byte flow, IntBuffer status);
	public static native void serialSetTimeout(byte port, float timeout, IntBuffer status);
	public static native void serialEnableTermination(byte port, char terminator, IntBuffer status);
	public static native void serialDisableTermination(byte port, IntBuffer status);
	public static native void serialSetReadBufferSize(byte port, int size, IntBuffer status);
	public static native void serialSetWriteBufferSize(byte port, int size, IntBuffer status);
	public static native int serialGetBytesRecieved(byte port, IntBuffer status);
	public static native int serialRead(byte port, ByteBuffer buffer, int count, IntBuffer status);
	public static native int serialWrite(byte port, ByteBuffer buffer, int count, IntBuffer status);
	public static native void serialFlush(byte port, IntBuffer status);
	public static native void serialClear(byte port, IntBuffer status);
	public static native void serialClose(byte port, IntBuffer status);	
}