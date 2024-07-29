// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <memory>

#include "frc/PneumaticsBase.h"
#include "frc/simulation/CallbackStore.h"
#include "frc/simulation/PneumaticsBaseSim.h"

namespace frc::sim {

/**
 * Class to control a simulated Pneumatic Control Module (PCM).
 */
class CTREPCMSim : public PneumaticsBaseSim {
 public:
  /**
   * Constructs with the default PCM module number (CAN ID).
   */
  CTREPCMSim();

  /**
   * Constructs from a PCM module number (CAN ID).
   *
   * @param module module number
   */
  explicit CTREPCMSim(int module);

  explicit CTREPCMSim(const PneumaticsBase& pneumatics);

  ~CTREPCMSim() override = default;

  [[nodiscard]]
  std::unique_ptr<CallbackStore> RegisterInitializedCallback(
      NotifyCallback callback, bool initialNotify) override;

  bool GetInitialized() const override;

  void SetInitialized(bool initialized) override;

  [[nodiscard]]
  std::unique_ptr<CallbackStore> RegisterSolenoidOutputCallback(
      int channel, NotifyCallback callback, bool initialNotify) override;

  bool GetSolenoidOutput(int channel) const override;

  void SetSolenoidOutput(int channel, bool solenoidOutput) override;

  [[nodiscard]]
  std::unique_ptr<CallbackStore> RegisterCompressorOnCallback(
      NotifyCallback callback, bool initialNotify) override;

  bool GetCompressorOn() const override;

  void SetCompressorOn(bool compressorOn) override;

  /**
   * Register a callback to be run whenever the closed loop state changes.
   *
   * @param callback the callback
   * @param initialNotify whether the callback should be called with the
   *                      initial value
   * @return the CallbackStore object associated with this callback
   */
  [[nodiscard]]
  std::unique_ptr<CallbackStore> RegisterClosedLoopEnabledCallback(
      NotifyCallback callback, bool initialNotify);

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
  [[nodiscard]]
  std::unique_ptr<CallbackStore> RegisterPressureSwitchCallback(
      NotifyCallback callback, bool initialNotify) override;

  /**
   * Check the value of the pressure switch.
   *
   * @return the pressure switch value
   */
  bool GetPressureSwitch() const override;

  /**
   * Set the value of the pressure switch.
   *
   * @param pressureSwitch the new value
   */
  void SetPressureSwitch(bool pressureSwitch) override;

  /**
   * Register a callback to be run whenever the compressor current changes.
   *
   * @param callback the callback
   * @param initialNotify whether to call the callback with the initial state
   * @return the CallbackStore object associated with this callback
   */
  [[nodiscard]]
  std::unique_ptr<CallbackStore> RegisterCompressorCurrentCallback(
      NotifyCallback callback, bool initialNotify) override;

  /**
   * Read the compressor current.
   *
   * @return the current of the compressor connected to this module
   */
  double GetCompressorCurrent() const override;

  /**
   * Set the compressor current.
   *
   * @param compressorCurrent the new compressor current
   */
  void SetCompressorCurrent(double compressorCurrent) override;

  uint8_t GetAllSolenoidOutputs() const override;

  void SetAllSolenoidOutputs(uint8_t outputs) override;

  void ResetData() override;
};
}  // namespace frc::sim
