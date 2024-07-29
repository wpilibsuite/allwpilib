// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <stdint.h>

#include "hal/Types.h"

/**
 * @defgroup hal_analoginput Analog Input Functions
 * @ingroup hal_capi
 * @{
 */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Initializes the analog input port using the given port object.
 *
 * @param[in] portHandle Handle to the port to initialize.
 * @param[in] allocationLocation the location where the allocation is occurring
 *                               (can be null)
 * @param[out] status the error code, or 0 for success
 * @return the created analog input handle
 */
HAL_AnalogInputHandle HAL_InitializeAnalogInputPort(
    HAL_PortHandle portHandle, const char* allocationLocation, int32_t* status);

/**
 * Frees an analog input port.
 *
 * @param[in,out] analogPortHandle Handle to the analog port.
 */
void HAL_FreeAnalogInputPort(HAL_AnalogInputHandle analogPortHandle);

/**
 * Checks that the analog module number is valid.
 *
 * @param[in] module The analog module number.
 * @return Analog module is valid and present
 */
HAL_Bool HAL_CheckAnalogModule(int32_t module);

/**
 * Checks that the analog output channel number is valid.
 * Verifies that the analog channel number is one of the legal channel numbers.
 * Channel numbers are 0-based.
 *
 * @param[in] channel The analog output channel number.
 * @return Analog channel is valid
 */
HAL_Bool HAL_CheckAnalogInputChannel(int32_t channel);

/**
 * Indicates the analog input is used by a simulated device.
 *
 * @param handle the analog input handle
 * @param device simulated device handle
 */
void HAL_SetAnalogInputSimDevice(HAL_AnalogInputHandle handle,
                                 HAL_SimDeviceHandle device);

/**
 * Sets the sample rate.
 *
 * This is a global setting for the Athena and effects all channels.
 *
 * @param[in] samplesPerSecond The number of samples per channel per second.
 * @param[out] status          the error code, or 0 for success
 */
void HAL_SetAnalogSampleRate(double samplesPerSecond, int32_t* status);

/**
 * Gets the current sample rate.
 *
 * This assumes one entry in the scan list.
 * This is a global setting for the Athena and effects all channels.
 *
 * @param[out] status the error code, or 0 for success
 * @return Sample rate.
 */
double HAL_GetAnalogSampleRate(int32_t* status);

/**
 * Sets the number of averaging bits.
 *
 * This sets the number of averaging bits. The actual number of averaged samples
 * is 2**bits. Use averaging to improve the stability of your measurement at the
 * expense of sampling rate. The averaging is done automatically in the FPGA.
 *
 * @param[in] analogPortHandle Handle to the analog port to configure.
 * @param[in] bits Number of bits to average.
 * @param[out] status the error code, or 0 for success
 */
void HAL_SetAnalogAverageBits(HAL_AnalogInputHandle analogPortHandle,
                              int32_t bits, int32_t* status);

/**
 * Gets the number of averaging bits.
 *
 * This gets the number of averaging bits from the FPGA. The actual number of
 * averaged samples is 2**bits. The averaging is done automatically in the FPGA.
 *
 * @param[in] analogPortHandle Handle to the analog port to use.
 * @param[out] status the error code, or 0 for success
 * @return Bits to average.
 */
int32_t HAL_GetAnalogAverageBits(HAL_AnalogInputHandle analogPortHandle,
                                 int32_t* status);

/**
 * Sets the number of oversample bits.
 *
 * This sets the number of oversample bits. The actual number of oversampled
 * values is 2**bits. Use oversampling to improve the resolution of your
 * measurements at the expense of sampling rate. The oversampling is done
 * automatically in the FPGA.
 *
 * @param[in] analogPortHandle Handle to the analog port to use.
 * @param[in] bits Number of bits to oversample.
 * @param[out] status the error code, or 0 for success
 */
void HAL_SetAnalogOversampleBits(HAL_AnalogInputHandle analogPortHandle,
                                 int32_t bits, int32_t* status);

/**
 * Gets the number of oversample bits.
 *
 * This gets the number of oversample bits from the FPGA. The actual number of
 * oversampled values is 2**bits. The oversampling is done automatically in the
 * FPGA.
 *
 * @param[in] analogPortHandle Handle to the analog port to use.
 * @param[out] status          the error code, or 0 for success
 * @return Bits to oversample.
 */
int32_t HAL_GetAnalogOversampleBits(HAL_AnalogInputHandle analogPortHandle,
                                    int32_t* status);

/**
 * Gets a sample straight from the channel on this module.
 *
 * The sample is a 12-bit value representing the 0V to 5V range of the A/D
 * converter in the module. The units are in A/D converter codes.  Use
 * GetVoltage() to get the analog value in calibrated units.
 *
 * @param[in] analogPortHandle Handle to the analog port to use.
 * @param[out] status the error code, or 0 for success
 * @return A sample straight from the channel on this module.
 */
int32_t HAL_GetAnalogValue(HAL_AnalogInputHandle analogPortHandle,
                           int32_t* status);

/**
 * Gets a sample from the output of the oversample and average engine for the
 * channel.
 *
 * The sample is 12-bit + the value configured in SetOversampleBits().
 * The value configured in SetAverageBits() will cause this value to be averaged
 * 2**bits number of samples. This is not a sliding window.  The sample will not
 * change until 2**(OversampleBits + AverageBits) samples have been acquired
 * from the module on this channel. Use GetAverageVoltage() to get the analog
 * value in calibrated units.
 *
 * @param[in] analogPortHandle Handle to the analog port to use.
 * @param[out] status the error code, or 0 for success
 * @return A sample from the oversample and average engine for the channel.
 */
int32_t HAL_GetAnalogAverageValue(HAL_AnalogInputHandle analogPortHandle,
                                  int32_t* status);

/**
 * Converts a voltage to a raw value for a specified channel.
 *
 * This process depends on the calibration of each channel, so the channel must
 * be specified.
 *
 * @todo This assumes raw values.  Oversampling not supported as is.
 *
 * @param[in] analogPortHandle Handle to the analog port to use.
 * @param[in] voltage The voltage to convert.
 * @param[out] status the error code, or 0 for success
 * @return The raw value for the channel.
 */
int32_t HAL_GetAnalogVoltsToValue(HAL_AnalogInputHandle analogPortHandle,
                                  double voltage, int32_t* status);

/**
 * Gets a scaled sample straight from the channel on this module.
 *
 * The value is scaled to units of Volts using the calibrated scaling data from
 * GetLSBWeight() and GetOffset().
 *
 * @param[in] analogPortHandle Handle to the analog port to use.
 * @param[out] status the error code, or 0 for success
 * @return A scaled sample straight from the channel on this module.
 */
double HAL_GetAnalogVoltage(HAL_AnalogInputHandle analogPortHandle,
                            int32_t* status);

/**
 * Gets a scaled sample from the output of the oversample and average engine for
 * the channel.
 *
 * The value is scaled to units of Volts using the calibrated scaling data from
 * GetLSBWeight() and GetOffset(). Using oversampling will cause this value to
 * be higher resolution, but it will update more slowly. Using averaging will
 * cause this value to be more stable, but it will update more slowly.
 *
 * @param[in] analogPortHandle Handle to the analog port to use.
 * @param[out] status the error code, or 0 for success
 * @return A scaled sample from the output of the oversample and average engine
 * for the channel.
 */
double HAL_GetAnalogAverageVoltage(HAL_AnalogInputHandle analogPortHandle,
                                   int32_t* status);

/**
 * Gets the factory scaling least significant bit weight constant.
 * The least significant bit weight constant for the channel that was calibrated
 * in manufacturing and stored in an eeprom in the module.
 *
 * Volts = ((LSB_Weight * 1e-9) * raw) - (Offset * 1e-9)
 *
 * @param[in] analogPortHandle Handle to the analog port to use.
 * @param[out] status the error code, or 0 for success
 * @return Least significant bit weight.
 */
int32_t HAL_GetAnalogLSBWeight(HAL_AnalogInputHandle analogPortHandle,
                               int32_t* status);

/**
 * Gets the factory scaling offset constant.
 * The offset constant for the channel that was calibrated in manufacturing and
 * stored in an eeprom in the module.
 *
 * Volts = ((LSB_Weight * 1e-9) * raw) - (Offset * 1e-9)
 *
 * @param[in] analogPortHandle Handle to the analog port to use.
 * @param[out] status Error status variable. 0 on success.
 * @return Offset constant.
 */
int32_t HAL_GetAnalogOffset(HAL_AnalogInputHandle analogPortHandle,
                            int32_t* status);

/**
 *  Get the analog voltage from a raw value.
 *
 * @param[in] analogPortHandle  Handle to the analog port the values were read
 *                              from.
 * @param[in] rawValue          The raw analog value
 * @param[out] status           Error status variable. 0 on success.
 * @return The voltage relating to the value
 */
double HAL_GetAnalogValueToVolts(HAL_AnalogInputHandle analogPortHandle,
                                 int32_t rawValue, int32_t* status);
#ifdef __cplusplus
}  // extern "C"
#endif
/** @} */
