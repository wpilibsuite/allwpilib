// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <vector>

#include <hal/PowerDistribution.h>
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
  /// Default module number.
  static constexpr int kDefaultModule = -1;

  /**
   * Power distribution module type.
   */
  enum class ModuleType {
    /// CTRE (Cross The Road Electronics) CTRE Power Distribution Panel (PDP).
    kCTRE = 1,
    /// REV Power Distribution Hub (PDH).
    kRev = 2
  };

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

  PowerDistribution(PowerDistribution&&) = default;
  PowerDistribution& operator=(PowerDistribution&&) = default;

  ~PowerDistribution() override = default;

  /**
   * Gets the number of channels for this power distribution object.
   *
   * @return Number of output channels (16 for PDP, 24 for PDH).
   */
  int GetNumChannels() const;

  /**
   * Query the input voltage of the PDP/PDH.
   *
   * @return The input voltage in volts
   */
  double GetVoltage() const;

  /**
   * Query the temperature of the PDP.
   *
   * Not supported on the Rev PDH and returns 0.
   *
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
   * Query all currents of the PDP.
   *
   * @return The current of each channel in Amperes
   */
  std::vector<double> GetAllCurrents() const;

  /**
   * Query the total current of all monitored PDP/PDH channels.
   *
   * @return The total current drawn from all channels in Amperes
   */
  double GetTotalCurrent() const;

  /**
   * Query the total power drawn from all monitored PDP channels.
   *
   * Not supported on the Rev PDH and returns 0.
   *
   * @return The total power drawn in Watts
   */
  double GetTotalPower() const;

  /**
   * Query the total energy drawn from the monitored PDP channels.
   *
   * Not supported on the Rev PDH and returns 0.
   *
   * @return The total energy drawn in Joules
   */
  double GetTotalEnergy() const;

  /**
   * Reset the total energy drawn from the PDP.
   *
   * Not supported on the Rev PDH and does nothing.
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

  /** Version and device data received from a PowerDistribution device */
  struct Version {
    /** Firmware major version number. */
    uint32_t FirmwareMajor;
    /** Firmware minor version number. */
    uint32_t FirmwareMinor;
    /** Firmware fix version number. */
    uint32_t FirmwareFix;
    /** Hardware minor version number. */
    uint32_t HardwareMinor;
    /** Hardware major version number. */
    uint32_t HardwareMajor;
    /** Unique ID. */
    uint32_t UniqueId;
  };

  Version GetVersion() const;

  /**
   * Faults for a PowerDistribution device. These faults are only active while
   * the condition is active.
   */
  struct Faults {
    /** Breaker fault on channel 0. */
    uint32_t Channel0BreakerFault : 1;
    /** Breaker fault on channel 1. */
    uint32_t Channel1BreakerFault : 1;
    /** Breaker fault on channel 2. */
    uint32_t Channel2BreakerFault : 1;
    /** Breaker fault on channel 3. */
    uint32_t Channel3BreakerFault : 1;
    /** Breaker fault on channel 4. */
    uint32_t Channel4BreakerFault : 1;
    /** Breaker fault on channel 5. */
    uint32_t Channel5BreakerFault : 1;
    /** Breaker fault on channel 6. */
    uint32_t Channel6BreakerFault : 1;
    /** Breaker fault on channel 7. */
    uint32_t Channel7BreakerFault : 1;
    /** Breaker fault on channel 8. */
    uint32_t Channel8BreakerFault : 1;
    /** Breaker fault on channel 9. */
    uint32_t Channel9BreakerFault : 1;
    /** Breaker fault on channel 10. */
    uint32_t Channel10BreakerFault : 1;
    /** Breaker fault on channel 12. */
    uint32_t Channel11BreakerFault : 1;
    /** Breaker fault on channel 13. */
    uint32_t Channel12BreakerFault : 1;
    /** Breaker fault on channel 14. */
    uint32_t Channel13BreakerFault : 1;
    /** Breaker fault on channel 15. */
    uint32_t Channel14BreakerFault : 1;
    /** Breaker fault on channel 16. */
    uint32_t Channel15BreakerFault : 1;
    /** Breaker fault on channel 17. */
    uint32_t Channel16BreakerFault : 1;
    /** Breaker fault on channel 18. */
    uint32_t Channel17BreakerFault : 1;
    /** Breaker fault on channel 19. */
    uint32_t Channel18BreakerFault : 1;
    /** Breaker fault on channel 20. */
    uint32_t Channel19BreakerFault : 1;
    /** Breaker fault on channel 21. */
    uint32_t Channel20BreakerFault : 1;
    /** Breaker fault on channel 22. */
    uint32_t Channel21BreakerFault : 1;
    /** Breaker fault on channel 23. */
    uint32_t Channel22BreakerFault : 1;
    /** Breaker fault on channel 24. */
    uint32_t Channel23BreakerFault : 1;
    /** The input voltage is below the minimum voltage. */
    uint32_t Brownout : 1;
    /** A warning was raised by the device's CAN controller. */
    uint32_t CanWarning : 1;
    /** The hardware on the device has malfunctioned. */
    uint32_t HardwareFault : 1;

    /**
     * Gets whether there is a breaker fault at a specified channel.
     * @param channel Channel to check for faults.
     * @return If there is a breaker fault.
     * @throws A ChannelIndexOutOfRange error if the given int is outside of the
     * range supported by the hardware.
     */
    bool GetBreakerFault(int channel) const;
  };

  /**
   * Returns the power distribution faults.
   *
   * On a CTRE PDP, this will return an object with no faults active.
   *
   * @return The power distribution faults.
   */
  Faults GetFaults() const;

  /**
   * Sticky faults for a PowerDistribution device. These faults will remain
   * active until they are reset by the user.
   */
  struct StickyFaults {
    /** Breaker fault on channel 0. */
    uint32_t Channel0BreakerFault : 1;
    /** Breaker fault on channel 1. */
    uint32_t Channel1BreakerFault : 1;
    /** Breaker fault on channel 2. */
    uint32_t Channel2BreakerFault : 1;
    /** Breaker fault on channel 3. */
    uint32_t Channel3BreakerFault : 1;
    /** Breaker fault on channel 4. */
    uint32_t Channel4BreakerFault : 1;
    /** Breaker fault on channel 5. */
    uint32_t Channel5BreakerFault : 1;
    /** Breaker fault on channel 6. */
    uint32_t Channel6BreakerFault : 1;
    /** Breaker fault on channel 7. */
    uint32_t Channel7BreakerFault : 1;
    /** Breaker fault on channel 8. */
    uint32_t Channel8BreakerFault : 1;
    /** Breaker fault on channel 9. */
    uint32_t Channel9BreakerFault : 1;
    /** Breaker fault on channel 10. */
    uint32_t Channel10BreakerFault : 1;
    /** Breaker fault on channel 12. */
    uint32_t Channel11BreakerFault : 1;
    /** Breaker fault on channel 13. */
    uint32_t Channel12BreakerFault : 1;
    /** Breaker fault on channel 14. */
    uint32_t Channel13BreakerFault : 1;
    /** Breaker fault on channel 15. */
    uint32_t Channel14BreakerFault : 1;
    /** Breaker fault on channel 16. */
    uint32_t Channel15BreakerFault : 1;
    /** Breaker fault on channel 17. */
    uint32_t Channel16BreakerFault : 1;
    /** Breaker fault on channel 18. */
    uint32_t Channel17BreakerFault : 1;
    /** Breaker fault on channel 19. */
    uint32_t Channel18BreakerFault : 1;
    /** Breaker fault on channel 20. */
    uint32_t Channel19BreakerFault : 1;
    /** Breaker fault on channel 21. */
    uint32_t Channel20BreakerFault : 1;
    /** Breaker fault on channel 22. */
    uint32_t Channel21BreakerFault : 1;
    /** Breaker fault on channel 23. */
    uint32_t Channel22BreakerFault : 1;
    /** Breaker fault on channel 24. */
    uint32_t Channel23BreakerFault : 1;
    /** The input voltage is below the minimum voltage. */
    uint32_t Brownout : 1;
    /** A warning was raised by the device's CAN controller. */
    uint32_t CanWarning : 1;
    /** The device's CAN controller experienced a "Bus Off" event. */
    uint32_t CanBusOff : 1;
    /** The hardware on the device has malfunctioned. */
    uint32_t HardwareFault : 1;
    /** The firmware on the device has malfunctioned. */
    uint32_t FirmwareFault : 1;
    /** The device has rebooted. */
    uint32_t HasReset : 1;

    /**
     * Gets whether there is a sticky breaker fault at the specified channel.
     * @param channel Index to check for sticky faults.
     * @return True if there is a sticky breaker fault at the channel, otherwise
     * false.
     * @throws A ChannelIndexOutOfRange error if the provided channel is outside
     * of the range supported by the hardware.
     */
    bool GetBreakerFault(int channel) const;
  };

  /**
   * Returns the power distribution sticky faults.
   *
   * On a CTRE PDP, this will return an object with no faults active.
   *
   * @return The power distribution sticky faults.
   */
  StickyFaults GetStickyFaults() const;

  void InitSendable(wpi::SendableBuilder& builder) override;

 private:
  hal::Handle<HAL_PowerDistributionHandle, HAL_CleanPowerDistribution> m_handle;
  int m_module;
};

}  // namespace frc
