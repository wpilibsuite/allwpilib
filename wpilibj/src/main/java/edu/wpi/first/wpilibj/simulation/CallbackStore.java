// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.simulation;

/** Manages simulation callbacks; each object is associated with a callback. */
public class CallbackStore implements AutoCloseable {
  /** <b>Note: This interface is for simulation classes only. It should not be used by teams!</b> */
  interface CancelCallbackFunc {
    void cancel(int index, int uid);
  }

  /** <b>Note: This interface is for simulation classes only. It should not be used by teams!</b> */
  interface CancelCallbackChannelFunc {
    void cancel(int index, int channel, int uid);
  }

  /** <b>Note: This interface is for simulation classes only. It should not be used by teams!</b> */
  interface CancelCallbackNoIndexFunc {
    void cancel(int uid);
  }

  /**
   * <b>Note: This constructor is for simulation classes only. It should not be called by teams!</b>
   */
  public CallbackStore(int index, int uid, CancelCallbackFunc ccf) {
    this.m_cancelType = kNormalCancel;
    this.m_index = index;
    this.m_uid = uid;
    this.m_cancelCallback = ccf;
  }

  /**
   * <b>Note: This constructor is for simulation classes only. It should not be called by teams!</b>
   */
  public CallbackStore(int index, int channel, int uid, CancelCallbackChannelFunc ccf) {
    this.m_cancelType = kChannelCancel;
    this.m_index = index;
    this.m_uid = uid;
    this.m_channel = channel;
    this.m_cancelCallbackChannel = ccf;
  }

  /**
   * <b>Note: This constructor is for simulation classes only. It should not be called by teams!</b>
   */
  public CallbackStore(int uid, CancelCallbackNoIndexFunc ccf) {
    this.m_cancelType = kNoIndexCancel;
    this.m_uid = uid;
    this.m_cancelCallbackNoIndex = ccf;
  }

  private int m_index;
  private int m_channel;
  private final int m_uid;
  private CancelCallbackFunc m_cancelCallback;
  private CancelCallbackChannelFunc m_cancelCallbackChannel;
  private CancelCallbackNoIndexFunc m_cancelCallbackNoIndex;
  private static final int kNormalCancel = 0;
  private static final int kChannelCancel = 1;
  private static final int kNoIndexCancel = 2;
  private int m_cancelType;

  /** Cancel the callback associated with this object. */
  @Override
  public void close() {
    switch (m_cancelType) {
      case kNormalCancel:
        m_cancelCallback.cancel(m_index, m_uid);
        break;
      case kChannelCancel:
        m_cancelCallbackChannel.cancel(m_index, m_channel, m_uid);
        break;
      case kNoIndexCancel:
        m_cancelCallbackNoIndex.cancel(m_uid);
        break;
      default:
        assert false;
        break;
    }
    m_cancelType = -1;
  }

  @SuppressWarnings("NoFinalizer")
  @Override
  protected void finalize() throws Throwable {
    try {
      if (m_cancelType >= 0) {
        close(); // close open files
      }
    } finally {
      super.finalize();
    }
  }
}
