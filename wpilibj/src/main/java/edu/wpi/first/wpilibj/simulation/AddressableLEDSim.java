// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.simulation;

import edu.wpi.first.hal.simulation.AddressableLEDDataJNI;
import edu.wpi.first.hal.simulation.ConstBufferCallback;
import edu.wpi.first.hal.simulation.NotifyCallback;
import edu.wpi.first.wpilibj.AddressableLED;
import java.util.NoSuchElementException;

/** Class to control a simulated addressable LED. */
public class AddressableLEDSim {
  private final int m_index;

  /** Constructs for the first addressable LED. */
  public AddressableLEDSim() {
    m_index = 0;
  }

  /**
   * Constructs from an AddressableLED object.
   *
   * @param addressableLED AddressableLED to simulate
   */
  @SuppressWarnings("PMD.UnusedFormalParameter")
  public AddressableLEDSim(AddressableLED addressableLED) {
    // there is only support for a single AddressableLED, so no lookup
    m_index = 0;
  }

  private AddressableLEDSim(int index) {
    m_index = index;
  }

  /**
   * Creates an AddressableLEDSim for a PWM channel.
   *
   * @param pwmChannel PWM channel
   * @return Simulated object
   * @throws NoSuchElementException if no AddressableLED is configured for that channel
   */
  public static AddressableLEDSim createForChannel(int pwmChannel) {
    int index = AddressableLEDDataJNI.findForChannel(pwmChannel);
    if (index < 0) {
      throw new NoSuchElementException("no addressable LED found for PWM channel " + pwmChannel);
    }
    return new AddressableLEDSim(index);
  }

  /**
   * Creates an AddressableLEDSim for a simulated index. The index is incremented for each simulated
   * AddressableLED.
   *
   * @param index simulator index
   * @return Simulated object
   */
  public static AddressableLEDSim createForIndex(int index) {
    return new AddressableLEDSim(index);
  }

  /**
   * Register a callback on the Initialized property.
   *
   * @param callback the callback that will be called whenever the Initialized property is changed
   * @param initialNotify if true, the callback will be run on the initial value
   * @return the {@link CallbackStore} object associated with this callback. Save a reference to
   *     this object so GC doesn't cancel the callback.
   */
  public CallbackStore registerInitializedCallback(NotifyCallback callback, boolean initialNotify) {
    int uid = AddressableLEDDataJNI.registerInitializedCallback(m_index, callback, initialNotify);
    return new CallbackStore(m_index, uid, AddressableLEDDataJNI::cancelInitializedCallback);
  }

  /**
   * Check if initialized.
   *
   * @return true if initialized
   */
  public boolean getInitialized() {
    return AddressableLEDDataJNI.getInitialized(m_index);
  }

  /**
   * Change the Initialized value of the LED strip.
   *
   * @param initialized the new value
   */
  public void setInitialized(boolean initialized) {
    AddressableLEDDataJNI.setInitialized(m_index, initialized);
  }

  /**
   * Register a callback on the output port.
   *
   * @param callback the callback that will be called whenever the output port is changed
   * @param initialNotify if true, the callback will be run on the initial value
   * @return the {@link CallbackStore} object associated with this callback. Save a reference to
   *     this object so GC doesn't cancel the callback.
   */
  public CallbackStore registerOutputPortCallback(NotifyCallback callback, boolean initialNotify) {
    int uid = AddressableLEDDataJNI.registerOutputPortCallback(m_index, callback, initialNotify);
    return new CallbackStore(m_index, uid, AddressableLEDDataJNI::cancelOutputPortCallback);
  }

  /**
   * Get the output port.
   *
   * @return the output port
   */
  public int getOutputPort() {
    return AddressableLEDDataJNI.getOutputPort(m_index);
  }

  /**
   * Change the output port.
   *
   * @param outputPort the new output port
   */
  public void setOutputPort(int outputPort) {
    AddressableLEDDataJNI.setOutputPort(m_index, outputPort);
  }

  /**
   * Register a callback on the length.
   *
   * @param callback the callback that will be called whenever the length is changed
   * @param initialNotify if true, the callback will be run on the initial value
   * @return the {@link CallbackStore} object associated with this callback. Save a reference to
   *     this object so GC doesn't cancel the callback.
   */
  public CallbackStore registerLengthCallback(NotifyCallback callback, boolean initialNotify) {
    int uid = AddressableLEDDataJNI.registerLengthCallback(m_index, callback, initialNotify);
    return new CallbackStore(m_index, uid, AddressableLEDDataJNI::cancelLengthCallback);
  }

  /**
   * Get the length of the LED strip.
   *
   * @return the length
   */
  public int getLength() {
    return AddressableLEDDataJNI.getLength(m_index);
  }

  /**
   * Change the length of the LED strip.
   *
   * @param length the new value
   */
  public void setLength(int length) {
    AddressableLEDDataJNI.setLength(m_index, length);
  }

  /**
   * Register a callback on whether the LEDs are running.
   *
   * @param callback the callback that will be called whenever the LED state is changed
   * @param initialNotify if true, the callback will be run on the initial value
   * @return the {@link CallbackStore} object associated with this callback. Save a reference to
   *     this object so GC doesn't cancel the callback.
   */
  public CallbackStore registerRunningCallback(NotifyCallback callback, boolean initialNotify) {
    int uid = AddressableLEDDataJNI.registerRunningCallback(m_index, callback, initialNotify);
    return new CallbackStore(m_index, uid, AddressableLEDDataJNI::cancelRunningCallback);
  }

  /**
   * Check if the LEDs are running.
   *
   * @return true if they are
   */
  public boolean getRunning() {
    return AddressableLEDDataJNI.getRunning(m_index);
  }

  /**
   * Change whether the LEDs are active.
   *
   * @param running the new value
   */
  public void setRunning(boolean running) {
    AddressableLEDDataJNI.setRunning(m_index, running);
  }

  /**
   * Register a callback on the LED data.
   *
   * @param callback the callback that will be called whenever the LED data is changed
   * @return the {@link CallbackStore} object associated with this callback. Save a reference to
   *     this object so GC doesn't cancel the callback.
   */
  public CallbackStore registerDataCallback(ConstBufferCallback callback) {
    int uid = AddressableLEDDataJNI.registerDataCallback(m_index, callback);
    return new CallbackStore(m_index, uid, AddressableLEDDataJNI::cancelDataCallback);
  }

  /**
   * Get the LED data.
   *
   * @return the LED data
   */
  public byte[] getData() {
    return AddressableLEDDataJNI.getData(m_index);
  }

  /**
   * Change the LED data.
   *
   * @param data the new data
   */
  public void setData(byte[] data) {
    AddressableLEDDataJNI.setData(m_index, data);
  }

  /** Reset all simulation data for this LED object. */
  public void resetData() {
    AddressableLEDDataJNI.resetData(m_index);
  }
}
