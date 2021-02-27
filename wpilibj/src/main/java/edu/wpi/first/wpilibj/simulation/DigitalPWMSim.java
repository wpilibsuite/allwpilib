// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.simulation;

import edu.wpi.first.hal.simulation.DigitalPWMDataJNI;
import edu.wpi.first.hal.simulation.NotifyCallback;
import edu.wpi.first.wpilibj.DigitalOutput;
import java.util.NoSuchElementException;

/**
 * Class to control a simulated digital PWM output.
 *
 * <p>This is for duty cycle PWM outputs on a DigitalOutput, not for the servo style PWM outputs on
 * a PWM channel.
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
   * Creates an DigitalPWMSim for a simulated index. The index is incremented for each simulated
   * DigitalPWM.
   *
   * @param index simulator index
   * @return Simulated object
   */
  public static DigitalPWMSim createForIndex(int index) {
    return new DigitalPWMSim(index);
  }

  /**
   * Register a callback to be run when this PWM output is initialized.
   *
   * @param callback the callback
   * @param initialNotify whether to run the callback with the initial state
   * @return the {@link CallbackStore} object associated with this callback. Save a reference to
   *     this object so GC doesn't cancel the callback.
   */
  public CallbackStore registerInitializedCallback(NotifyCallback callback, boolean initialNotify) {
    int uid = DigitalPWMDataJNI.registerInitializedCallback(m_index, callback, initialNotify);
    return new CallbackStore(m_index, uid, DigitalPWMDataJNI::cancelInitializedCallback);
  }

  /**
   * Check whether this PWM output has been initialized.
   *
   * @return true if initialized
   */
  public boolean getInitialized() {
    return DigitalPWMDataJNI.getInitialized(m_index);
  }

  /**
   * Define whether this PWM output has been initialized.
   *
   * @param initialized whether this object is initialized
   */
  public void setInitialized(boolean initialized) {
    DigitalPWMDataJNI.setInitialized(m_index, initialized);
  }

  /**
   * Register a callback to be run whenever the duty cycle value changes.
   *
   * @param callback the callback
   * @param initialNotify whether to call the callback with the initial state
   * @return the {@link CallbackStore} object associated with this callback. Save a reference to
   *     this object so GC doesn't cancel the callback.
   */
  public CallbackStore registerDutyCycleCallback(NotifyCallback callback, boolean initialNotify) {
    int uid = DigitalPWMDataJNI.registerDutyCycleCallback(m_index, callback, initialNotify);
    return new CallbackStore(m_index, uid, DigitalPWMDataJNI::cancelDutyCycleCallback);
  }

  /**
   * Read the duty cycle value.
   *
   * @return the duty cycle value of this PWM output
   */
  public double getDutyCycle() {
    return DigitalPWMDataJNI.getDutyCycle(m_index);
  }

  /**
   * Set the duty cycle value of this PWM output.
   *
   * @param dutyCycle the new value
   */
  public void setDutyCycle(double dutyCycle) {
    DigitalPWMDataJNI.setDutyCycle(m_index, dutyCycle);
  }

  /**
   * Register a callback to be run whenever the pin changes.
   *
   * @param callback the callback
   * @param initialNotify whether to call the callback with the initial state
   * @return the {@link CallbackStore} object associated with this callback. Save a reference to
   *     this object so GC doesn't cancel the callback.
   */
  public CallbackStore registerPinCallback(NotifyCallback callback, boolean initialNotify) {
    int uid = DigitalPWMDataJNI.registerPinCallback(m_index, callback, initialNotify);
    return new CallbackStore(m_index, uid, DigitalPWMDataJNI::cancelPinCallback);
  }

  /**
   * Check the pin number.
   *
   * @return the pin number
   */
  public int getPin() {
    return DigitalPWMDataJNI.getPin(m_index);
  }

  /**
   * Change the pin number.
   *
   * @param pin the new pin number
   */
  public void setPin(int pin) {
    DigitalPWMDataJNI.setPin(m_index, pin);
  }

  /** Reset all simulation data. */
  public void resetData() {
    DigitalPWMDataJNI.resetData(m_index);
  }
}
