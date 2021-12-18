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
 * CAN PDP.
 */
class PowerDistribution : public wpi::Sendable,
                          public wpi::SendableHelper<PowerDistribution> {
 public:
  static constexpr int kDefaultModule = -1;
  enum class ModuleType { kCTRE = 1, kRev = 2 };

  /**
   * Constructs a PowerDistribution.
   *
   * Uses the default CAN ID (0 for CTRE and 1 for REV).
   */
  PowerDistribution();

  /**
   * Constructs a PowerDistribution.
   *
   * @param module The CAN ID of the PDP
   * @param moduleType The type of module
   */
  PowerDistribution(int module, ModuleType moduleType);

  ~PowerDistribution() override;
  PowerDistribution(PowerDistribution&&) = default;
  PowerDistribution& operator=(PowerDistribution&&) = default;

  /**
   * Query the input voltage of the PDP.
   *
   * @return The voltage of the PDP in volts
   */
  double GetVoltage() const;

  /**
   * Query the temperature of the PDP.
   *
   * @return The temperature of the PDP in degrees Celsius
   */
  double GetTemperature() const;

  /**
   * Query the current of a single channel of the PDP.
   *
   * @return The current of one of the PDP channels (channels 0-15) in Amperes
   */
  double GetCurrent(int channel) const;

  /**
   * Query the total current of all monitored PDP channels (0-15).
   *
   * @return The the total current drawn from the PDP channels in Amperes
   */
  double GetTotalCurrent() const;

  /**
   * Query the total power drawn from the monitored PDP channels.
   *
   * @return The the total power drawn from the PDP channels in Watts
   */
  double GetTotalPower() const;

  /**
   * Query the total energy drawn from the monitored PDP channels.
   *
   * @return The the total energy drawn from the PDP channels in Joules
   */
  double GetTotalEnergy() const;

  /**
   * Reset the total energy drawn from the PDP.
   *
   * @see PowerDistribution#GetTotalEnergy
   */
  void ResetTotalEnergy();

  /**
   * Remove all of the fault flags on the PDP.
   */
  void ClearStickyFaults();

  /**
   * Gets module number (CAN ID).
   */
  int GetModule() const;

  bool GetSwitchableChannel() const;

  void SetSwitchableChannel(bool enabled);

  struct Version {
    uint32_t FirmwareMajor;
    uint32_t FirmwareMinor;
    uint32_t FirmwareFix;
    uint32_t FardwareMinor;
    uint32_t FardwareMajor;
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
