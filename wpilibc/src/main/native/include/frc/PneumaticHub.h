// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <memory>

#include <hal/Types.h>
#include <wpi/DenseMap.h>
#include <wpi/mutex.h>

#include "PneumaticsBase.h"

namespace frc {
class PneumaticHub : public PneumaticsBase {
 public:
  PneumaticHub();
  explicit PneumaticHub(int module);

  ~PneumaticHub() override = default;

  bool GetCompressor() const override;

  void DiableCompressor() override;

  void EnableCompressorDigital() override;

  void EnableCompressorAnalog(double minAnalogVoltage,
                              double maxAnalogVoltage) override;

  void EnableCompressorHybrid(double minAnalogVoltage,
                              double maxAnalogVoltage) override;

  CompressorControlType GetCompressorControlType() const override;

  bool GetPressureSwitch() const override;

  double GetCompressorCurrent() const override;

  void SetSolenoids(int mask, int values) override;

  int GetSolenoids() const override;

  int GetModuleNumber() const override;

  int GetSolenoidDisabledList() const override;

  void FireOneShot(int index) override;

  void SetOneShotDuration(int index, units::second_t duration) override;

  bool CheckSolenoidChannel(int channel) const override;

  int CheckAndReserveSolenoids(int mask) override;

  void UnreserveSolenoids(int mask) override;

  bool ReserveCompressor() override;

  void UnreserveCompressor() override;

  Solenoid MakeSolenoid(int channel) override;
  DoubleSolenoid MakeDoubleSolenoid(int forwardChannel,
                                    int reverseChannel) override;
  Compressor MakeCompressor() override;

 private:
  class DataStore;
  friend class DataStore;
  friend class PneumaticsBase;
  PneumaticHub(HAL_REVPHHandle handle, int module);

  static std::shared_ptr<PneumaticsBase> GetForModule(int module);

  std::shared_ptr<DataStore> m_dataStore;
  HAL_REVPHHandle m_handle;
  int m_module;

  static wpi::mutex m_handleLock;
  static std::unique_ptr<wpi::DenseMap<int, std::weak_ptr<DataStore>>>
      m_handleMap;
  static std::weak_ptr<DataStore>& GetDataStore(int module);
};
}  // namespace frc
