// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.simulation;

import edu.wpi.first.hal.simulation.NotifyCallback;
import edu.wpi.first.wpilibj.PneumaticsBase;
import edu.wpi.first.wpilibj.PneumaticsModuleType;

/** Class to control a simulated {@link edu.wpi.first.wpilibj.Solenoid}. */
public class SolenoidSim {
  private final PneumaticsBaseSim m_module;
  private final int m_channel;

  /**
   * Constructs for a solenoid on the given pneumatics module.
   *
   * @param moduleSim the PCM the solenoid is connected to.
   * @param channel the solenoid channel.
   */
  public SolenoidSim(PneumaticsBaseSim moduleSim, int channel) {
    m_module = moduleSim;
    m_channel = channel;
  }

  /**
   * Constructs for a solenoid on a pneumatics module of the given type and ID.
   *
   * @param module the CAN ID of the pneumatics module the solenoid is connected to.
   * @param moduleType the module type (PH or PCM)
   * @param channel the solenoid channel.
   */
  public SolenoidSim(int module, PneumaticsModuleType moduleType, int channel) {
    this(PneumaticsBaseSim.getForType(module, moduleType), channel);
  }

  /**
   * Constructs for a solenoid on a pneumatics module of the given type and default ID.
   *
   * @param moduleType the module type (PH or PCM)
   * @param channel the solenoid channel.
   */
  public SolenoidSim(PneumaticsModuleType moduleType, int channel) {
    this(PneumaticsBase.getDefaultForType(moduleType), moduleType, channel);
  }

  /**
   * Check the solenoid output.
   *
   * @return the solenoid output
   */
  public boolean getOutput() {
    return m_module.getSolenoidOutput(m_channel);
  }

  /**
   * Change the solenoid output.
   *
   * @param output the new solenoid output
   */
  public void setOutput(boolean output) {
    m_module.setSolenoidOutput(m_channel, output);
  }

  /**
   * Register a callback to be run when the output of this solenoid has changed.
   *
   * @param callback the callback
   * @param initialNotify should the callback be run with the initial value
   * @return the {@link CallbackStore} object associated with this callback.
   */
  public CallbackStore registerOutputCallback(NotifyCallback callback, boolean initialNotify) {
    return m_module.registerSolenoidOutputCallback(m_channel, callback, initialNotify);
  }

  /**
   * Get the wrapped {@link PneumaticsBaseSim} object.
   *
   * @return the wrapped {@link PneumaticsBaseSim} object.
   */
  public PneumaticsBaseSim getPCMSim() {
    return m_module;
  }
}
