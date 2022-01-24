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
class PneumaticsControlModule : public PneumaticsBase {
 public:
  PneumaticsControlModule();
  explicit PneumaticsControlModule(int module);

  ~PneumaticsControlModule() override = default;

  bool GetCompressor() const override;

  void DisableCompressor() override;

  void EnableCompressorDigital() override;

  void EnableCompressorAnalog(
      units::pounds_per_square_inch_t minPressure,
      units::pounds_per_square_inch_t maxPressure) override;

  void EnableCompressorHybrid(
      units::pounds_per_square_inch_t minPressure,
      units::pounds_per_square_inch_t maxPressure) override;

  CompressorConfigType GetCompressorConfigType() const override;

  bool GetPressureSwitch() const override;

  units::ampere_t GetCompressorCurrent() const override;

  bool GetCompressorCurrentTooHighFault() const;
  bool GetCompressorCurrentTooHighStickyFault() const;
  bool GetCompressorShortedFault() const;
  bool GetCompressorShortedStickyFault() const;
  bool GetCompressorNotConnectedFault() const;
  bool GetCompressorNotConnectedStickyFault() const;

  bool GetSolenoidVoltageFault() const;
  bool GetSolenoidVoltageStickyFault() const;

  void ClearAllStickyFaults();

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

  units::volt_t GetAnalogVoltage(int channel) const override;

  units::pounds_per_square_inch_t GetPressure(int channel) const override;

  Solenoid MakeSolenoid(int channel) override;
  DoubleSolenoid MakeDoubleSolenoid(int forwardChannel,
                                    int reverseChannel) override;
  Compressor MakeCompressor() override;

 private:
  class DataStore;
  friend class DataStore;
  friend class PneumaticsBase;
  PneumaticsControlModule(HAL_CTREPCMHandle handle, int module);

  static std::shared_ptr<PneumaticsBase> GetForModule(int module);

  std::shared_ptr<DataStore> m_dataStore;
  HAL_CTREPCMHandle m_handle;
  int m_module;

  static wpi::mutex m_handleLock;
  static std::unique_ptr<wpi::DenseMap<int, std::weak_ptr<DataStore>>>
      m_handleMap;
  static std::weak_ptr<DataStore>& GetDataStore(int module);
};
}  // namespace frc
