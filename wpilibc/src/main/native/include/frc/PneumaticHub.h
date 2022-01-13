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

  struct Version {
    uint32_t FirmwareMajor;
    uint32_t FirmwareMinor;
    uint32_t FirmwareFix;
    uint32_t HardwareMinor;
    uint32_t HardwareMajor;
    uint32_t UniqueId;
  };

  Version GetVersion() const;

  struct Faults {
    uint32_t Channel0Fault : 1;
    uint32_t Channel1Fault : 1;
    uint32_t Channel2Fault : 1;
    uint32_t Channel3Fault : 1;
    uint32_t Channel4Fault : 1;
    uint32_t Channel5Fault : 1;
    uint32_t Channel6Fault : 1;
    uint32_t Channel7Fault : 1;
    uint32_t Channel8Fault : 1;
    uint32_t Channel9Fault : 1;
    uint32_t Channel10Fault : 1;
    uint32_t Channel11Fault : 1;
    uint32_t Channel12Fault : 1;
    uint32_t Channel13Fault : 1;
    uint32_t Channel14Fault : 1;
    uint32_t Channel15Fault : 1;
    uint32_t CompressorOverCurrent : 1;
    uint32_t CompressorOpen : 1;
    uint32_t SolenoidOverCurrent : 1;
    uint32_t Brownout : 1;
    uint32_t CanWarning : 1;
    uint32_t HardwareFault : 1;
  };

  Faults GetFaults() const;

  struct StickyFaults {
    uint32_t CompressorOverCurrent : 1;
    uint32_t CompressorOpen : 1;
    uint32_t SolenoidOverCurrent : 1;
    uint32_t Brownout : 1;
    uint32_t CanWarning : 1;
    uint32_t CanBusOff : 1;
    uint32_t HasReset : 1;
  };

  StickyFaults GetStickyFaults() const;

  void ClearStickyFaults();

  units::volt_t GetInputVoltage() const;

  units::volt_t Get5VRegulatedVoltage() const;

  units::ampere_t GetSolenoidsTotalCurrent() const;

  units::volt_t GetSolenoidsVoltage() const;

  units::volt_t GetAnalogVoltage(int channel) const override;

  units::pounds_per_square_inch_t GetPressure(int channel) const override;

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
