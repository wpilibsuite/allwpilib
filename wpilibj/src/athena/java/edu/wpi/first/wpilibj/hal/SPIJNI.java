package edu.wpi.first.wpilibj.hal;

import java.nio.ByteBuffer;

public class SPIJNI extends JNIWrapper {
  public static native void spiInitialize(byte port);

  public static native int spiTransaction(byte port, ByteBuffer dataToSend,
      ByteBuffer dataReceived, byte size);

  public static native int spiWrite(byte port, ByteBuffer dataToSend, byte sendSize);

  public static native int spiRead(byte port, ByteBuffer dataReceived, byte size);

  public static native void spiClose(byte port);

  public static native void spiSetSpeed(byte port, int speed);

  public static native void spiSetOpts(byte port, int msb_first, int sample_on_trailing,
      int clk_idle_high);

  public static native void spiSetChipSelectActiveHigh(byte port);

  public static native void spiSetChipSelectActiveLow(byte port);
}
