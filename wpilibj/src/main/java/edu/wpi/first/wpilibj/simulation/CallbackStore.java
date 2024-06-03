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
   * Constructs an empty CallbackStore. This constructor is to allow 3rd party sim providers (eg
   * vendors) to subclass this class (without needing provide dummy constructing parameters) so that
   * the register methods of their sim classes can return CallbackStores like the builtin sims.
   * <b>Note: It should not be called by teams that are just using sims!</b>
   */
  protected CallbackStore() {
    this.m_cancelType = -1;
    this.m_index = -1;
    this.m_uid = -1;
    this.m_cancelCallback = null;
    this.m_cancelCallbackChannel = null;
  }

  /**
   * <b>Note: This constructor is for simulation classes only. It should not be called by teams!</b>
   *
   * @param index TODO
   * @param uid TODO
   * @param ccf TODO
   */
  public CallbackStore(int index, int uid, CancelCallbackFunc ccf) {
    this.m_cancelType = kNormalCancel;
    this.m_index = index;
    this.m_uid = uid;
    this.m_cancelCallback = ccf;
  }

  /**
   * <b>Note: This constructor is for simulation classes only. It should not be called by teams!</b>
   *
   * @param index TODO
   * @param channel TODO
   * @param uid TODO
   * @param ccf TODO
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
   *
   * @param uid TODO
   * @param ccf TODO
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
  private static final int kAlreadyCancelled = -1;
  private static final int kNormalCancel = 0;
  private static final int kChannelCancel = 1;
  private static final int kNoIndexCancel = 2;
  private int m_cancelType;

  /** Cancel the callback associated with this object. */
  @Override
  public void close() {
    switch (m_cancelType) {
      case kAlreadyCancelled -> {
        // Already cancelled so do nothing so that close() is idempotent.
        return;
      }
      case kNormalCancel -> m_cancelCallback.cancel(m_index, m_uid);
      case kChannelCancel -> m_cancelCallbackChannel.cancel(m_index, m_channel, m_uid);
      case kNoIndexCancel -> m_cancelCallbackNoIndex.cancel(m_uid);
      default -> {
        assert false;
      }
    }
    m_cancelType = kAlreadyCancelled;
  }
}
