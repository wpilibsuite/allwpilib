// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <stdint.h>

#include "hal/Types.h"

/**
 * @defgroup hal_rev_ph REV Pneumatic Hub (PH) Functions
 * @ingroup hal_capi
 * @{
 */

/**
 * The compressor configuration type
 */
HAL_ENUM(HAL_REVPHCompressorConfigType) {
  /** Disabled. */
  HAL_REVPHCompressorConfigType_kDisabled = 0,
  /** Digital. */
  HAL_REVPHCompressorConfigType_kDigital = 1,
  /** Analog. */
  HAL_REVPHCompressorConfigType_kAnalog = 2,
  /** Hybrid. */
  HAL_REVPHCompressorConfigType_kHybrid = 3,
};

/**
 * Storage for REV PH Version
 */
struct HAL_REVPHVersion {
  /** The firmware major version. */
  uint32_t firmwareMajor;
  /** The firmware minor version. */
  uint32_t firmwareMinor;
  /** The firmware fix version. */
  uint32_t firmwareFix;
  /** The hardware minor version. */
  uint32_t hardwareMinor;
  /** The hardware major version. */
  uint32_t hardwareMajor;
  /** The device's unique ID. */
  uint32_t uniqueId;
};

/**
 * Storage for compressor config
 */
struct HAL_REVPHCompressorConfig {
  double minAnalogVoltage;
  double maxAnalogVoltage;
  HAL_Bool forceDisable;
  HAL_Bool useDigital;
};

/**
 * Storage for REV PH Faults
 */
struct HAL_REVPHFaults {
  /** Fault on channel 0. */
  uint32_t channel0Fault : 1;
  /** Fault on channel 1. */
  uint32_t channel1Fault : 1;
  /** Fault on channel 2. */
  uint32_t channel2Fault : 1;
  /** Fault on channel 3. */
  uint32_t channel3Fault : 1;
  /** Fault on channel 4. */
  uint32_t channel4Fault : 1;
  /** Fault on channel 5. */
  uint32_t channel5Fault : 1;
  /** Fault on channel 6. */
  uint32_t channel6Fault : 1;
  /** Fault on channel 7. */
  uint32_t channel7Fault : 1;
  /** Fault on channel 8. */
  uint32_t channel8Fault : 1;
  /** Fault on channel 9. */
  uint32_t channel9Fault : 1;
  /** Fault on channel 10. */
  uint32_t channel10Fault : 1;
  /** Fault on channel 11. */
  uint32_t channel11Fault : 1;
  /** Fault on channel 12. */
  uint32_t channel12Fault : 1;
  /** Fault on channel 13. */
  uint32_t channel13Fault : 1;
  /** Fault on channel 14. */
  uint32_t channel14Fault : 1;
  /** Fault on channel 15. */
  uint32_t channel15Fault : 1;
  /** An overcurrent event occurred on the compressor output. */
  uint32_t compressorOverCurrent : 1;
  /** The compressor output has an open circuit. */
  uint32_t compressorOpen : 1;
  /** An overcurrent event occurred on a solenoid output. */
  uint32_t solenoidOverCurrent : 1;
  /** The input voltage is below the minimum voltage. */
  uint32_t brownout : 1;
  /** A warning was raised by the device's CAN controller. */
  uint32_t canWarning : 1;
  /** The hardware on the device has malfunctioned. */
  uint32_t hardwareFault : 1;
};

/**
 * Storage for REV PH Sticky Faults
 */
struct HAL_REVPHStickyFaults {
  /** An overcurrent event occurred on the compressor output. */
  uint32_t compressorOverCurrent : 1;
  /** The compressor output has an open circuit. */
  uint32_t compressorOpen : 1;
  /** An overcurrent event occurred on a solenoid output. */
  uint32_t solenoidOverCurrent : 1;
  /** The input voltage is below the minimum voltage. */
  uint32_t brownout : 1;
  /** A warning was raised by the device's CAN controller. */
  uint32_t canWarning : 1;
  /** The device's CAN controller experienced a "Bus Off" event. */
  uint32_t canBusOff : 1;
  /** The hardware on the device has malfunctioned. */
  uint32_t hardwareFault : 1;
  /** The firmware on the device has malfunctioned. */
  uint32_t firmwareFault : 1;
  /** The device has rebooted. */
  uint32_t hasReset : 1;
};

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Initializes a PH.
 *
 * @param[in] module             the CAN ID to initialize
 * @param[in] allocationLocation the location where the allocation is occurring
 *                               (can be null)
 * @param[out] status            Error status variable. 0 on success.
 * @return the created PH handle
 */
HAL_REVPHHandle HAL_InitializeREVPH(int32_t module,
                                    const char* allocationLocation,
                                    int32_t* status);

/**
 * Frees a PH handle.
 *
 * @param[in] handle the PH handle
 */
void HAL_FreeREVPH(HAL_REVPHHandle handle);

/**
 * Checks if a solenoid channel number is valid.
 *
 * @param[in] channel the channel to check
 * @return true if the channel is valid, otherwise false
 */
HAL_Bool HAL_CheckREVPHSolenoidChannel(int32_t channel);

/**
 * Checks if a PH module (CAN ID) is valid.
 *
 * @param[in] module the module to check
 * @return true if the module is valid, otherwise false
 */
HAL_Bool HAL_CheckREVPHModuleNumber(int32_t module);

/**
 * Get whether compressor is turned on.
 *
 * @param[in] handle  the PH handle
 * @param[out] status Error status variable. 0 on success.
 * @return true if the compressor is turned on
 */
HAL_Bool HAL_GetREVPHCompressor(HAL_REVPHHandle handle, int32_t* status);

/**
 * Send compressor configuration to the PH.
 *
 * @param[in] handle  the PH handle
 * @param[in] config  compressor configuration
 * @param[out] status Error status variable. 0 on success.
 */
void HAL_SetREVPHCompressorConfig(HAL_REVPHHandle handle,
                                  const HAL_REVPHCompressorConfig* config,
                                  int32_t* status);

/**
 * Disable Compressor.
 *
 * @param[in] handle  the PH handle
 * @param[out] status Error status variable. 0 on success.
 */
void HAL_SetREVPHClosedLoopControlDisabled(HAL_REVPHHandle handle,
                                           int32_t* status);

/**
 * Enables the compressor in digital mode using the digital pressure switch. The
 * compressor will turn on when the pressure switch indicates that the system is
 * not full, and will turn off when the pressure switch indicates that the
 * system is full.
 *
 * @param[in] handle  the PH handle
 * @param[out] status Error status variable. 0 on success.
 */
void HAL_SetREVPHClosedLoopControlDigital(HAL_REVPHHandle handle,
                                          int32_t* status);

/**
 * Enables the compressor in analog mode. This mode uses an analog
 * pressure sensor connected to analog channel 0 to cycle the compressor. The
 * compressor will turn on when the pressure drops below minAnalogVoltage and
 * will turn off when the pressure reaches maxAnalogVoltage. This mode is only
 * supported by the REV PH with the REV Analog Pressure Sensor connected to
 * analog channel 0.
 * @param[in] handle  the PH handle
 * @param[in] minAnalogVoltage The compressor will turn on when the analog
 * pressure sensor voltage drops below this value
 * @param[in] maxAnalogVoltage The compressor will turn off when the analog
 * pressure sensor reaches this value.
 * @param[out] status Error status variable. 0 on success.
 */
void HAL_SetREVPHClosedLoopControlAnalog(HAL_REVPHHandle handle,
                                         double minAnalogVoltage,
                                         double maxAnalogVoltage,
                                         int32_t* status);

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
 * @param[in] handle  the PH handle
 * @param[in] minAnalogVoltage The compressor will turn on when the analog
 * pressure sensor voltage drops below this value and the pressure switch
 * indicates that the system is not full.
 * @param[in] maxAnalogVoltage The compressor will turn off when the analog
 * pressure sensor reaches this value or the pressure switch is disconnected or
 * indicates that the system is full.
 * @param[out] status Error status variable. 0 on success.
 */
void HAL_SetREVPHClosedLoopControlHybrid(HAL_REVPHHandle handle,
                                         double minAnalogVoltage,
                                         double maxAnalogVoltage,
                                         int32_t* status);

/**
 * Get compressor configuration from the PH.
 *
 * @param[in] handle  the PH handle
 * @param[out] status Error status variable. 0 on success.
 * @return compressor configuration
 */
HAL_REVPHCompressorConfigType HAL_GetREVPHCompressorConfig(
    HAL_REVPHHandle handle, int32_t* status);

/**
 * Returns the state of the digital pressure switch.
 *
 * @param[in] handle  the PH handle
 * @param[out] status Error status variable. 0 on success.
 * @return True if pressure switch indicates that the system is full,
 * otherwise false.
 */
HAL_Bool HAL_GetREVPHPressureSwitch(HAL_REVPHHandle handle, int32_t* status);

/**
 * Returns the current drawn by the compressor.
 *
 * @param[in] handle  the PH handle
 * @param[out] status Error status variable. 0 on success.
 * @return The current drawn by the compressor in amps.
 */
double HAL_GetREVPHCompressorCurrent(HAL_REVPHHandle handle, int32_t* status);

/**
 * Returns the raw voltage of the specified analog
 * input channel.
 *
 * @param[in] handle  the PH handle
 * @param[in] channel The analog input channel to read voltage from.
 * @param[out] status Error status variable. 0 on success.
 * @return The voltage of the specified analog input channel in volts.
 */
double HAL_GetREVPHAnalogVoltage(HAL_REVPHHandle handle, int32_t channel,
                                 int32_t* status);

/**
 * Returns the current input voltage for the PH.
 *
 * @param[in] handle  the PH handle
 * @param[out] status Error status variable. 0 on success.
 * @return The input voltage in volts.
 */
double HAL_GetREVPHVoltage(HAL_REVPHHandle handle, int32_t* status);

/**
 * Returns the current voltage of the regulated 5v supply.
 *
 * @param[in] handle  the PH handle
 * @param[out] status Error status variable. 0 on success.
 * @return The current voltage of the 5v supply in volts.
 */
double HAL_GetREVPH5VVoltage(HAL_REVPHHandle handle, int32_t* status);

/**
 * Returns the total current drawn by all solenoids.
 *
 * @param[in] handle  the PH handle
 * @param[out] status Error status variable. 0 on success.
 * @return Total current drawn by all solenoids in amps.
 */
double HAL_GetREVPHSolenoidCurrent(HAL_REVPHHandle handle, int32_t* status);

/**
 * Returns the current voltage of the solenoid power supply.
 *
 * @param[in] handle  the PH handle
 * @param[out] status Error status variable. 0 on success.
 * @return The current voltage of the solenoid power supply in volts.
 */
double HAL_GetREVPHSolenoidVoltage(HAL_REVPHHandle handle, int32_t* status);

/**
 * Returns the hardware and firmware versions of the PH.
 *
 * @param[in] handle  the PH handle
 * @param[out] version The hardware and firmware versions.
 * @param[out] status Error status variable. 0 on success.
 */
void HAL_GetREVPHVersion(HAL_REVPHHandle handle, HAL_REVPHVersion* version,
                         int32_t* status);

/**
 * Gets a bitmask of solenoid values.
 *
 * @param[in] handle  the PH handle
 * @param[out] status Error status variable. 0 on success.
 * @return Bitmask containing the state of the solenoids. The LSB represents
 * solenoid 0.
 */
int32_t HAL_GetREVPHSolenoids(HAL_REVPHHandle handle, int32_t* status);

/**
 * Sets solenoids on a PH.
 *
 * @param[in] handle  the PH handle
 * @param[in] mask Bitmask indicating which solenoids to set. The LSB represents
 * solenoid 0.
 * @param[in] values Bitmask indicating the desired states of the solenoids. The
 * LSB represents solenoid 0.
 * @param[out] status Error status variable. 0 on success.
 */
void HAL_SetREVPHSolenoids(HAL_REVPHHandle handle, int32_t mask, int32_t values,
                           int32_t* status);

/**
 * Fire a single solenoid shot for the specified duration.
 *
 * @param[in] handle  the PH handle
 * @param[in] index solenoid index
 * @param[in] durMs shot duration in ms
 * @param[out] status Error status variable. 0 on success.
 */
void HAL_FireREVPHOneShot(HAL_REVPHHandle handle, int32_t index, int32_t durMs,
                          int32_t* status);

/**
 * Returns the faults currently active on the PH.
 *
 * @param[in] handle  the PH handle
 * @param[out] faults The faults.
 * @param[out] status Error status variable. 0 on success.
 */
void HAL_GetREVPHFaults(HAL_REVPHHandle handle, HAL_REVPHFaults* faults,
                        int32_t* status);

/**
 * Returns the sticky faults currently active on this device.
 *
 * @param[in] handle  the PH handle
 * @param[out] stickyFaults The sticky faults.
 * @param[out] status Error status variable. 0 on success.
 */
void HAL_GetREVPHStickyFaults(HAL_REVPHHandle handle,
                              HAL_REVPHStickyFaults* stickyFaults,
                              int32_t* status);

/**
 * Clears the sticky faults.
 *
 * @param[in] handle  the PH handle
 * @param[out] status Error status variable. 0 on success.
 */
void HAL_ClearREVPHStickyFaults(HAL_REVPHHandle handle, int32_t* status);

/**
 * Get a bitmask of disabled solenoids.
 *
 * @param[in] handle  the PH handle
 * @param[out] status Error status variable. 0 on success.
 * @return Bitmask indicating disabled solenoids. The LSB represents solenoid 0.
 */
int32_t HAL_GetREVPHSolenoidDisabledList(HAL_REVPHHandle handle,
                                         int32_t* status);

#ifdef __cplusplus
}  // extern "C"
#endif
/** @} */
