/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.simulation;

import edu.wpi.first.hal.simulation.AddressableLEDDataJNI;
import edu.wpi.first.hal.simulation.ConstBufferCallback;
import edu.wpi.first.hal.simulation.NotifyCallback;
import edu.wpi.first.wpilibj.AddressableLED;
import java.util.NoSuchElementException;

/**
 * Class to control a simulated addressable LED.
 */
public class AddressableLEDSim {
  private final int m_index;

  /**
   * Constructs for the first addressable LED.
   */
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
   * Creates an AddressableLEDSim for a simulated index.
   * The index is incremented for each simulated AddressableLED.
   *
   * @param index simulator index
   * @return Simulated object
   */
  public static AddressableLEDSim createForIndex(int index) {
    return new AddressableLEDSim(index);
  }

  public CallbackStore registerInitializedCallback(NotifyCallback callback, boolean initialNotify) {
    int uid = AddressableLEDDataJNI.registerInitializedCallback(m_index, callback, initialNotify);
    return new CallbackStore(m_index, uid, AddressableLEDDataJNI::cancelInitializedCallback);
  }
  public boolean getInitialized() {
    return AddressableLEDDataJNI.getInitialized(m_index);
  }
  public void setInitialized(boolean initialized) {
    AddressableLEDDataJNI.setInitialized(m_index, initialized);
  }

  public CallbackStore registerOutputPortCallback(NotifyCallback callback, boolean initialNotify) {
    int uid = AddressableLEDDataJNI.registerOutputPortCallback(m_index, callback, initialNotify);
    return new CallbackStore(m_index, uid, AddressableLEDDataJNI::cancelOutputPortCallback);
  }
  public int getOutputPort() {
    return AddressableLEDDataJNI.getOutputPort(m_index);
  }
  public void setOutputPort(int outputPort) {
    AddressableLEDDataJNI.setOutputPort(m_index, outputPort);
  }

  public CallbackStore registerLengthCallback(NotifyCallback callback, boolean initialNotify) {
    int uid = AddressableLEDDataJNI.registerLengthCallback(m_index, callback, initialNotify);
    return new CallbackStore(m_index, uid, AddressableLEDDataJNI::cancelLengthCallback);
  }
  public int getLength() {
    return AddressableLEDDataJNI.getLength(m_index);
  }
  public void setLength(int length) {
    AddressableLEDDataJNI.setLength(m_index, length);
  }

  public CallbackStore registerRunningCallback(NotifyCallback callback, boolean initialNotify) {
    int uid = AddressableLEDDataJNI.registerRunningCallback(m_index, callback, initialNotify);
    return new CallbackStore(m_index, uid, AddressableLEDDataJNI::cancelRunningCallback);
  }
  public boolean getRunning() {
    return AddressableLEDDataJNI.getRunning(m_index);
  }
  public void setRunning(boolean running) {
    AddressableLEDDataJNI.setRunning(m_index, running);
  }

  public CallbackStore registerDataCallback(ConstBufferCallback callback) {
    int uid = AddressableLEDDataJNI.registerDataCallback(m_index, callback);
    return new CallbackStore(m_index, uid, AddressableLEDDataJNI::cancelDataCallback);
  }
  public byte[] getData() {
    return AddressableLEDDataJNI.getData(m_index);
  }
  public void setData(byte[] data) {
    AddressableLEDDataJNI.setData(m_index, data);
  }

  public void resetData() {
    AddressableLEDDataJNI.resetData(m_index);
  }
}
