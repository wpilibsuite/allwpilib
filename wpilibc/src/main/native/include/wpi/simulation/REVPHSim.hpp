// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <memory>

#include "frc/PneumaticsBase.h"
#include "frc/simulation/CallbackStore.h"
#include "frc/simulation/PneumaticsBaseSim.h"

namespace frc {

class Compressor;

namespace sim {

/**
 * Class to control a simulated Pneumatic Control Module (PCM).
 */
class REVPHSim : public PneumaticsBaseSim {
 public:
  /**
   * Constructs with the default PCM module number (CAN ID).
   */
  REVPHSim();

  /**
   * Constructs from a PCM module number (CAN ID).
   *
   * @param module module number
   */
  explicit REVPHSim(int module);

  explicit REVPHSim(const PneumaticsBase& pneumatics);

  ~REVPHSim() override = default;

  [[nodiscard]]
  std::unique_ptr<CallbackStore> RegisterInitializedCallback(
      NotifyCallback callback, bool initialNotify) override;

  bool GetInitialized() const override;

  void SetInitialized(bool solenoidInitialized) override;

  [[nodiscard]]
  std::unique_ptr<CallbackStore> RegisterSolenoidOutputCallback(
      int channel, NotifyCallback callback, bool initialNotify) override;

  bool GetSolenoidOutput(int channel) const override;

  void SetSolenoidOutput(int channel, bool solenoidOutput) override;

  [[nodiscard]]
  std::unique_ptr<CallbackStore> RegisterCompressorOnCallback(
      NotifyCallback callback, bool initialNotify) override;

  /**
   * Check if the compressor is on.
   *
   * @return true if the compressor is active
   */
  bool GetCompressorOn() const override;

  /**
   * Set whether the compressor is active.
   *
   * @param compressorOn the new value
   */
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
  std::unique_ptr<CallbackStore> RegisterCompressorConfigTypeCallback(
      NotifyCallback callback, bool initialNotify);

  /**
   * Check whether the closed loop compressor control is active.
   *
   * @return compressor config type
   */
  int GetCompressorConfigType() const;

  /**
   * Turn on/off the closed loop control of the compressor.
   *
   * @param compressorConfigType compressor config type
   */
  void SetCompressorConfigType(int compressorConfigType);

  [[nodiscard]]
  std::unique_ptr<CallbackStore> RegisterPressureSwitchCallback(
      NotifyCallback callback, bool initialNotify) override;

  bool GetPressureSwitch() const override;

  void SetPressureSwitch(bool pressureSwitch) override;

  [[nodiscard]]
  std::unique_ptr<CallbackStore> RegisterCompressorCurrentCallback(
      NotifyCallback callback, bool initialNotify) override;

  double GetCompressorCurrent() const override;

  void SetCompressorCurrent(double compressorCurrent) override;

  uint8_t GetAllSolenoidOutputs() const override;

  void SetAllSolenoidOutputs(uint8_t outputs) override;

  void ResetData() override;
};
}  // namespace sim
}  // namespace frc
