/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.simulation;

import edu.wpi.first.hal.simulation.DigitalPWMDataJNI;
import edu.wpi.first.hal.simulation.NotifyCallback;
import edu.wpi.first.wpilibj.DigitalOutput;
import java.util.NoSuchElementException;

/**
 * Class to control a simulated digital PWM output.
 *
 * <p>This is for duty cycle PWM outputs on a DigitalOutput, not for the servo
 * style PWM outputs on a PWM channel.
 */
public class DigitalPWMSim {
  private final int m_index;

  /**
   * Constructs from a DigitalOutput object.
   *
   * @param digitalOutput DigitalOutput to simulate
   */
  public DigitalPWMSim(DigitalOutput digitalOutput) {
    m_index = digitalOutput.getChannel();
  }

  private DigitalPWMSim(int index) {
    m_index = index;
  }

  /**
   * Creates an DigitalPWMSim for a digital I/O channel.
   *
   * @param channel DIO channel
   * @return Simulated object
   * @throws NoSuchElementException if no Digital PWM is configured for that channel
   */
  public static DigitalPWMSim createForChannel(int channel) {
    int index = DigitalPWMDataJNI.findForChannel(channel);
    if (index < 0) {
      throw new NoSuchElementException("no digital PWM found for channel " + channel);
    }
    return new DigitalPWMSim(index);
  }

  /**
   * Creates an DigitalPWMSim for a simulated index.
   * The index is incremented for each simulated DigitalPWM.
   *
   * @param index simulator index
   * @return Simulated object
   */
  public static DigitalPWMSim createForIndex(int index) {
    return new DigitalPWMSim(index);
  }

  public CallbackStore registerInitializedCallback(NotifyCallback callback, boolean initialNotify) {
    int uid = DigitalPWMDataJNI.registerInitializedCallback(m_index, callback, initialNotify);
    return new CallbackStore(m_index, uid, DigitalPWMDataJNI::cancelInitializedCallback);
  }
  public boolean getInitialized() {
    return DigitalPWMDataJNI.getInitialized(m_index);
  }
  public void setInitialized(boolean initialized) {
    DigitalPWMDataJNI.setInitialized(m_index, initialized);
  }

  public CallbackStore registerDutyCycleCallback(NotifyCallback callback, boolean initialNotify) {
    int uid = DigitalPWMDataJNI.registerDutyCycleCallback(m_index, callback, initialNotify);
    return new CallbackStore(m_index, uid, DigitalPWMDataJNI::cancelDutyCycleCallback);
  }
  public double getDutyCycle() {
    return DigitalPWMDataJNI.getDutyCycle(m_index);
  }
  public void setDutyCycle(double dutyCycle) {
    DigitalPWMDataJNI.setDutyCycle(m_index, dutyCycle);
  }

  public CallbackStore registerPinCallback(NotifyCallback callback, boolean initialNotify) {
    int uid = DigitalPWMDataJNI.registerPinCallback(m_index, callback, initialNotify);
    return new CallbackStore(m_index, uid, DigitalPWMDataJNI::cancelPinCallback);
  }
  public int getPin() {
    return DigitalPWMDataJNI.getPin(m_index);
  }
  public void setPin(int pin) {
    DigitalPWMDataJNI.setPin(m_index, pin);
  }

  public void resetData() {
    DigitalPWMDataJNI.resetData(m_index);
  }
}
