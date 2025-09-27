// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.hardware.hal;

import java.nio.ByteBuffer;

/**
 * Addressable LED HAL JNI Methods.
 *
 * @see "hal/AddressableLED.h"
 */
public class AddressableLEDJNI extends JNIWrapper {
  public static final int COLOR_ORDER_RGB = 0;
  public static final int COLOR_ORDER_RBG = 1;
  public static final int COLOR_ORDER_BGR = 2;
  public static final int COLOR_ORDER_BRG = 3;
  public static final int COLOR_ORDER_GBR = 4;
  public static final int COLOR_ORDER_GRB = 5;

  /**
   * Create a new instance of an Addressable LED port.
   *
   * @param channel the smartio channel
   * @return Addressable LED handle
   * @see "HAL_InitializeAddressableLED"
   */
  public static native int initialize(int channel);

  /**
   * Free the Addressable LED Handle.
   *
   * @param handle the Addressable LED handle to free
   * @see "HAL_FreeAddressableLED"
   */
  public static native void free(int handle);

  /**
   * Sets the start buffer location used for the LED strip.
   *
   * <p>All addressable LEDs use a single backing buffer 1024 LEDs in size. The max length for a
   * single output is 1024 LEDs (with an offset of zero).
   *
   * @param handle the Addressable LED handle
   * @param start the strip start, in LEDs
   */
  public static native void setStart(int handle, int start);

  /**
   * Sets the length of the LED strip.
   *
   * <p>All addressable LEDs use a single backing buffer 1024 LEDs in size. The max length for a
   * single output is 1024 LEDs (with an offset of zero).
   *
   * @param handle the Addressable LED handle
   * @param length the strip length, in LEDs
   */
  public static native void setLength(int handle, int length);

  /**
   * Sets the led output data.
   *
   * <p>All addressable LEDs use a single backing buffer 1024 LEDs in size. This function may be
   * used to set part of or all of the buffer.
   *
   * @param outStart the strip start in the backing buffer, in LEDs
   * @param colorOrder the color order
   * @param data the buffer to write
   * @see "HAL_WriteAddressableLEDData"
   */
  public static void setData(int outStart, int colorOrder, byte[] data) {
    setData(outStart, colorOrder, data, 0, data.length);
  }

  /**
   * Sets the led output data.
   *
   * <p>All addressable LEDs use a single backing buffer 1024 LEDs in size. This function may be
   * used to set part of or all of the buffer.
   *
   * @param outStart the strip start in the backing buffer, in LEDs
   * @param colorOrder the color order
   * @param data the buffer to write
   * @param start offset into data, in bytes
   * @param len Length of data, in bytes
   */
  public static native void setData(int outStart, int colorOrder, byte[] data, int start, int len);

  /**
   * Sets the led output data.
   *
   * <p>All addressable LEDs use a single backing buffer 1024 LEDs in size. This function may be
   * used to set part of or all of the buffer.
   *
   * @param outStart the strip start in the backing buffer, in LEDs
   * @param colorOrder the color order
   * @param data the buffer to write
   */
  public static void setData(int outStart, int colorOrder, ByteBuffer data) {
    int pos = data.position();
    setData(outStart, colorOrder, data, pos, data.capacity() - pos);
  }

  /**
   * Sets the led output data.
   *
   * <p>All addressable LEDs use a single backing buffer 1024 LEDs in size. This function may be
   * used to set part of or all of the buffer.
   *
   * @param outStart the strip start in the backing buffer, in LEDs
   * @param colorOrder the color order
   * @param data the buffer to write
   * @param start offset into data, in bytes
   * @param len Length of data, in bytes
   */
  public static void setData(int outStart, int colorOrder, ByteBuffer data, int start, int len) {
    if (data.isDirect()) {
      if (start < 0) {
        throw new IndexOutOfBoundsException("start must be >= 0");
      }
      if (len < 0) {
        throw new IndexOutOfBoundsException("len must be >= 0");
      }
      if ((start + len) > data.capacity()) {
        throw new IndexOutOfBoundsException("start + len must be smaller than buffer capacity");
      }
      setDataFromBuffer(outStart, colorOrder, data, start, len);
    } else if (data.hasArray()) {
      setData(outStart, colorOrder, data.array(), data.arrayOffset() + start, len);
    } else {
      throw new UnsupportedOperationException("ByteBuffer must be direct or have a backing array");
    }
  }

  /**
   * Sets the led output data.
   *
   * <p>All addressable LEDs use a single backing buffer 1024 LEDs in size. This function may be
   * used to set part of or all of the buffer.
   *
   * @param outStart the strip start in the backing buffer, in LEDs
   * @param colorOrder the color order
   * @param data the buffer to write
   * @param start offset into data, in bytes
   * @param len Length of data, in bytes
   */
  private static native void setDataFromBuffer(
      int outStart, int colorOrder, ByteBuffer data, int start, int len);

  /** Utility class. */
  private AddressableLEDJNI() {}
}
