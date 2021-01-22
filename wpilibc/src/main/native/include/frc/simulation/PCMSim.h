// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <memory>

#include "frc/simulation/CallbackStore.h"

namespace frc {

class Compressor;

namespace sim {

/**
 * Class to control a simulated Pneumatic Control Module (PCM).
 */
class PCMSim {
 public:
  /**
   * Constructs with the default PCM module number (CAN ID).
   */
  PCMSim();

  /**
   * Constructs from a PCM module number (CAN ID).
   *
   * @param module module number
   */
  explicit PCMSim(int module);

  /**
   * Constructs from a Compressor object.
   *
   * @param compressor Compressor connected to PCM to simulate
   */
  explicit PCMSim(const Compressor& compressor);

  /**
   * Register a callback to be run when a solenoid is initialized on a channel.
   *
   * @param channel the channel to monitor
   * @param callback the callback
   * @param initialNotify should the callback be run with the initial state
   * @return the CallbackStore object associated with this callback
   */
  [[nodiscard]] std::unique_ptr<CallbackStore>
  RegisterSolenoidInitializedCallback(int channel, NotifyCallback callback,
                                      bool initialNotify);

  /**
   * Check if a solenoid has been initialized on a specific channel.
   *
   * @param channel the channel to check
   * @return true if initialized
   */
  bool GetSolenoidInitialized(int channel) const;

  /**
   * Define whether a solenoid has been initialized on a specific channel.
   *
   * @param channel the channel
   * @param solenoidInitialized is there a solenoid initialized on that channel
   */
  void SetSolenoidInitialized(int channel, bool solenoidInitialized);

  /**
   * Register a callback to be run when the solenoid output on a channel
   * changes.
   *
   * @param channel the channel to monitor
   * @param callback the callback
   * @param initialNotify should the callback be run with the initial value
   * @return the CallbackStore object associated with this callback
   */
  [[nodiscard]] std::unique_ptr<CallbackStore> RegisterSolenoidOutputCallback(
      int channel, NotifyCallback callback, bool initialNotify);

  /**
   * Check the solenoid output on a specific channel.
   *
   * @param channel the channel to check
   * @return the solenoid output
   */
  bool GetSolenoidOutput(int channel) const;

  /**
   * Change the solenoid output on a specific channel.
   *
   * @param channel the channel to check
   * @param solenoidOutput the new solenoid output
   */
  void SetSolenoidOutput(int channel, bool solenoidOutput);

  /**
   * Register a callback to be run when the compressor is initialized.
   *
   * @param callback the callback
   * @param initialNotify whether to run the callback with the initial state
   * @return the CallbackStore object associated with this callback
   */
  [[nodiscard]] std::unique_ptr<CallbackStore>
  RegisterCompressorInitializedCallback(NotifyCallback callback,
                                        bool initialNotify);

  /**
   * Check whether the compressor has been initialized.
   *
   * @return true if initialized
   */
  bool GetCompressorInitialized() const;

  /**
   * Define whether the compressor has been initialized.
   *
   * @param compressorInitialized whether the compressor is initialized
   */
  void SetCompressorInitialized(bool compressorInitialized);

  /**
   * Register a callback to be run when the compressor activates.
   *
   * @param callback the callback
   * @param initialNotify whether to run the callback with the initial state
   * @return the CallbackStore object associated with this callback
   */
  [[nodiscard]] std::unique_ptr<CallbackStore> RegisterCompressorOnCallback(
      NotifyCallback callback, bool initialNotify);

  /**
   * Check if the compressor is on.
   *
   * @return true if the compressor is active
   */
  bool GetCompressorOn() const;

  /**
   * Set whether the compressor is active.
   *
   * @param compressorOn the new value
   */
  void SetCompressorOn(bool compressorOn);

  /**
   * Register a callback to be run whenever the closed loop state changes.
   *
   * @param callback the callback
   * @param initialNotify whether the callback should be called with the
   *                      initial value
   * @return the CallbackStore object associated with this callback
   */
  [[nodiscard]] std::unique_ptr<CallbackStore>
  RegisterClosedLoopEnabledCallback(NotifyCallback callback,
                                    bool initialNotify);

  /**
   * Check whether the closed loop compressor control is active.
   *
   * @return true if active
   */
  bool GetClosedLoopEnabled() const;

  /**
   * Turn on/off the closed loop control of the compressor.
   *
   * @param closedLoopEnabled whether the control loop is active
   */
  void SetClosedLoopEnabled(bool closedLoopEnabled);

  /**
   * Register a callback to be run whenever the pressure switch value changes.
   *
   * @param callback the callback
   * @param initialNotify whether the callback should be called with the
   *                      initial value
   * @return the CallbackStore object associated with this callback
   */
  [[nodiscard]] std::unique_ptr<CallbackStore> RegisterPressureSwitchCallback(
      NotifyCallback callback, bool initialNotify);

  /**
   * Check the value of the pressure switch.
   *
   * @return the pressure switch value
   */
  bool GetPressureSwitch() const;

  /**
   * Set the value of the pressure switch.
   *
   * @param pressureSwitch the new value
   */
  void SetPressureSwitch(bool pressureSwitch);

  /**
   * Register a callback to be run whenever the compressor current changes.
   *
   * @param callback the callback
   * @param initialNotify whether to call the callback with the initial state
   * @return the CallbackStore object associated with this callback
   */
  [[nodiscard]] std::unique_ptr<CallbackStore>
  RegisterCompressorCurrentCallback(NotifyCallback callback,
                                    bool initialNotify);

  /**
   * Read the compressor current.
   *
   * @return the current of the compressor connected to this module
   */
  double GetCompressorCurrent() const;

  /**
   * Set the compressor current.
   *
   * @param compressorCurrent the new compressor current
   */
  void SetCompressorCurrent(double compressorCurrent);

  /**
   * Get the current value of all solenoid outputs.
   *
   * @return the solenoid outputs (1 bit per output)
   */
  uint8_t GetAllSolenoidOutputs() const;

  /**
   * Change all of the solenoid outputs.
   *
   * @param outputs the new solenoid outputs (1 bit per output)
   */
  void SetAllSolenoidOutputs(uint8_t outputs);

  /**
   * Reset all simulation data for this object.
   */
  void ResetData();

 private:
  int m_index;
};
}  // namespace sim
}  // namespace frc
