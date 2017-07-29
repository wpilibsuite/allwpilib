/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2016-2017. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "HAL/DIO.h"

#include <cmath>

#include "DigitalInternal.h"
#include "HAL/handles/HandlesInternal.h"
#include "HAL/handles/LimitedHandleResource.h"
#include "PortsInternal.h"

// Create a mutex to protect changes to the digital output values
static hal::priority_recursive_mutex digitalDIOMutex;

static hal::LimitedHandleResource<HAL_DigitalPWMHandle, uint8_t,
                                  hal::kNumDigitalPWMOutputs,
                                  hal::HAL_HandleEnum::DigitalPWM>
    digitalPWMHandles;

extern "C" {

/**
 * Create a new instance of a digital port.
 */
HAL_DigitalHandle HAL_InitializeDIOPort(HAL_PortHandle portHandle,
                                        HAL_Bool input, int32_t* status) {
  hal::initializeDigital(status);

  if (*status != 0) return HAL_kInvalidHandle;

  int16_t channel = hal::getPortHandleChannel(portHandle);
  if (channel == hal::InvalidHandleIndex ||
      channel >= hal::kNumDigitalChannels) {
    *status = PARAMETER_OUT_OF_RANGE;
    return HAL_kInvalidHandle;
  }

  auto handle = hal::digitalChannelHandles.Allocate(
      channel, hal::HAL_HandleEnum::DIO, status);

  if (*status != 0)
    return HAL_kInvalidHandle;  // failed to allocate. Pass error back.

  auto port = hal::digitalChannelHandles.Get(handle, hal::HAL_HandleEnum::DIO);
  if (port == nullptr) {  // would only occur on thread issue.
    *status = HAL_HANDLE_ERROR;
    return HAL_kInvalidHandle;
  }

  port->channel = static_cast<uint8_t>(channel);

  std::lock_guard<hal::priority_recursive_mutex> sync(digitalDIOMutex);

  hal::tDIO::tOutputEnable outputEnable =
      hal::digitalSystem->readOutputEnable(status);

  if (port->channel >= hal::kNumDigitalHeaders + hal::kNumDigitalMXPChannels) {
    if (!hal::getPortHandleSPIEnable(portHandle)) {
      // if this flag is not set, we actually want DIO.
      uint32_t bitToSet = 1u << hal::remapSPIChannel(port->channel);

      uint16_t specialFunctions = hal::spiSystem->readEnableDIO(status);
      // Set the field to enable SPI DIO
      hal::spiSystem->writeEnableDIO(specialFunctions | bitToSet, status);

      if (input) {
        outputEnable.SPIPort =
            outputEnable.SPIPort & (~bitToSet);  // clear the field for read
      } else {
        outputEnable.SPIPort =
            outputEnable.SPIPort | bitToSet;  // set the bits for write
      }
    }
  } else if (port->channel < hal::kNumDigitalHeaders) {
    uint32_t bitToSet = 1u << port->channel;
    if (input) {
      outputEnable.Headers =
          outputEnable.Headers & (~bitToSet);  // clear the bit for read
    } else {
      outputEnable.Headers =
          outputEnable.Headers | bitToSet;  // set the bit for write
    }
  } else {
    uint32_t bitToSet = 1u << hal::remapMXPChannel(port->channel);

    uint16_t specialFunctions =
        hal::digitalSystem->readEnableMXPSpecialFunction(status);
    hal::digitalSystem->writeEnableMXPSpecialFunction(
        specialFunctions & ~bitToSet, status);

    if (input) {
      outputEnable.MXP =
          outputEnable.MXP & (~bitToSet);  // clear the bit for read
    } else {
      outputEnable.MXP = outputEnable.MXP | bitToSet;  // set the bit for write
    }
  }

  hal::digitalSystem->writeOutputEnable(outputEnable, status);

  return handle;
}

HAL_Bool HAL_CheckDIOChannel(int32_t channel) {
  return channel < hal::kNumDigitalChannels && channel >= 0;
}

void HAL_FreeDIOPort(HAL_DigitalHandle dioPortHandle) {
  auto port =
      hal::digitalChannelHandles.Get(dioPortHandle, hal::HAL_HandleEnum::DIO);
  // no status, so no need to check for a proper free.
  hal::digitalChannelHandles.Free(dioPortHandle, hal::HAL_HandleEnum::DIO);
  if (port == nullptr) return;
  int32_t status = 0;
  std::lock_guard<hal::priority_recursive_mutex> sync(digitalDIOMutex);
  if (port->channel >= hal::kNumDigitalHeaders + hal::kNumDigitalMXPChannels) {
    // Unset the SPI flag
    int32_t bitToUnset = 1 << hal::remapSPIChannel(port->channel);
    uint16_t specialFunctions = hal::spiSystem->readEnableDIO(&status);
    hal::spiSystem->writeEnableDIO(specialFunctions & ~bitToUnset, &status);
  } else if (port->channel >= hal::kNumDigitalHeaders) {
    // Unset the MXP flag
    uint32_t bitToUnset = 1u << hal::remapMXPChannel(port->channel);

    uint16_t specialFunctions =
        hal::digitalSystem->readEnableMXPSpecialFunction(&status);
    hal::digitalSystem->writeEnableMXPSpecialFunction(
        specialFunctions | bitToUnset, &status);
  }
}

/**
 * Allocate a DO PWM Generator.
 * Allocate PWM generators so that they are not accidentally reused.
 *
 * @return PWM Generator handle
 */
HAL_DigitalPWMHandle HAL_AllocateDigitalPWM(int32_t* status) {
  auto handle = digitalPWMHandles.Allocate();
  if (handle == HAL_kInvalidHandle) {
    *status = NO_AVAILABLE_RESOURCES;
    return HAL_kInvalidHandle;
  }

  auto id = digitalPWMHandles.Get(handle);
  if (id == nullptr) {  // would only occur on thread issue.
    *status = HAL_HANDLE_ERROR;
    return HAL_kInvalidHandle;
  }
  *id = static_cast<uint8_t>(hal::getHandleIndex(handle));

  return handle;
}

/**
 * Free the resource associated with a DO PWM generator.
 *
 * @param pwmGenerator The pwmGen to free that was allocated with
 * allocateDigitalPWM()
 */
void HAL_FreeDigitalPWM(HAL_DigitalPWMHandle pwmGenerator, int32_t* status) {
  digitalPWMHandles.Free(pwmGenerator);
}

/**
 * Change the frequency of the DO PWM generator.
 *
 * The valid range is from 0.6 Hz to 19 kHz.  The frequency resolution is
 * logarithmic.
 *
 * @param rate The frequency to output all digital output PWM signals.
 */
void HAL_SetDigitalPWMRate(double rate, int32_t* status) {
  // Currently rounding in the log rate domain... heavy weight toward picking a
  // higher freq.
  // TODO: Round in the linear rate domain.
  hal::initializeDigital(status);
  if (*status != 0) return;
  uint8_t pwmPeriodPower = static_cast<uint8_t>(
      std::log(1.0 /
               (hal::pwmSystem->readLoopTiming(status) * 0.25E-6 * rate)) /
          std::log(2.0) +
      0.5);
  hal::digitalSystem->writePWMPeriodPower(pwmPeriodPower, status);
}

/**
 * Configure the duty-cycle of the PWM generator
 *
 * @param pwmGenerator The generator index reserved by allocateDigitalPWM()
 * @param dutyCycle The percent duty cycle to output [0..1].
 */
void HAL_SetDigitalPWMDutyCycle(HAL_DigitalPWMHandle pwmGenerator,
                                double dutyCycle, int32_t* status) {
  auto port = digitalPWMHandles.Get(pwmGenerator);
  if (port == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return;
  }
  int32_t id = *port;
  if (dutyCycle > 1.0) dutyCycle = 1.0;
  if (dutyCycle < 0.0) dutyCycle = 0.0;
  double rawDutyCycle = 256.0 * dutyCycle;
  if (rawDutyCycle > 255.5) rawDutyCycle = 255.5;
  {
    std::lock_guard<hal::priority_recursive_mutex> sync(hal::digitalPwmMutex);
    uint16_t pwmPeriodPower = hal::digitalSystem->readPWMPeriodPower(status);
    if (pwmPeriodPower < 4) {
      // The resolution of the duty cycle drops close to the highest
      // frequencies.
      rawDutyCycle = rawDutyCycle / std::pow(2.0, 4 - pwmPeriodPower);
    }
    if (id < 4)
      hal::digitalSystem->writePWMDutyCycleA(
          id, static_cast<uint8_t>(rawDutyCycle), status);
    else
      hal::digitalSystem->writePWMDutyCycleB(
          id - 4, static_cast<uint8_t>(rawDutyCycle), status);
  }
}

/**
 * Configure which DO channel the PWM signal is output on
 *
 * @param pwmGenerator The generator index reserved by allocateDigitalPWM()
 * @param channel The Digital Output channel to output on
 */
void HAL_SetDigitalPWMOutputChannel(HAL_DigitalPWMHandle pwmGenerator,
                                    int32_t channel, int32_t* status) {
  auto port = digitalPWMHandles.Get(pwmGenerator);
  if (port == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return;
  }

  // If digital PWM channel is on the MXP, then an offset is needed to write to
  // the correct channel.
  if (channel >= hal::kNumDigitalHeaders &&
      channel < hal::kNumDigitalHeaders + hal::kNumDigitalMXPChannels) {
    channel += hal::kMXPDigitalPWMOffset;
  }

  int32_t id = *port;
  hal::digitalSystem->writePWMOutputSelect(id, channel, status);
}

/**
 * Write a digital I/O bit to the FPGA.
 * Set a single value on a digital I/O channel.
 *
 * @param channel The Digital I/O channel
 * @param value The state to set the digital channel (if it is configured as an
 * output)
 */
void HAL_SetDIO(HAL_DigitalHandle dioPortHandle, HAL_Bool value,
                int32_t* status) {
  auto port =
      hal::digitalChannelHandles.Get(dioPortHandle, hal::HAL_HandleEnum::DIO);
  if (port == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return;
  }
  if (value != 0 && value != 1) {
    if (value != 0) value = 1;
  }
  {
    std::lock_guard<hal::priority_recursive_mutex> sync(digitalDIOMutex);
    hal::tDIO::tDO currentDIO = hal::digitalSystem->readDO(status);

    if (port->channel >=
        hal::kNumDigitalHeaders + hal::kNumDigitalMXPChannels) {
      if (value == 0) {
        currentDIO.SPIPort =
            currentDIO.SPIPort & ~(1u << hal::remapSPIChannel(port->channel));
      } else if (value == 1) {
        currentDIO.SPIPort =
            currentDIO.SPIPort | (1u << hal::remapSPIChannel(port->channel));
      }
    } else if (port->channel < hal::kNumDigitalHeaders) {
      if (value == 0) {
        currentDIO.Headers = currentDIO.Headers & ~(1u << port->channel);
      } else if (value == 1) {
        currentDIO.Headers = currentDIO.Headers | (1u << port->channel);
      }
    } else {
      if (value == 0) {
        currentDIO.MXP =
            currentDIO.MXP & ~(1u << hal::remapMXPChannel(port->channel));
      } else if (value == 1) {
        currentDIO.MXP =
            currentDIO.MXP | (1u << hal::remapMXPChannel(port->channel));
      }
    }
    hal::digitalSystem->writeDO(currentDIO, status);
  }
}

/**
 * Read a digital I/O bit from the FPGA.
 * Get a single value from a digital I/O channel.
 *
 * @param channel The digital I/O channel
 * @return The state of the specified channel
 */
HAL_Bool HAL_GetDIO(HAL_DigitalHandle dioPortHandle, int32_t* status) {
  auto port =
      hal::digitalChannelHandles.Get(dioPortHandle, hal::HAL_HandleEnum::DIO);
  if (port == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return false;
  }
  hal::tDIO::tDI currentDIO = hal::digitalSystem->readDI(status);
  // Shift 00000001 over channel-1 places.
  // AND it against the currentDIO
  // if it == 0, then return false
  // else return true

  if (port->channel >= hal::kNumDigitalHeaders + hal::kNumDigitalMXPChannels) {
    return ((currentDIO.SPIPort >> hal::remapSPIChannel(port->channel)) & 1) !=
           0;
  } else if (port->channel < hal::kNumDigitalHeaders) {
    return ((currentDIO.Headers >> port->channel) & 1) != 0;
  } else {
    return ((currentDIO.MXP >> hal::remapMXPChannel(port->channel)) & 1) != 0;
  }
}

/**
 * Read the direction of a the Digital I/O lines
 * A 1 bit means output and a 0 bit means input.
 *
 * @param channel The digital I/O channel
 * @return The direction of the specified channel
 */
HAL_Bool HAL_GetDIODirection(HAL_DigitalHandle dioPortHandle, int32_t* status) {
  auto port =
      hal::digitalChannelHandles.Get(dioPortHandle, hal::HAL_HandleEnum::DIO);
  if (port == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return false;
  }
  hal::tDIO::tOutputEnable currentOutputEnable =
      hal::digitalSystem->readOutputEnable(status);
  // Shift 00000001 over port->channel-1 places.
  // AND it against the currentOutputEnable
  // if it == 0, then return false
  // else return true

  if (port->channel >= hal::kNumDigitalHeaders + hal::kNumDigitalMXPChannels) {
    return ((currentOutputEnable.SPIPort >>
             hal::remapSPIChannel(port->channel)) &
            1) != 0;
  } else if (port->channel < hal::kNumDigitalHeaders) {
    return ((currentOutputEnable.Headers >> port->channel) & 1) != 0;
  } else {
    return ((currentOutputEnable.MXP >> hal::remapMXPChannel(port->channel)) &
            1) != 0;
  }
}

/**
 * Generate a single pulse.
 * Write a pulse to the specified digital output channel. There can only be a
 * single pulse going at any time.
 *
 * @param channel The Digital Output channel that the pulse should be output on
 * @param pulseLength The active length of the pulse (in seconds)
 */
void HAL_Pulse(HAL_DigitalHandle dioPortHandle, double pulseLength,
               int32_t* status) {
  auto port =
      hal::digitalChannelHandles.Get(dioPortHandle, hal::HAL_HandleEnum::DIO);
  if (port == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return;
  }
  hal::tDIO::tPulse pulse;

  if (port->channel >= hal::kNumDigitalHeaders + hal::kNumDigitalMXPChannels) {
    pulse.SPIPort = 1u << hal::remapSPIChannel(port->channel);
  } else if (port->channel < hal::kNumDigitalHeaders) {
    pulse.Headers = 1u << port->channel;
  } else {
    pulse.MXP = 1u << hal::remapMXPChannel(port->channel);
  }

  hal::digitalSystem->writePulseLength(
      static_cast<uint8_t>(1.0e9 * pulseLength /
                           (hal::pwmSystem->readLoopTiming(status) * 25)),
      status);
  hal::digitalSystem->writePulse(pulse, status);
}

/**
 * Check a DIO line to see if it is currently generating a pulse.
 *
 * @return A pulse is in progress
 */
HAL_Bool HAL_IsPulsing(HAL_DigitalHandle dioPortHandle, int32_t* status) {
  auto port =
      hal::digitalChannelHandles.Get(dioPortHandle, hal::HAL_HandleEnum::DIO);
  if (port == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return false;
  }
  hal::tDIO::tPulse pulseRegister = hal::digitalSystem->readPulse(status);

  if (port->channel >= hal::kNumDigitalHeaders + hal::kNumDigitalMXPChannels) {
    return (pulseRegister.SPIPort &
            (1 << hal::remapSPIChannel(port->channel))) != 0;
  } else if (port->channel < hal::kNumDigitalHeaders) {
    return (pulseRegister.Headers & (1 << port->channel)) != 0;
  } else {
    return (pulseRegister.MXP & (1 << hal::remapMXPChannel(port->channel))) !=
           0;
  }
}

/**
 * Check if any DIO line is currently generating a pulse.
 *
 * @return A pulse on some line is in progress
 */
HAL_Bool HAL_IsAnyPulsing(int32_t* status) {
  hal::initializeDigital(status);
  if (*status != 0) return false;
  hal::tDIO::tPulse pulseRegister = hal::digitalSystem->readPulse(status);
  return pulseRegister.Headers != 0 && pulseRegister.MXP != 0 &&
         pulseRegister.SPIPort != 0;
}

/**
 * Write the filter index from the FPGA.
 * Set the filter index used to filter out short pulses.
 *
 * @param dioPortHandle Handle to the digital I/O channel
 * @param filterIndex The filter index.  Must be in the range 0 - 3, where 0
 *                    means "none" and 1 - 3 means filter # filterIndex - 1.
 */
void HAL_SetFilterSelect(HAL_DigitalHandle dioPortHandle, int32_t filterIndex,
                         int32_t* status) {
  auto port =
      hal::digitalChannelHandles.Get(dioPortHandle, hal::HAL_HandleEnum::DIO);
  if (port == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return;
  }

  std::lock_guard<hal::priority_recursive_mutex> sync(digitalDIOMutex);
  if (port->channel >= hal::kNumDigitalHeaders + hal::kNumDigitalMXPChannels) {
    // Channels 10-15 are SPI channels, so subtract our MXP channels
    hal::digitalSystem->writeFilterSelectHdr(
        port->channel - hal::kNumDigitalMXPChannels, filterIndex, status);
  } else if (port->channel < hal::kNumDigitalHeaders) {
    hal::digitalSystem->writeFilterSelectHdr(port->channel, filterIndex,
                                             status);
  } else {
    hal::digitalSystem->writeFilterSelectMXP(
        hal::remapMXPChannel(port->channel), filterIndex, status);
  }
}

/**
 * Read the filter index from the FPGA.
 * Get the filter index used to filter out short pulses.
 *
 * @param dioPortHandle Handle to the digital I/O channel
 * @return filterIndex The filter index.  Must be in the range 0 - 3,
 * where 0 means "none" and 1 - 3 means filter # filterIndex - 1.
 */
int32_t HAL_GetFilterSelect(HAL_DigitalHandle dioPortHandle, int32_t* status) {
  auto port =
      hal::digitalChannelHandles.Get(dioPortHandle, hal::HAL_HandleEnum::DIO);
  if (port == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return 0;
  }

  std::lock_guard<hal::priority_recursive_mutex> sync(digitalDIOMutex);
  if (port->channel >= hal::kNumDigitalHeaders + hal::kNumDigitalMXPChannels) {
    // Channels 10-15 are SPI channels, so subtract our MXP channels
    return hal::digitalSystem->readFilterSelectHdr(
        port->channel - hal::kNumDigitalMXPChannels, status);
  } else if (port->channel < hal::kNumDigitalHeaders) {
    return hal::digitalSystem->readFilterSelectHdr(port->channel, status);
  } else {
    return hal::digitalSystem->readFilterSelectMXP(
        hal::remapMXPChannel(port->channel), status);
  }
}

/**
 * Set the filter period for the specified filter index.
 *
 * Set the filter period in FPGA cycles.  Even though there are 2 different
 * filter index domains (MXP vs HDR), ignore that distinction for now since it
 * compilicates the interface.  That can be changed later.
 *
 * @param filterIndex The filter index, 0 - 2.
 * @param value The number of cycles that the signal must not transition to be
 * counted as a transition.
 */
void HAL_SetFilterPeriod(int32_t filterIndex, int64_t value, int32_t* status) {
  hal::initializeDigital(status);
  if (*status != 0) return;
  std::lock_guard<hal::priority_recursive_mutex> sync(digitalDIOMutex);
  hal::digitalSystem->writeFilterPeriodHdr(filterIndex, value, status);
  if (*status == 0) {
    hal::digitalSystem->writeFilterPeriodMXP(filterIndex, value, status);
  }
}

/**
 * Get the filter period for the specified filter index.
 *
 * Get the filter period in FPGA cycles.  Even though there are 2 different
 * filter index domains (MXP vs HDR), ignore that distinction for now since it
 * compilicates the interface.  Set status to NiFpga_Status_SoftwareFault if the
 * filter values miss-match.
 *
 * @param filterIndex The filter index, 0 - 2.
 * @param value The number of cycles that the signal must not transition to be
 * counted as a transition.
 */
int64_t HAL_GetFilterPeriod(int32_t filterIndex, int32_t* status) {
  hal::initializeDigital(status);
  if (*status != 0) return 0;
  uint32_t hdrPeriod = 0;
  uint32_t mxpPeriod = 0;
  {
    std::lock_guard<hal::priority_recursive_mutex> sync(digitalDIOMutex);
    hdrPeriod = hal::digitalSystem->readFilterPeriodHdr(filterIndex, status);
    if (*status == 0) {
      mxpPeriod = hal::digitalSystem->readFilterPeriodMXP(filterIndex, status);
    }
  }
  if (hdrPeriod != mxpPeriod) {
    *status = NiFpga_Status_SoftwareFault;
    return -1;
  }
  return hdrPeriod;
}
}
