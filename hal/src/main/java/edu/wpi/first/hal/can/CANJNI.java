// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.hal.can;

import edu.wpi.first.hal.JNIWrapper;
import java.nio.ByteBuffer;
import java.nio.IntBuffer;

@SuppressWarnings("MethodName")
public class CANJNI extends JNIWrapper {
  public static final int CAN_SEND_PERIOD_NO_REPEAT = 0;
  public static final int CAN_SEND_PERIOD_STOP_REPEATING = -1;

  /* Flags in the upper bits of the messageID */
  public static final int CAN_IS_FRAME_REMOTE = 0x80000000;
  public static final int CAN_IS_FRAME_11BIT = 0x40000000;

  public static native void FRCNetCommCANSessionMuxSendMessage(
      int messageID, byte[] data, int periodMs);

  public static native byte[] FRCNetCommCANSessionMuxReceiveMessage(
      IntBuffer messageID, int messageIDMask, ByteBuffer timeStamp);

  public static native void getCANStatus(CANStatus status);
}
