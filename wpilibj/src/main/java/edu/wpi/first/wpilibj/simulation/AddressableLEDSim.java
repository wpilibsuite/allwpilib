// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.simulation;

import edu.wpi.first.hal.simulation.AddressableLEDDataJNI;
import edu.wpi.first.hal.simulation.ConstBufferCallback;
import edu.wpi.first.hal.simulation.NotifyCallback;
import edu.wpi.first.wpilibj.AddressableLED;

/** Class to control a simulated addressable LED. */
public class AddressableLEDSim {
  private final int m_channel;

  /**
   * Constructs an addressable LED for a specific channel.
   *
   * @param channel output channel
   */
  public AddressableLEDSim(int channel) {
    m_channel = channel;
  }

  /**
   * Constructs from an AddressableLED object.
   *
   * @param addressableLED AddressableLED to simulate
   */
  @SuppressWarnings("PMD.UnusedFormalParameter")
  public AddressableLEDSim(AddressableLED addressableLED) {
    m_channel = addressableLED.getChannel();
  }

  /**
   * Register a callback on the Initialized property.
   *
   * @param callback the callback that will be called whenever the Initialized property is changed
   * @param initialNotify if true, the callback will be run on the initial value
   * @return the {@link CallbackStore} object associated with this callback.
   */
  public CallbackStore registerInitializedCallback(NotifyCallback callback, boolean initialNotify) {
    int uid = AddressableLEDDataJNI.registerInitializedCallback(m_channel, callback, initialNotify);
    return new CallbackStore(m_channel, uid, AddressableLEDDataJNI::cancelInitializedCallback);
  }

  /**
   * Check if initialized.
   *
   * @return true if initialized
   */
  public boolean getInitialized() {
    return AddressableLEDDataJNI.getInitialized(m_channel);
  }

  /**
   * Change the Initialized value of the LED strip.
   *
   * @param initialized the new value
   */
  public void setInitialized(boolean initialized) {
    AddressableLEDDataJNI.setInitialized(m_channel, initialized);
  }

  /**
   * Register a callback on the start.
   *
   * @param callback the callback that will be called whenever the start is changed
   * @param initialNotify if true, the callback will be run on the initial value
   * @return the {@link CallbackStore} object associated with this callback.
   */
  public CallbackStore registerStartCallback(NotifyCallback callback, boolean initialNotify) {
    int uid = AddressableLEDDataJNI.registerStartCallback(m_channel, callback, initialNotify);
    return new CallbackStore(m_channel, uid, AddressableLEDDataJNI::cancelStartCallback);
  }

  /**
   * Get the start.
   *
   * @return the start
   */
  public int getStart() {
    return AddressableLEDDataJNI.getStart(m_channel);
  }

  /**
   * Change the start.
   *
   * @param start the new start
   */
  public void setStart(int start) {
    AddressableLEDDataJNI.setStart(m_channel, start);
  }

  /**
   * Register a callback on the length.
   *
   * @param callback the callback that will be called whenever the length is changed
   * @param initialNotify if true, the callback will be run on the initial value
   * @return the {@link CallbackStore} object associated with this callback.
   */
  public CallbackStore registerLengthCallback(NotifyCallback callback, boolean initialNotify) {
    int uid = AddressableLEDDataJNI.registerLengthCallback(m_channel, callback, initialNotify);
    return new CallbackStore(m_channel, uid, AddressableLEDDataJNI::cancelLengthCallback);
  }

  /**
   * Get the length of the LED strip.
   *
   * @return the length
   */
  public int getLength() {
    return AddressableLEDDataJNI.getLength(m_channel);
  }

  /**
   * Change the length of the LED strip.
   *
   * @param length the new value
   */
  public void setLength(int length) {
    AddressableLEDDataJNI.setLength(m_channel, length);
  }

  /**
   * Register a callback on the LED data.
   *
   * @param callback the callback that will be called whenever the LED data is changed
   * @return the {@link CallbackStore} object associated with this callback.
   */
  public CallbackStore registerDataCallback(ConstBufferCallback callback) {
    int uid = AddressableLEDDataJNI.registerDataCallback(callback);
    return new CallbackStore(uid, AddressableLEDDataJNI::cancelDataCallback);
  }

  /**
   * Get the LED data.
   *
   * @return the LED data
   */
  public byte[] getData() {
    return getGlobalData(getStart(), getLength());
  }

  /**
   * Change the LED data.
   *
   * @param data the new data
   */
  public void setData(byte[] data) {
    setGlobalData(getStart(), data);
  }

  /**
   * Get the global LED data.
   *
   * @param start start, in LEDs
   * @param length length, in LEDs
   * @return the LED data
   */
  public static byte[] getGlobalData(int start, int length) {
    return AddressableLEDDataJNI.getData(start, length);
  }

  /**
   * Change the global LED data.
   *
   * @param start start, in LEDs
   * @param data the new data
   */
  public static void setGlobalData(int start, byte[] data) {
    AddressableLEDDataJNI.setData(start, data);
  }

  /** Reset all simulation data for this LED object. */
  public void resetData() {
    AddressableLEDDataJNI.resetData(m_channel);
  }
}
