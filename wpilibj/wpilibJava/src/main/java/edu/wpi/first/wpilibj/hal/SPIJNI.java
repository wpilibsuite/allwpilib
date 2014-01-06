package edu.wpi.first.wpilibj.hal;

import java.nio.ByteBuffer;
import java.nio.IntBuffer;

public class SPIJNI extends JNIWrapper {
	public static native ByteBuffer initializeSPI(byte sclk_routing_module, int sclk_routing_pin, byte mosi_routing_module, int mosi_routing_pin, byte miso_routing_module, int miso_routing_pin, IntBuffer status);
	public static native void cleanSPI(ByteBuffer spi_pointer, IntBuffer status);
	public static native void setSPIBitsPerWord(ByteBuffer spi_pointer, int bits, IntBuffer status);
	public static native int getSPIBitsPerWord(ByteBuffer spi_pointer, IntBuffer status);
	public static native void setSPIClockRate(ByteBuffer spi_pointer, double hz, IntBuffer status);
	public static native void setSPIMSBFirst(ByteBuffer spi_pointer, IntBuffer status);
	public static native void setSPILSBFirst(ByteBuffer spi_pointer, IntBuffer status);
	public static native void setSPISampleDataOnFalling(ByteBuffer spi_pointer, IntBuffer status);
	public static native void setSPISampleDataOnRising(ByteBuffer spi_pointer, IntBuffer status);
	public static native void setSPISlaveSelect(ByteBuffer spi_pointer, byte ss_routing_module, int ss_routing_pin, IntBuffer status);
	public static native void setSPILatchMode(ByteBuffer spi_pointer, int mode, IntBuffer status);
	public static native int getSPILatchMode(ByteBuffer spi_pointer, IntBuffer status);
	public static native void setSPIFramePolarity(ByteBuffer spi_pointer, byte activeLow, IntBuffer status);
	public static native byte getSPIFramePolarity(ByteBuffer spi_pointer, IntBuffer status);
	public static native void setSPIClockActiveLow(ByteBuffer spi_pointer, IntBuffer status);
	public static native void setSPIClockActiveHigh(ByteBuffer spi_pointer, IntBuffer status);
	public static native void applySPIConfig(ByteBuffer spi_pointer, IntBuffer status);
	public static native short getSPIOutputFIFOAvailable(ByteBuffer spi_pointer, IntBuffer status);
	public static native short getSPINumReceived(ByteBuffer spi_pointer, IntBuffer status);
	public static native byte isSPIDone(ByteBuffer spi_pointer, IntBuffer status);
	public static native byte hadSPIReceiveOverflow(ByteBuffer spi_pointer, IntBuffer status);
	public static native void writeSPI(ByteBuffer spi_pointer, int data, IntBuffer status);
	public static native int readSPI(ByteBuffer spi_pointer, byte initiate, IntBuffer status);
	public static native void resetSPI(ByteBuffer spi_pointer, IntBuffer status);
	public static native void clearSPIReceivedData(ByteBuffer spi_pointer, IntBuffer status);
}
