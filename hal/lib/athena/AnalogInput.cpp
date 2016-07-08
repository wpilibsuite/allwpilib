/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2016. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "HAL/AnalogInput.h"

#include <mutex>

#include "AnalogInternal.h"
#include "FRC_NetworkCommunication/AICalibration.h"
#include "HAL/AnalogAccumulator.h"
#include "HAL/HAL.h"
#include "HAL/cpp/priority_mutex.h"
#include "PortsInternal.h"
#include "handles/HandlesInternal.h"

using namespace hal;

static bool analogSampleRateSet = false;

extern "C" {
/**
 * Initialize the analog input port using the given port object.
 */
HAL_AnalogInputHandle HAL_InitializeAnalogInputPort(HAL_PortHandle port_handle,
                                                    int32_t* status) {
  initializeAnalog(status);

  if (*status != 0) return HAL_kInvalidHandle;

  int16_t pin = getPortHandlePin(port_handle);
  if (pin == InvalidHandleIndex) {
    *status = PARAMETER_OUT_OF_RANGE;
    return HAL_kInvalidHandle;
  }

  HAL_AnalogInputHandle handle = analogInputHandles.Allocate(pin, status);

  if (*status != 0)
    return HAL_kInvalidHandle;  // failed to allocate. Pass error back.

  // Initialize port structure
  auto analog_port = analogInputHandles.Get(handle);
  if (analog_port == nullptr) {  // would only error on thread issue
    *status = HAL_HANDLE_ERROR;
    return HAL_kInvalidHandle;
  }
  analog_port->pin = static_cast<uint8_t>(pin);
  if (HAL_IsAccumulatorChannel(handle, status)) {
    analog_port->accumulator = tAccumulator::create(pin, status);
  } else
    analog_port->accumulator = nullptr;

  // Set default configuration
  analogInputSystem->writeScanList(pin, pin, status);
  HAL_SetAnalogAverageBits(handle, kDefaultAverageBits, status);
  HAL_SetAnalogOversampleBits(handle, kDefaultOversampleBits, status);
  return handle;
}

void HAL_FreeAnalogInputPort(HAL_AnalogInputHandle analog_port_handle) {
  auto port = analogInputHandles.Get(analog_port_handle);
  if (!port) return;
  // no status, so no need to check for a proper free.
  analogInputHandles.Free(analog_port_handle);
  delete port->accumulator;
}

/**
 * Check that the analog module number is valid.
 *
 * @return Analog module is valid and present
 */
bool HAL_CheckAnalogModule(uint8_t module) { return module == 1; }

/**
 * Check that the analog output channel number is value.
 * Verify that the analog channel number is one of the legal channel numbers.
 * Channel numbers are 0-based.
 *
 * @return Analog channel is valid
 */
bool HAL_CheckAnalogInputChannel(uint32_t pin) {
  if (pin < kNumAnalogInputs) return true;
  return false;
}

/**
 * Set the sample rate.
 *
 * This is a global setting for the Athena and effects all channels.
 *
 * @param samplesPerSecond The number of samples per channel per second.
 */
void HAL_SetAnalogSampleRate(double samplesPerSecond, int32_t* status) {
  // TODO: This will change when variable size scan lists are implemented.
  // TODO: Need float comparison with epsilon.
  // wpi_assert(!sampleRateSet || GetSampleRate() == samplesPerSecond);
  analogSampleRateSet = true;

  // Compute the convert rate
  uint32_t ticksPerSample = (uint32_t)((float)kTimebase / samplesPerSecond);
  uint32_t ticksPerConversion =
      ticksPerSample / getAnalogNumChannelsToActivate(status);
  // ticksPerConversion must be at least 80
  if (ticksPerConversion < 80) {
    if ((*status) >= 0) *status = SAMPLE_RATE_TOO_HIGH;
    ticksPerConversion = 80;
  }

  // Atomically set the scan size and the convert rate so that the sample rate
  // is constant
  tAI::tConfig config;
  config.ScanSize = getAnalogNumChannelsToActivate(status);
  config.ConvertRate = ticksPerConversion;
  analogInputSystem->writeConfig(config, status);

  // Indicate that the scan size has been commited to hardware.
  setAnalogNumChannelsToActivate(0);
}

/**
 * Get the current sample rate.
 *
 * This assumes one entry in the scan list.
 * This is a global setting for the Athena and effects all channels.
 *
 * @return Sample rate.
 */
float HAL_GetAnalogSampleRate(int32_t* status) {
  uint32_t ticksPerConversion = analogInputSystem->readLoopTiming(status);
  uint32_t ticksPerSample =
      ticksPerConversion * getAnalogNumActiveChannels(status);
  return (float)kTimebase / (float)ticksPerSample;
}

/**
 * Set the number of averaging bits.
 *
 * This sets the number of averaging bits. The actual number of averaged samples
 * is 2**bits. Use averaging to improve the stability of your measurement at the
 * expense of sampling rate. The averaging is done automatically in the FPGA.
 *
 * @param analog_port_pointer Pointer to the analog port to configure.
 * @param bits Number of bits to average.
 */
void HAL_SetAnalogAverageBits(HAL_AnalogInputHandle analog_port_handle,
                              uint32_t bits, int32_t* status) {
  auto port = analogInputHandles.Get(analog_port_handle);
  if (port == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return;
  }
  analogInputSystem->writeAverageBits(port->pin, bits, status);
}

/**
 * Get the number of averaging bits.
 *
 * This gets the number of averaging bits from the FPGA. The actual number of
 * averaged samples is 2**bits. The averaging is done automatically in the FPGA.
 *
 * @param analog_port_pointer Pointer to the analog port to use.
 * @return Bits to average.
 */
uint32_t HAL_GetAnalogAverageBits(HAL_AnalogInputHandle analog_port_handle,
                                  int32_t* status) {
  auto port = analogInputHandles.Get(analog_port_handle);
  if (port == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return kDefaultAverageBits;
  }
  uint32_t result = analogInputSystem->readAverageBits(port->pin, status);
  return result;
}

/**
 * Set the number of oversample bits.
 *
 * This sets the number of oversample bits. The actual number of oversampled
 * values is 2**bits. Use oversampling to improve the resolution of your
 * measurements at the expense of sampling rate. The oversampling is done
 * automatically in the FPGA.
 *
 * @param analog_port_pointer Pointer to the analog port to use.
 * @param bits Number of bits to oversample.
 */
void HAL_SetAnalogOversampleBits(HAL_AnalogInputHandle analog_port_handle,
                                 uint32_t bits, int32_t* status) {
  auto port = analogInputHandles.Get(analog_port_handle);
  if (port == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return;
  }
  analogInputSystem->writeOversampleBits(port->pin, bits, status);
}

/**
 * Get the number of oversample bits.
 *
 * This gets the number of oversample bits from the FPGA. The actual number of
 * oversampled values is 2**bits. The oversampling is done automatically in the
 * FPGA.
 *
 * @param analog_port_pointer Pointer to the analog port to use.
 * @return Bits to oversample.
 */
uint32_t HAL_GetAnalogOversampleBits(HAL_AnalogInputHandle analog_port_handle,
                                     int32_t* status) {
  auto port = analogInputHandles.Get(analog_port_handle);
  if (port == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return kDefaultOversampleBits;
  }
  uint32_t result = analogInputSystem->readOversampleBits(port->pin, status);
  return result;
}

/**
 * Get a sample straight from the channel on this module.
 *
 * The sample is a 12-bit value representing the 0V to 5V range of the A/D
 * converter in the module. The units are in A/D converter codes.  Use
 * GetVoltage() to get the analog value in calibrated units.
 *
 * @param analog_port_pointer Pointer to the analog port to use.
 * @return A sample straight from the channel on this module.
 */
int16_t HAL_GetAnalogValue(HAL_AnalogInputHandle analog_port_handle,
                           int32_t* status) {
  auto port = analogInputHandles.Get(analog_port_handle);
  if (port == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return 0;
  }
  int16_t value;
  if (!HAL_CheckAnalogInputChannel(port->pin)) {
    return 0;
  }

  tAI::tReadSelect readSelect;
  readSelect.Channel = port->pin;
  readSelect.Averaged = false;

  {
    std::lock_guard<priority_recursive_mutex> sync(analogRegisterWindowMutex);
    analogInputSystem->writeReadSelect(readSelect, status);
    analogInputSystem->strobeLatchOutput(status);
    value = (int16_t)analogInputSystem->readOutput(status);
  }

  return value;
}

/**
 * Get a sample from the output of the oversample and average engine for the
 * channel.
 *
 * The sample is 12-bit + the value configured in SetOversampleBits().
 * The value configured in SetAverageBits() will cause this value to be averaged
 * 2**bits number of samples. This is not a sliding window.  The sample will not
 * change until 2**(OversamplBits + AverageBits) samples have been acquired from
 * the module on this channel. Use GetAverageVoltage() to get the analog value
 * in calibrated units.
 *
 * @param analog_port_pointer Pointer to the analog port to use.
 * @return A sample from the oversample and average engine for the channel.
 */
int32_t HAL_GetAnalogAverageValue(HAL_AnalogInputHandle analog_port_handle,
                                  int32_t* status) {
  auto port = analogInputHandles.Get(analog_port_handle);
  if (port == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return 0;
  }
  int32_t value;
  if (!HAL_CheckAnalogInputChannel(port->pin)) {
    return 0;
  }

  tAI::tReadSelect readSelect;
  readSelect.Channel = port->pin;
  readSelect.Averaged = true;

  {
    std::lock_guard<priority_recursive_mutex> sync(analogRegisterWindowMutex);
    analogInputSystem->writeReadSelect(readSelect, status);
    analogInputSystem->strobeLatchOutput(status);
    value = (int32_t)analogInputSystem->readOutput(status);
  }

  return value;
}

/**
 * Get a scaled sample straight from the channel on this module.
 *
 * The value is scaled to units of Volts using the calibrated scaling data from
 * GetLSBWeight() and GetOffset().
 *
 * @param analog_port_pointer Pointer to the analog port to use.
 * @return A scaled sample straight from the channel on this module.
 */
float HAL_GetAnalogVoltage(HAL_AnalogInputHandle analog_port_handle,
                           int32_t* status) {
  int16_t value = HAL_GetAnalogValue(analog_port_handle, status);
  uint32_t LSBWeight = HAL_GetAnalogLSBWeight(analog_port_handle, status);
  int32_t offset = HAL_GetAnalogOffset(analog_port_handle, status);
  float voltage = LSBWeight * 1.0e-9 * value - offset * 1.0e-9;
  return voltage;
}

/**
 * Get a scaled sample from the output of the oversample and average engine for
 * the channel.
 *
 * The value is scaled to units of Volts using the calibrated scaling data from
 * GetLSBWeight() and GetOffset(). Using oversampling will cause this value to
 * be higher resolution, but it will update more slowly. Using averaging will
 * cause this value to be more stable, but it will update more slowly.
 *
 * @param analog_port_pointer Pointer to the analog port to use.
 * @return A scaled sample from the output of the oversample and average engine
 * for the channel.
 */
float HAL_GetAnalogAverageVoltage(HAL_AnalogInputHandle analog_port_handle,
                                  int32_t* status) {
  int32_t value = HAL_GetAnalogAverageValue(analog_port_handle, status);
  uint32_t LSBWeight = HAL_GetAnalogLSBWeight(analog_port_handle, status);
  int32_t offset = HAL_GetAnalogOffset(analog_port_handle, status);
  uint32_t oversampleBits =
      HAL_GetAnalogOversampleBits(analog_port_handle, status);
  float voltage =
      ((LSBWeight * 1.0e-9 * value) / (float)(1 << oversampleBits)) -
      offset * 1.0e-9;
  return voltage;
}

/**
 * Convert a voltage to a raw value for a specified channel.
 *
 * This process depends on the calibration of each channel, so the channel
 * must be specified.
 *
 * @todo This assumes raw values.  Oversampling not supported as is.
 *
 * @param analog_port_pointer Pointer to the analog port to use.
 * @param voltage The voltage to convert.
 * @return The raw value for the channel.
 */
int32_t HAL_GetAnalogVoltsToValue(HAL_AnalogInputHandle analog_port_handle,
                                  double voltage, int32_t* status) {
  if (voltage > 5.0) {
    voltage = 5.0;
    *status = VOLTAGE_OUT_OF_RANGE;
  }
  if (voltage < 0.0) {
    voltage = 0.0;
    *status = VOLTAGE_OUT_OF_RANGE;
  }
  uint32_t LSBWeight = HAL_GetAnalogLSBWeight(analog_port_handle, status);
  int32_t offset = HAL_GetAnalogOffset(analog_port_handle, status);
  int32_t value = (int32_t)((voltage + offset * 1.0e-9) / (LSBWeight * 1.0e-9));
  return value;
}

/**
 * Get the factory scaling least significant bit weight constant.
 * The least significant bit weight constant for the channel that was calibrated
 * in manufacturing and stored in an eeprom in the module.
 *
 * Volts = ((LSB_Weight * 1e-9) * raw) - (Offset * 1e-9)
 *
 * @param analog_port_pointer Pointer to the analog port to use.
 * @return Least significant bit weight.
 */
uint32_t HAL_GetAnalogLSBWeight(HAL_AnalogInputHandle analog_port_handle,
                                int32_t* status) {
  auto port = analogInputHandles.Get(analog_port_handle);
  if (port == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return 0;
  }
  uint32_t lsbWeight = FRC_NetworkCommunication_nAICalibration_getLSBWeight(
      0, port->pin, status);  // XXX: aiSystemIndex == 0?
  return lsbWeight;
}

/**
 * Get the factory scaling offset constant.
 * The offset constant for the channel that was calibrated in manufacturing and
 * stored in an eeprom in the module.
 *
 * Volts = ((LSB_Weight * 1e-9) * raw) - (Offset * 1e-9)
 *
 * @param analog_port_pointer Pointer to the analog port to use.
 * @return Offset constant.
 */
int32_t HAL_GetAnalogOffset(HAL_AnalogInputHandle analog_port_handle,
                            int32_t* status) {
  auto port = analogInputHandles.Get(analog_port_handle);
  if (port == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return 0;
  }
  int32_t offset = FRC_NetworkCommunication_nAICalibration_getOffset(
      0, port->pin, status);  // XXX: aiSystemIndex == 0?
  return offset;
}
}
