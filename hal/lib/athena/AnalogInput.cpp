/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2016-2017. All Rights Reserved.                        */
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
#include "HAL/handles/HandlesInternal.h"
#include "PortsInternal.h"

using namespace hal;

extern "C" {
/**
 * Initialize the analog input port using the given port object.
 *
 * @param portHandle Handle to the port to initialize.
 */
HAL_AnalogInputHandle HAL_InitializeAnalogInputPort(HAL_PortHandle portHandle,
                                                    int32_t* status) {
  initializeAnalog(status);

  if (*status != 0) return HAL_kInvalidHandle;

  int16_t channel = getPortHandleChannel(portHandle);
  if (channel == InvalidHandleIndex) {
    *status = PARAMETER_OUT_OF_RANGE;
    return HAL_kInvalidHandle;
  }

  HAL_AnalogInputHandle handle = analogInputHandles.Allocate(channel, status);

  if (*status != 0)
    return HAL_kInvalidHandle;  // failed to allocate. Pass error back.

  // Initialize port structure
  auto analog_port = analogInputHandles.Get(handle);
  if (analog_port == nullptr) {  // would only error on thread issue
    *status = HAL_HANDLE_ERROR;
    return HAL_kInvalidHandle;
  }
  analog_port->channel = static_cast<uint8_t>(channel);
  if (HAL_IsAccumulatorChannel(handle, status)) {
    analog_port->accumulator.reset(tAccumulator::create(channel, status));
  } else {
    analog_port->accumulator = nullptr;
  }

  // Set default configuration
  analogInputSystem->writeScanList(channel, channel, status);
  HAL_SetAnalogAverageBits(handle, kDefaultAverageBits, status);
  HAL_SetAnalogOversampleBits(handle, kDefaultOversampleBits, status);
  return handle;
}

/**
 * @param analogPortHandle Handle to the analog port.
 */
void HAL_FreeAnalogInputPort(HAL_AnalogInputHandle analogPortHandle) {
  // no status, so no need to check for a proper free.
  analogInputHandles.Free(analogPortHandle);
}

/**
 * Check that the analog module number is valid.
 *
 * @param module The analog module number.
 * @return Analog module is valid and present
 */
HAL_Bool HAL_CheckAnalogModule(int32_t module) { return module == 1; }

/**
 * Check that the analog output channel number is value.
 * Verify that the analog channel number is one of the legal channel numbers.
 * Channel numbers are 0-based.
 *
 * @param channel The analog output channel number.
 * @return Analog channel is valid
 */
HAL_Bool HAL_CheckAnalogInputChannel(int32_t channel) {
  return channel < kNumAnalogInputs && channel >= 0;
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
  // TODO: Need double comparison with epsilon.
  // wpi_assert(!sampleRateSet || GetSampleRate() == samplesPerSecond);
  initializeAnalog(status);
  if (*status != 0) return;
  setAnalogSampleRate(samplesPerSecond, status);
}

/**
 * Get the current sample rate.
 *
 * This assumes one entry in the scan list.
 * This is a global setting for the Athena and effects all channels.
 *
 * @return Sample rate.
 */
double HAL_GetAnalogSampleRate(int32_t* status) {
  initializeAnalog(status);
  if (*status != 0) return 0;
  uint32_t ticksPerConversion = analogInputSystem->readLoopTiming(status);
  uint32_t ticksPerSample =
      ticksPerConversion * getAnalogNumActiveChannels(status);
  return static_cast<double>(kTimebase) / static_cast<double>(ticksPerSample);
}

/**
 * Set the number of averaging bits.
 *
 * This sets the number of averaging bits. The actual number of averaged samples
 * is 2**bits. Use averaging to improve the stability of your measurement at the
 * expense of sampling rate. The averaging is done automatically in the FPGA.
 *
 * @param analogPortHandle Handle to the analog port to configure.
 * @param bits Number of bits to average.
 */
void HAL_SetAnalogAverageBits(HAL_AnalogInputHandle analogPortHandle,
                              int32_t bits, int32_t* status) {
  auto port = analogInputHandles.Get(analogPortHandle);
  if (port == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return;
  }
  analogInputSystem->writeAverageBits(port->channel, static_cast<uint8_t>(bits),
                                      status);
}

/**
 * Get the number of averaging bits.
 *
 * This gets the number of averaging bits from the FPGA. The actual number of
 * averaged samples is 2**bits. The averaging is done automatically in the FPGA.
 *
 * @param analogPortHandle Handle to the analog port to use.
 * @return Bits to average.
 */
int32_t HAL_GetAnalogAverageBits(HAL_AnalogInputHandle analogPortHandle,
                                 int32_t* status) {
  auto port = analogInputHandles.Get(analogPortHandle);
  if (port == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return kDefaultAverageBits;
  }
  uint8_t result = analogInputSystem->readAverageBits(port->channel, status);
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
 * @param analogPortHandle Handle to the analog port to use.
 * @param bits Number of bits to oversample.
 */
void HAL_SetAnalogOversampleBits(HAL_AnalogInputHandle analogPortHandle,
                                 int32_t bits, int32_t* status) {
  auto port = analogInputHandles.Get(analogPortHandle);
  if (port == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return;
  }
  analogInputSystem->writeOversampleBits(port->channel,
                                         static_cast<uint8_t>(bits), status);
}

/**
 * Get the number of oversample bits.
 *
 * This gets the number of oversample bits from the FPGA. The actual number of
 * oversampled values is 2**bits. The oversampling is done automatically in the
 * FPGA.
 *
 * @param analogPortHandle Handle to the analog port to use.
 * @return Bits to oversample.
 */
int32_t HAL_GetAnalogOversampleBits(HAL_AnalogInputHandle analogPortHandle,
                                    int32_t* status) {
  auto port = analogInputHandles.Get(analogPortHandle);
  if (port == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return kDefaultOversampleBits;
  }
  uint8_t result = analogInputSystem->readOversampleBits(port->channel, status);
  return result;
}

/**
 * Get a sample straight from the channel on this module.
 *
 * The sample is a 12-bit value representing the 0V to 5V range of the A/D
 * converter in the module. The units are in A/D converter codes.  Use
 * GetVoltage() to get the analog value in calibrated units.
 *
 * @param analogPortHandle Handle to the analog port to use.
 * @return A sample straight from the channel on this module.
 */
int32_t HAL_GetAnalogValue(HAL_AnalogInputHandle analogPortHandle,
                           int32_t* status) {
  auto port = analogInputHandles.Get(analogPortHandle);
  if (port == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return 0;
  }

  tAI::tReadSelect readSelect;
  readSelect.Channel = port->channel;
  readSelect.Averaged = false;

  std::lock_guard<priority_recursive_mutex> sync(analogRegisterWindowMutex);
  analogInputSystem->writeReadSelect(readSelect, status);
  analogInputSystem->strobeLatchOutput(status);
  return static_cast<int16_t>(analogInputSystem->readOutput(status));
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
 * @param analogPortHandle Handle to the analog port to use.
 * @return A sample from the oversample and average engine for the channel.
 */
int32_t HAL_GetAnalogAverageValue(HAL_AnalogInputHandle analogPortHandle,
                                  int32_t* status) {
  auto port = analogInputHandles.Get(analogPortHandle);
  if (port == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return 0;
  }
  tAI::tReadSelect readSelect;
  readSelect.Channel = port->channel;
  readSelect.Averaged = true;

  std::lock_guard<priority_recursive_mutex> sync(analogRegisterWindowMutex);
  analogInputSystem->writeReadSelect(readSelect, status);
  analogInputSystem->strobeLatchOutput(status);
  return static_cast<int32_t>(analogInputSystem->readOutput(status));
}

/**
 * Get a scaled sample straight from the channel on this module.
 *
 * The value is scaled to units of Volts using the calibrated scaling data from
 * GetLSBWeight() and GetOffset().
 *
 * @param analogPortHandle Handle to the analog port to use.
 * @return A scaled sample straight from the channel on this module.
 */
double HAL_GetAnalogVoltage(HAL_AnalogInputHandle analogPortHandle,
                            int32_t* status) {
  int32_t value = HAL_GetAnalogValue(analogPortHandle, status);
  int32_t LSBWeight = HAL_GetAnalogLSBWeight(analogPortHandle, status);
  int32_t offset = HAL_GetAnalogOffset(analogPortHandle, status);
  double voltage = LSBWeight * 1.0e-9 * value - offset * 1.0e-9;
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
 * @param analogPortHandle Handle to the analog port to use.
 * @return A scaled sample from the output of the oversample and average engine
 * for the channel.
 */
double HAL_GetAnalogAverageVoltage(HAL_AnalogInputHandle analogPortHandle,
                                   int32_t* status) {
  int32_t value = HAL_GetAnalogAverageValue(analogPortHandle, status);
  int32_t LSBWeight = HAL_GetAnalogLSBWeight(analogPortHandle, status);
  int32_t offset = HAL_GetAnalogOffset(analogPortHandle, status);
  int32_t oversampleBits =
      HAL_GetAnalogOversampleBits(analogPortHandle, status);
  double voltage =
      LSBWeight * 1.0e-9 * value / static_cast<double>(1 << oversampleBits) -
      offset * 1.0e-9;
  return voltage;
}

/**
 * Convert a voltage to a raw value for a specified channel.
 *
 * This process depends on the calibration of each channel, so the channel must
 * be specified.
 *
 * @todo This assumes raw values.  Oversampling not supported as is.
 *
 * @param analogPortHandle Handle to the analog port to use.
 * @param voltage The voltage to convert.
 * @return The raw value for the channel.
 */
int32_t HAL_GetAnalogVoltsToValue(HAL_AnalogInputHandle analogPortHandle,
                                  double voltage, int32_t* status) {
  if (voltage > 5.0) {
    voltage = 5.0;
    *status = VOLTAGE_OUT_OF_RANGE;
  }
  if (voltage < 0.0) {
    voltage = 0.0;
    *status = VOLTAGE_OUT_OF_RANGE;
  }
  int32_t LSBWeight = HAL_GetAnalogLSBWeight(analogPortHandle, status);
  int32_t offset = HAL_GetAnalogOffset(analogPortHandle, status);
  int32_t value =
      static_cast<int32_t>((voltage + offset * 1.0e-9) / (LSBWeight * 1.0e-9));
  return value;
}

/**
 * Get the factory scaling least significant bit weight constant.
 * The least significant bit weight constant for the channel that was calibrated
 * in manufacturing and stored in an eeprom in the module.
 *
 * Volts = ((LSB_Weight * 1e-9) * raw) - (Offset * 1e-9)
 *
 * @param analogPortHandle Handle to the analog port to use.
 * @return Least significant bit weight.
 */
int32_t HAL_GetAnalogLSBWeight(HAL_AnalogInputHandle analogPortHandle,
                               int32_t* status) {
  auto port = analogInputHandles.Get(analogPortHandle);
  if (port == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return 0;
  }
  int32_t lsbWeight = FRC_NetworkCommunication_nAICalibration_getLSBWeight(
      0, port->channel, status);  // XXX: aiSystemIndex == 0?
  return lsbWeight;
}

/**
 * Get the factory scaling offset constant.
 * The offset constant for the channel that was calibrated in manufacturing and
 * stored in an eeprom in the module.
 *
 * Volts = ((LSB_Weight * 1e-9) * raw) - (Offset * 1e-9)
 *
 * @param analogPortHandle Handle to the analog port to use.
 * @return Offset constant.
 */
int32_t HAL_GetAnalogOffset(HAL_AnalogInputHandle analogPortHandle,
                            int32_t* status) {
  auto port = analogInputHandles.Get(analogPortHandle);
  if (port == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return 0;
  }
  int32_t offset = FRC_NetworkCommunication_nAICalibration_getOffset(
      0, port->channel, status);  // XXX: aiSystemIndex == 0?
  return offset;
}
}
