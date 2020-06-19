/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.simulation;

import edu.wpi.first.hal.simulation.DutyCycleDataJNI;
import edu.wpi.first.hal.simulation.NotifyCallback;
import edu.wpi.first.wpilibj.DutyCycle;
import java.util.NoSuchElementException;

/**
 * Class to control a simulated duty cycle digital input.
 */
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
   * Creates a DutyCycleSim for a simulated index.
   * The index is incremented for each simulated DutyCycle.
   *
   * @param index simulator index
   * @return Simulated object
   */
  public static DutyCycleSim createForIndex(int index) {
    return new DutyCycleSim(index);
  }

  public CallbackStore registerInitializedCallback(NotifyCallback callback, boolean initialNotify) {
    int uid = DutyCycleDataJNI.registerInitializedCallback(m_index, callback, initialNotify);
    return new CallbackStore(m_index, uid, DutyCycleDataJNI::cancelInitializedCallback);
  }
  public boolean getInitialized() {
    return DutyCycleDataJNI.getInitialized(m_index);
  }
  public void setInitialized(boolean initialized) {
    DutyCycleDataJNI.setInitialized(m_index, initialized);
  }

  public CallbackStore registerFrequencyCallback(NotifyCallback callback, boolean initialNotify) {
    int uid = DutyCycleDataJNI.registerFrequencyCallback(m_index, callback, initialNotify);
    return new CallbackStore(m_index, uid, DutyCycleDataJNI::cancelFrequencyCallback);
  }
  public int getFrequency() {
    return DutyCycleDataJNI.getFrequency(m_index);
  }
  public void setFrequency(int frequency) {
    DutyCycleDataJNI.setFrequency(m_index, frequency);
  }

  public CallbackStore registerOutputCallback(NotifyCallback callback, boolean initialNotify) {
    int uid = DutyCycleDataJNI.registerOutputCallback(m_index, callback, initialNotify);
    return new CallbackStore(m_index, uid, DutyCycleDataJNI::cancelOutputCallback);
  }
  public double getOutput() {
    return DutyCycleDataJNI.getOutput(m_index);
  }
  public void setOutput(double output) {
    DutyCycleDataJNI.setOutput(m_index, output);
  }
}
