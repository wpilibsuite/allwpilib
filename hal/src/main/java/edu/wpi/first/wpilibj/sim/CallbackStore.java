/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.sim;

import java.io.Closeable;

public class CallbackStore implements Closeable {
  interface CancelCallbackFunc {
    void cancel(int index, int uid);
  }

  interface CancelCallbackChannelFunc {
    void cancel(int index, int channel, int uid);
  }

  interface CancelCallbackNoIndexFunc {
    void cancel(int uid);
  }

  public CallbackStore(int index, int uid, CancelCallbackFunc ccf) {
    this.cancelType = normalCancel;
    this.index = index;
    this.uid = uid;
    this.cancelCallback = ccf;
  }

  public CallbackStore(int index, int channel, int uid, CancelCallbackChannelFunc ccf) {
    this.cancelType = channelCancel;
    this.index = index;
    this.uid = uid;
    this.channel = channel;
    this.cancelCallbackChannel = ccf;
  }

  public CallbackStore(int uid, CancelCallbackNoIndexFunc ccf) {
    this.cancelType = noIndexCancel;
    this.uid = uid;
    this.cancelCallbackNoIndex = ccf;
  }

  private int index;
  private int channel;
  private int uid;
  private CancelCallbackFunc cancelCallback;
  private CancelCallbackChannelFunc cancelCallbackChannel;
  private CancelCallbackNoIndexFunc cancelCallbackNoIndex;
  private static final int normalCancel = 0;
  private static final int channelCancel = 1;
  private static final int noIndexCancel = 2;
  private int cancelType;

  @Override
  public void close() {
    switch (cancelType) {
      case normalCancel:
        cancelCallback.cancel(index, uid);
        break;
      case channelCancel:
        cancelCallbackChannel.cancel(index, channel, uid);
        break;
      case noIndexCancel:
        cancelCallbackNoIndex.cancel(uid);
        break;
    }
    cancelType = -1;
  }

  @Override
  protected void finalize() throws Throwable {
    try {
      if (cancelType >= 0) {
        close();        // close open files
      }
    } finally {
        super.finalize();
    }
  }
}
