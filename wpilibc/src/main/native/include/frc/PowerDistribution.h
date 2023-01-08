// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <hal/Types.h>
#include <wpi/sendable/Sendable.h>
#include <wpi/sendable/SendableHelper.h>

namespace frc {

/**
 * Class for getting voltage, current, temperature, power and energy from the
 * CTRE Power Distribution Panel (PDP) or REV Power Distribution Hub (PDH).
 */
class PowerDistribution : public wpi::Sendable,
                          public wpi::SendableHelper<PowerDistribution> {
 public:
  static constexpr int kDefaultModule = -1;
  enum class ModuleType { kCTRE = 1, kRev = 2 };

  /**
   * Constructs a PowerDistribution object.
   *
   * Detects the connected PDP/PDH using the default CAN ID (0 for CTRE and 1
   * for REV).
   */
  PowerDistribution();

  /**
   * Constructs a PowerDistribution object.
   *
   * @param module The CAN ID of the PDP/PDH
   * @param moduleType The type of module
   */
  PowerDistribution(int module, ModuleType moduleType);

  ~PowerDistribution() override;
  PowerDistribution(PowerDistribution&&) = default;
  PowerDistribution& operator=(PowerDistribution&&) = default;

  /**
   * Query the input voltage of the PDP/PDH.
   *
   * @return The input voltage in volts
   */
  double GetVoltage() const;

  /**
   * Query the temperature of the PDP/PDH.
   *
   * @return The temperature in degrees Celsius
   */
  double GetTemperature() const;

  /**
   * Query the current of a single channel of the PDP/PDH.
   *
   * @param channel the channel to query (0-15 for PDP, 0-23 for PDH)
   * @return The current of the channel in Amperes
   */
  double GetCurrent(int channel) const;

  /**
   * Query the total current of all monitored PDP/PDH channels.
   *
   * @return The total current drawn from all channels in Amperes
   */
  double GetTotalCurrent() const;

  /**
   * Query the total power drawn from all monitored PDP/PDH channels.
   *
   * @return The total power drawn in Watts
   */
  double GetTotalPower() const;

  /**
   * Query the total energy drawn from the monitored PDP/PDH channels.
   *
   * @return The total energy drawn in Joules
   */
  double GetTotalEnergy() const;

  /**
   * Reset the total energy drawn from the PDP/PDH.
   *
   * @see PowerDistribution#GetTotalEnergy
   */
  void ResetTotalEnergy();

  /**
   * Remove all of the fault flags on the PDP/PDH.
   */
  void ClearStickyFaults();

  /**
   * Gets module number (CAN ID).
   */
  int GetModule() const;

  /**
   * Gets module type.
   */
  ModuleType GetType() const;

  /**
   * Gets whether the PDH switchable channel is turned on or off. Returns false
   * with the CTRE PDP.
   *
   * @return The output state of the PDH switchable channel
   */
  bool GetSwitchableChannel() const;

  /**
   * Sets the PDH switchable channel on or off. Does nothing with the CTRE PDP.
   *
   * @param enabled Whether to turn the PDH switchable channel on or off
   */
  void SetSwitchableChannel(bool enabled);

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
    uint32_t Channel0BreakerFault : 1;
    uint32_t Channel1BreakerFault : 1;
    uint32_t Channel2BreakerFault : 1;
    uint32_t Channel3BreakerFault : 1;
    uint32_t Channel4BreakerFault : 1;
    uint32_t Channel5BreakerFault : 1;
    uint32_t Channel6BreakerFault : 1;
    uint32_t Channel7BreakerFault : 1;
    uint32_t Channel8BreakerFault : 1;
    uint32_t Channel9BreakerFault : 1;
    uint32_t Channel10BreakerFault : 1;
    uint32_t Channel11BreakerFault : 1;
    uint32_t Channel12BreakerFault : 1;
    uint32_t Channel13BreakerFault : 1;
    uint32_t Channel14BreakerFault : 1;
    uint32_t Channel15BreakerFault : 1;
    uint32_t Channel16BreakerFault : 1;
    uint32_t Channel17BreakerFault : 1;
    uint32_t Channel18BreakerFault : 1;
    uint32_t Channel19BreakerFault : 1;
    uint32_t Channel20BreakerFault : 1;
    uint32_t Channel21BreakerFault : 1;
    uint32_t Channel22BreakerFault : 1;
    uint32_t Channel23BreakerFault : 1;
    uint32_t Brownout : 1;
    uint32_t CanWarning : 1;
    uint32_t HardwareFault : 1;
  };

  Faults GetFaults() const;

  struct StickyFaults {
    uint32_t Channel0BreakerFault : 1;
    uint32_t Channel1BreakerFault : 1;
    uint32_t Channel2BreakerFault : 1;
    uint32_t Channel3BreakerFault : 1;
    uint32_t Channel4BreakerFault : 1;
    uint32_t Channel5BreakerFault : 1;
    uint32_t Channel6BreakerFault : 1;
    uint32_t Channel7BreakerFault : 1;
    uint32_t Channel8BreakerFault : 1;
    uint32_t Channel9BreakerFault : 1;
    uint32_t Channel10BreakerFault : 1;
    uint32_t Channel11BreakerFault : 1;
    uint32_t Channel12BreakerFault : 1;
    uint32_t Channel13BreakerFault : 1;
    uint32_t Channel14BreakerFault : 1;
    uint32_t Channel15BreakerFault : 1;
    uint32_t Channel16BreakerFault : 1;
    uint32_t Channel17BreakerFault : 1;
    uint32_t Channel18BreakerFault : 1;
    uint32_t Channel19BreakerFault : 1;
    uint32_t Channel20BreakerFault : 1;
    uint32_t Channel21BreakerFault : 1;
    uint32_t Channel22BreakerFault : 1;
    uint32_t Channel23BreakerFault : 1;
    uint32_t Brownout : 1;
    uint32_t CanWarning : 1;
    uint32_t CanBusOff : 1;
    uint32_t HasReset : 1;
  };

  StickyFaults GetStickyFaults() const;

  void InitSendable(wpi::SendableBuilder& builder) override;

 private:
  hal::Handle<HAL_PowerDistributionHandle> m_handle;
  int m_module;
};

}  // namespace frc
