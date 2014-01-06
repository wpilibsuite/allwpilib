package edu.wpi.first.wpilibj.hal;

import java.nio.ByteBuffer;
import java.nio.IntBuffer;

public class I2CJNI extends JNIWrapper {
	public static native byte doI2CTransaction(byte address, byte compatibilityMode, ByteBuffer dataToSend, byte sendSize, ByteBuffer dataReceived, byte receiveSize, IntBuffer status);
	public static native byte doI2CTransactionWithModule(byte module, byte address, byte compatibilityMode, ByteBuffer dataToSend, byte sendSize, ByteBuffer dataReceived, byte receiveSize, IntBuffer status);
}
