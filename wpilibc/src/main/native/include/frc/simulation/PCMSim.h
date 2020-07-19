/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

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

  std::unique_ptr<CallbackStore> RegisterSolenoidInitializedCallback(
      int channel, NotifyCallback callback, bool initialNotify);

  bool GetSolenoidInitialized(int channel) const;

  void SetSolenoidInitialized(int channel, bool solenoidInitialized);

  std::unique_ptr<CallbackStore> RegisterSolenoidOutputCallback(
      int channel, NotifyCallback callback, bool initialNotify);

  bool GetSolenoidOutput(int channel) const;

  void SetSolenoidOutput(int channel, bool solenoidOutput);

  std::unique_ptr<CallbackStore> RegisterCompressorInitializedCallback(
      NotifyCallback callback, bool initialNotify);

  bool GetCompressorInitialized() const;

  void SetCompressorInitialized(bool compressorInitialized);

  std::unique_ptr<CallbackStore> RegisterCompressorOnCallback(
      NotifyCallback callback, bool initialNotify);

  bool GetCompressorOn() const;

  void SetCompressorOn(bool compressorOn);

  std::unique_ptr<CallbackStore> RegisterClosedLoopEnabledCallback(
      NotifyCallback callback, bool initialNotify);

  bool GetClosedLoopEnabled() const;

  void SetClosedLoopEnabled(bool closedLoopEnabled);

  std::unique_ptr<CallbackStore> RegisterPressureSwitchCallback(
      NotifyCallback callback, bool initialNotify);

  bool GetPressureSwitch() const;

  void SetPressureSwitch(bool pressureSwitch);

  std::unique_ptr<CallbackStore> RegisterCompressorCurrentCallback(
      NotifyCallback callback, bool initialNotify);

  double GetCompressorCurrent() const;

  void SetCompressorCurrent(double compressorCurrent);

  uint8_t GetAllSolenoidOutputs() const;

  void SetAllSolenoidOutputs(uint8_t outputs);

  void ResetData();

 private:
  int m_index;
};
}  // namespace sim
}  // namespace frc
