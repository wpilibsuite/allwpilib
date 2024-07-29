// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <memory>

#include <hal/Types.h>
#include <units/pressure.h>
#include <wpi/DenseMap.h>
#include <wpi/mutex.h>

#include "PneumaticsBase.h"

namespace frc {
/** Module class for controlling a REV Robotics Pneumatic Hub. */
class PneumaticHub : public PneumaticsBase {
 public:
  /** Constructs a PneumaticHub with the default ID (1). */
  PneumaticHub();

  /**
   * Constructs a PneumaticHub.
   *
   * @param module module number to construct
   */
  explicit PneumaticHub(int module);

  ~PneumaticHub() override = default;

  bool GetCompressor() const override;

  /**
   * Disables the compressor. The compressor will not turn on until
   * EnableCompressorDigital(), EnableCompressorAnalog(), or
   * EnableCompressorHybrid() are called.
   */
  void DisableCompressor() override;

  void EnableCompressorDigital() override;

  /**
   * Enables the compressor in analog mode. This mode uses an analog pressure
   * sensor connected to analog channel 0 to cycle the compressor. The
   * compressor will turn on when the pressure drops below {@code minPressure}
   * and will turn off when the pressure reaches {@code maxPressure}.
   *
   * @param minPressure The minimum pressure. The compressor will turn on when
   * the pressure drops below this value. Range 0 - 120 PSI.
   * @param maxPressure The maximum pressure. The compressor will turn off when
   * the pressure reaches this value. Range 0 - 120 PSI. Must be larger then
   * minPressure.
   */
  void EnableCompressorAnalog(
      units::pounds_per_square_inch_t minPressure,
      units::pounds_per_square_inch_t maxPressure) override;

  /**
   * Enables the compressor in hybrid mode. This mode uses both a digital
   * pressure switch and an analog pressure sensor connected to analog channel 0
   * to cycle the compressor.
   *
   * The compressor will turn on when \a both:
   *
   * - The digital pressure switch indicates the system is not full AND
   * - The analog pressure sensor indicates that the pressure in the system is
   * below the specified minimum pressure.
   *
   * The compressor will turn off when \a either:
   *
   * - The digital pressure switch is disconnected or indicates that the system
   * is full OR
   * - The pressure detected by the analog sensor is greater than the specified
   * maximum pressure.
   *
   * @param minPressure The minimum pressure. The compressor will turn on when
   * the pressure drops below this value and the pressure switch indicates that
   * the system is not full.  Range 0 - 120 PSI.
   * @param maxPressure The maximum pressure. The compressor will turn off when
   * the pressure reaches this value or the pressure switch is disconnected or
   * indicates that the system is full. Range 0 - 120 PSI. Must be larger then
   * minPressure.
   */
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

  /**
   * Returns the hardware and firmware versions of this device.
   *
   * @return The hardware and firmware versions.
   */
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

    /**
     * Gets whether there is a fault at the specified channel.
     * @param channel Channel to check for faults.
     * @return True if a a fault exists at the channel, otherwise false.
     * @throws A ChannelIndexOutOfRange error if the provided channel is outside
     * of the range supported by the hardware.
     */
    bool GetChannelFault(int channel) const;
  };

  /**
   * Returns the faults currently active on this device.
   *
   * @return The faults.
   */
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

  /**
   * Returns the sticky faults currently active on this device.
   *
   * @return The sticky faults.
   */
  StickyFaults GetStickyFaults() const;

  /** Clears the sticky faults. */
  void ClearStickyFaults();

  /**
   * Returns the current input voltage for this device.
   *
   * @return The input voltage.
   */
  units::volt_t GetInputVoltage() const;

  /**
   * Returns the current voltage of the regulated 5v supply.
   *
   * @return The current voltage of the 5v supply.
   */
  units::volt_t Get5VRegulatedVoltage() const;

  /**
   * Returns the total current drawn by all solenoids.
   *
   * @return Total current drawn by all solenoids.
   */
  units::ampere_t GetSolenoidsTotalCurrent() const;

  /**
   * Returns the current voltage of the solenoid power supply.
   *
   * @return The current voltage of the solenoid power supply.
   */
  units::volt_t GetSolenoidsVoltage() const;

  /**
   * Returns the raw voltage of the specified analog input channel.
   *
   * @param channel The analog input channel to read voltage from.
   * @return The voltage of the specified analog input channel.
   */
  units::volt_t GetAnalogVoltage(int channel) const override;

  /**
   * Returns the pressure read by an analog pressure sensor on the specified
   * analog input channel.
   *
   * @param channel The analog input channel to read pressure from.
   * @return The pressure read by an analog pressure sensor on the specified
   * analog input channel.
   */
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
