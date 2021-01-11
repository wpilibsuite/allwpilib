// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.simulation;

import edu.wpi.first.hal.simulation.DutyCycleDataJNI;
import edu.wpi.first.hal.simulation.NotifyCallback;
import edu.wpi.first.wpilibj.DutyCycle;
import java.util.NoSuchElementException;

/** Class to control a simulated duty cycle digital input. */
public class DutyCycleSim {
  private final int m_index;

  /**
   * Constructs from a DutyCycle object.
   *
   * @param dutyCycle DutyCycle to simulate
   */
  public DutyCycleSim(DutyCycle dutyCycle) {
    m_index = dutyCycle.getFPGAIndex();
  }

  private DutyCycleSim(int index) {
    m_index = index;
  }

  /**
   * Creates a DutyCycleSim for a digital input channel.
   *
   * @param channel digital input channel
   * @return Simulated object
   * @throws NoSuchElementException if no DutyCycle is configured for that channel
   */
  public static DutyCycleSim createForChannel(int channel) {
    int index = DutyCycleDataJNI.findForChannel(channel);
    if (index < 0) {
      throw new NoSuchElementException("no duty cycle found for channel " + channel);
    }
    return new DutyCycleSim(index);
  }

  /**
   * Creates a DutyCycleSim for a simulated index. The index is incremented for each simulated
   * DutyCycle.
   *
   * @param index simulator index
   * @return Simulated object
   */
  public static DutyCycleSim createForIndex(int index) {
    return new DutyCycleSim(index);
  }

  /**
   * Register a callback to be run when this duty cycle input is initialized.
   *
   * @param callback the callback
   * @param initialNotify whether to run the callback with the initial state
   * @return the {@link CallbackStore} object associated with this callback. Save a reference to
   *     this object so GC doesn't cancel the callback.
   */
  public CallbackStore registerInitializedCallback(NotifyCallback callback, boolean initialNotify) {
    int uid = DutyCycleDataJNI.registerInitializedCallback(m_index, callback, initialNotify);
    return new CallbackStore(m_index, uid, DutyCycleDataJNI::cancelInitializedCallback);
  }

  /**
   * Check whether this duty cycle input has been initialized.
   *
   * @return true if initialized
   */
  public boolean getInitialized() {
    return DutyCycleDataJNI.getInitialized(m_index);
  }

  /**
   * Define whether this duty cycle input has been initialized.
   *
   * @param initialized whether this object is initialized
   */
  public void setInitialized(boolean initialized) {
    DutyCycleDataJNI.setInitialized(m_index, initialized);
  }

  /**
   * Register a callback to be run whenever the frequency changes.
   *
   * @param callback the callback
   * @param initialNotify whether to call the callback with the initial state
   * @return the {@link CallbackStore} object associated with this callback. Save a reference to
   *     this object so GC doesn't cancel the callback.
   */
  public CallbackStore registerFrequencyCallback(NotifyCallback callback, boolean initialNotify) {
    int uid = DutyCycleDataJNI.registerFrequencyCallback(m_index, callback, initialNotify);
    return new CallbackStore(m_index, uid, DutyCycleDataJNI::cancelFrequencyCallback);
  }

  /**
   * Measure the frequency.
   *
   * @return the duty cycle frequency
   */
  public int getFrequency() {
    return DutyCycleDataJNI.getFrequency(m_index);
  }

  /**
   * Change the duty cycle frequency.
   *
   * @param frequency the new frequency
   */
  public void setFrequency(int frequency) {
    DutyCycleDataJNI.setFrequency(m_index, frequency);
  }

  /**
   * Register a callback to be run whenever the output changes.
   *
   * @param callback the callback
   * @param initialNotify whether to call the callback with the initial state
   * @return the {@link CallbackStore} object associated with this callback. Save a reference to
   *     this object so GC doesn't cancel the callback.
   */
  public CallbackStore registerOutputCallback(NotifyCallback callback, boolean initialNotify) {
    int uid = DutyCycleDataJNI.registerOutputCallback(m_index, callback, initialNotify);
    return new CallbackStore(m_index, uid, DutyCycleDataJNI::cancelOutputCallback);
  }

  /**
   * Measure the output from this duty cycle port.
   *
   * @return the output value
   */
  public double getOutput() {
    return DutyCycleDataJNI.getOutput(m_index);
  }

  /**
   * Change the duty cycle output.
   *
   * @param output the new output value
   */
  public void setOutput(double output) {
    DutyCycleDataJNI.setOutput(m_index, output);
  }

  /** Reset all simulation data for the duty cycle output. */
  public void resetData() {
    DutyCycleDataJNI.resetData(m_index);
  }
}
