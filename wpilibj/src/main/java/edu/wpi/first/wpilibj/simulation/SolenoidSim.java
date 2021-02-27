// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.simulation;

import edu.wpi.first.hal.simulation.NotifyCallback;
import edu.wpi.first.wpilibj.Solenoid;

/** Class to control a simulated {@link edu.wpi.first.wpilibj.Solenoid}. */
public class SolenoidSim {
  private final PCMSim m_pcm;
  private final int m_channel;

  /**
   * Constructs for a solenoid on the default PCM.
   *
   * @param channel the solenoid channel.
   * @see PCMSim#PCMSim()
   */
  public SolenoidSim(int channel) {
    this.m_pcm = new PCMSim();
    this.m_channel = channel;
  }

  /**
   * Constructs for a solenoid.
   *
   * @param module the CAN ID of the PCM the solenoid is connected to.
   * @param channel the solenoid channel.
   * @see PCMSim#PCMSim(int)
   */
  public SolenoidSim(int module, int channel) {
    this(new PCMSim(module), channel);
  }

  /**
   * Constructs for a solenoid on the given PCM.
   *
   * @param pcm the PCM the solenoid is connected to.
   * @param channel the solenoid channel.
   */
  public SolenoidSim(PCMSim pcm, int channel) {
    this.m_pcm = pcm;
    this.m_channel = channel;
  }

  /**
   * Constructs for the given solenoid.
   *
   * @param solenoid the solenoid to simulate.
   */
  public SolenoidSim(Solenoid solenoid) {
    this(solenoid.getModuleNumber(), solenoid.getChannel());
  }

  /**
   * Register a callback to be run when this solenoid is initialized.
   *
   * @param callback the callback
   * @param initialNotify should the callback be run with the initial state
   * @return the {@link CallbackStore} object associated with this callback. Save a reference to
   *     this object so GC doesn't cancel the callback.
   */
  public CallbackStore registerInitializedCallback(NotifyCallback callback, boolean initialNotify) {
    return m_pcm.registerSolenoidInitializedCallback(m_channel, callback, initialNotify);
  }

  /**
   * Check if this solenoid has been initialized.
   *
   * @return true if initialized
   */
  public boolean getInitialized() {
    return m_pcm.getSolenoidInitialized(m_channel);
  }

  /**
   * Define whether this solenoid has been initialized.
   *
   * @param initialized whether the solenoid is intiialized.
   */
  public void setInitialized(boolean initialized) {
    m_pcm.setSolenoidInitialized(m_channel, initialized);
  }

  /**
   * Register a callback to be run when the output of this solenoid has changed.
   *
   * @param callback the callback
   * @param initialNotify should the callback be run with the initial value
   * @return the {@link CallbackStore} object associated with this callback. Save a reference to
   *     this object so GC doesn't cancel the callback.
   */
  public CallbackStore registerOutputCallback(NotifyCallback callback, boolean initialNotify) {
    return m_pcm.registerSolenoidOutputCallback(m_channel, callback, initialNotify);
  }

  /**
   * Check the solenoid output.
   *
   * @return the solenoid output
   */
  public boolean getOutput() {
    return m_pcm.getSolenoidOutput(m_channel);
  }

  /**
   * Change the solenoid output.
   *
   * @param output the new solenoid output
   */
  public void setOutput(boolean output) {
    m_pcm.setSolenoidOutput(m_channel, output);
  }

  /**
   * Get the wrapped {@link PCMSim} object.
   *
   * @return the wrapped {@link PCMSim} object.
   */
  public PCMSim getPCMSim() {
    return m_pcm;
  }
}
