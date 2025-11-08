// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <memory>

#include "frc/PneumaticsBase.h"
#include "frc/PneumaticsModuleType.h"
#include "frc/simulation/CallbackStore.h"

namespace frc::sim {

class PneumaticsBaseSim {
 public:
  virtual ~PneumaticsBaseSim() = default;

  static std::shared_ptr<PneumaticsBaseSim> GetForType(
      int module, PneumaticsModuleType type);

  /**
   * Check whether the PCM/PH has been initialized.
   *
   * @return true if initialized
   */
  virtual bool GetInitialized() const = 0;

  /**
   * Define whether the PCM/PH has been initialized.
   *
   * @param initialized true for initialized
   */
  virtual void SetInitialized(bool initialized) = 0;

  /**
   * Register a callback to be run when the PCM/PH is initialized.
   *
   * @param callback the callback
   * @param initialNotify whether to run the callback with the initial state
   * @return the {@link CallbackStore} object associated with this callback.
   * Save a reference to this object; it being deconstructed cancels the
   * callback.
   */
  [[nodiscard]]
  virtual std::unique_ptr<CallbackStore> RegisterInitializedCallback(
      NotifyCallback callback, bool initialNotify) = 0;

  /**
   * Check if the compressor is on.
   *
   * @return true if the compressor is active
   */
  virtual bool GetCompressorOn() const = 0;

  /**
   * Set whether the compressor is active.
   *
   * @param compressorOn the new value
   */
  virtual void SetCompressorOn(bool compressorOn) = 0;

  /**
   * Register a callback to be run when the compressor activates.
   *
   * @param callback the callback
   * @param initialNotify whether to run the callback with the initial state
   * @return the {@link CallbackStore} object associated with this callback.
   * Save a reference to this object; it being deconstructed cancels the
   * callback.
   */
  [[nodiscard]]
  virtual std::unique_ptr<CallbackStore> RegisterCompressorOnCallback(
      NotifyCallback callback, bool initialNotify) = 0;

  /**
   * Check the solenoid output on a specific channel.
   *
   * @param channel the channel to check
   * @return the solenoid output
   */
  virtual bool GetSolenoidOutput(int channel) const = 0;

  /**
   * Change the solenoid output on a specific channel.
   *
   * @param channel the channel to check
   * @param solenoidOutput the new solenoid output
   */
  virtual void SetSolenoidOutput(int channel, bool solenoidOutput) = 0;

  /**
   * Register a callback to be run when the solenoid output on a channel
   * changes.
   *
   * @param channel the channel to monitor
   * @param callback the callback
   * @param initialNotify should the callback be run with the initial value
   * @return the {@link CallbackStore} object associated with this callback.
   * Save a reference to this object; it being deconstructed cancels the
   * callback.
   */
  [[nodiscard]]
  virtual std::unique_ptr<CallbackStore> RegisterSolenoidOutputCallback(
      int channel, NotifyCallback callback, bool initialNotify) = 0;

  /**
   * Check the value of the pressure switch.
   *
   * @return the pressure switch value
   */
  virtual bool GetPressureSwitch() const = 0;

  /**
   * Set the value of the pressure switch.
   *
   * @param pressureSwitch the new value
   */
  virtual void SetPressureSwitch(bool pressureSwitch) = 0;

  /**
   * Register a callback to be run whenever the pressure switch value changes.
   *
   * @param callback the callback
   * @param initialNotify whether the callback should be called with the initial
   * value
   * @return the {@link CallbackStore} object associated with this callback.
   * Save a reference to this object; it being deconstructed cancels the
   * callback.
   */
  [[nodiscard]]
  virtual std::unique_ptr<CallbackStore> RegisterPressureSwitchCallback(
      NotifyCallback callback, bool initialNotify) = 0;

  /**
   * Read the compressor current.
   *
   * @return the current of the compressor connected to this module
   */
  virtual double GetCompressorCurrent() const = 0;

  /**
   * Set the compressor current.
   *
   * @param compressorCurrent the new compressor current
   */
  virtual void SetCompressorCurrent(double compressorCurrent) = 0;

  /**
   * Register a callback to be run whenever the compressor current changes.
   *
   * @param callback the callback
   * @param initialNotify whether to call the callback with the initial state
   * @return the {@link CallbackStore} object associated with this callback.
   * Save a reference to this object; it being deconstructed cancels the
   * callback.
   */
  [[nodiscard]]
  virtual std::unique_ptr<CallbackStore> RegisterCompressorCurrentCallback(
      NotifyCallback callback, bool initialNotify) = 0;

  /**
   * Get the current value of all solenoid outputs.
   *
   * @return the solenoid outputs (1 bit per output)
   */
  virtual uint8_t GetAllSolenoidOutputs() const = 0;

  /**
   * Change all of the solenoid outputs.
   *
   * @param outputs the new solenoid outputs (1 bit per output)
   */
  virtual void SetAllSolenoidOutputs(uint8_t outputs) = 0;

  /** Reset all simulation data for this object. */
  virtual void ResetData() = 0;

 protected:
  /// PneumaticsBase index.
  const int m_index;

  /**
   * Constructs a PneumaticsBaseSim with the given index.
   *
   * @param index The index.
   */
  explicit PneumaticsBaseSim(const int index);

  /**
   * Constructs a PneumaticsBaseSim for the given module.
   *
   * @param module The module.
   */
  explicit PneumaticsBaseSim(const PneumaticsBase& module);
};

}  // namespace frc::sim
