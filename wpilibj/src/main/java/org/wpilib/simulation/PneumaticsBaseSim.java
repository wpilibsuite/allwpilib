// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.simulation;

import edu.wpi.first.hal.simulation.NotifyCallback;
import edu.wpi.first.wpilibj.PneumaticsBase;
import edu.wpi.first.wpilibj.PneumaticsModuleType;

/** Common base class for pneumatics module simulation classes. */
public abstract class PneumaticsBaseSim {
  /** PneumaticsBase index. */
  protected final int m_index;

  /**
   * Get a module sim for a specific type.
   *
   * @param module the module number / CAN ID.
   * @param type the module type.
   * @return the module object.
   */
  public static PneumaticsBaseSim getForType(int module, PneumaticsModuleType type) {
    return switch (type) {
      case CTREPCM -> new CTREPCMSim(module);
      case REVPH -> new REVPHSim(module);
    };
  }

  /**
   * Constructs a PneumaticsBaseSim with the given index.
   *
   * @param index The index.
   */
  protected PneumaticsBaseSim(int index) {
    m_index = index;
  }

  /**
   * Constructs a PneumaticsBaseSim for the given module.
   *
   * @param module The module.
   */
  protected PneumaticsBaseSim(PneumaticsBase module) {
    this(module.getModuleNumber());
  }

  /**
   * Check whether the PCM/PH has been initialized.
   *
   * @return true if initialized
   */
  public abstract boolean getInitialized();

  /**
   * Define whether the PCM/PH has been initialized.
   *
   * @param initialized true for initialized
   */
  public abstract void setInitialized(boolean initialized);

  /**
   * Register a callback to be run when the PCM/PH is initialized.
   *
   * @param callback the callback
   * @param initialNotify whether to run the callback with the initial state
   * @return the {@link CallbackStore} object associated with this callback.
   */
  public abstract CallbackStore registerInitializedCallback(
      NotifyCallback callback, boolean initialNotify);

  /**
   * Check if the compressor is on.
   *
   * @return true if the compressor is active
   */
  public abstract boolean getCompressorOn();

  /**
   * Set whether the compressor is active.
   *
   * @param compressorOn the new value
   */
  public abstract void setCompressorOn(boolean compressorOn);

  /**
   * Register a callback to be run when the compressor activates.
   *
   * @param callback the callback
   * @param initialNotify whether to run the callback with the initial state
   * @return the {@link CallbackStore} object associated with this callback.
   */
  public abstract CallbackStore registerCompressorOnCallback(
      NotifyCallback callback, boolean initialNotify);

  /**
   * Check the solenoid output on a specific channel.
   *
   * @param channel the channel to check
   * @return the solenoid output
   */
  public abstract boolean getSolenoidOutput(int channel);

  /**
   * Change the solenoid output on a specific channel.
   *
   * @param channel the channel to check
   * @param solenoidOutput the new solenoid output
   */
  public abstract void setSolenoidOutput(int channel, boolean solenoidOutput);

  /**
   * Register a callback to be run when the solenoid output on a channel changes.
   *
   * @param channel the channel to monitor
   * @param callback the callback
   * @param initialNotify should the callback be run with the initial value
   * @return the {@link CallbackStore} object associated with this callback.
   */
  public abstract CallbackStore registerSolenoidOutputCallback(
      int channel, NotifyCallback callback, boolean initialNotify);

  /**
   * Check the value of the pressure switch.
   *
   * @return the pressure switch value
   */
  public abstract boolean getPressureSwitch();

  /**
   * Set the value of the pressure switch.
   *
   * @param pressureSwitch the new value
   */
  public abstract void setPressureSwitch(boolean pressureSwitch);

  /**
   * Register a callback to be run whenever the pressure switch value changes.
   *
   * @param callback the callback
   * @param initialNotify whether the callback should be called with the initial value
   * @return the {@link CallbackStore} object associated with this callback.
   */
  public abstract CallbackStore registerPressureSwitchCallback(
      NotifyCallback callback, boolean initialNotify);

  /**
   * Read the compressor current.
   *
   * @return the current of the compressor connected to this module
   */
  public abstract double getCompressorCurrent();

  /**
   * Set the compressor current.
   *
   * @param compressorCurrent the new compressor current
   */
  public abstract void setCompressorCurrent(double compressorCurrent);

  /**
   * Register a callback to be run whenever the compressor current changes.
   *
   * @param callback the callback
   * @param initialNotify whether to call the callback with the initial state
   * @return the {@link CallbackStore} object associated with this callback.
   */
  public abstract CallbackStore registerCompressorCurrentCallback(
      NotifyCallback callback, boolean initialNotify);

  /** Reset all simulation data for this object. */
  public abstract void resetData();
}
