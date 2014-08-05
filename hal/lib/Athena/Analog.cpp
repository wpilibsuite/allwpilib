
#include "HAL/Analog.hpp"

#include "Port.h"
#include "HAL/HAL.hpp"
#include "ChipObject.h"
#include "HAL/cpp/Synchronized.hpp"
#include "HAL/cpp/Resource.hpp"
#include "NetworkCommunication/AICalibration.h"
#include "NetworkCommunication/LoadOut.h"

static const long kTimebase = 40000000; ///< 40 MHz clock
static const long kDefaultOversampleBits = 0;
static const long kDefaultAverageBits = 7;
static const float kDefaultSampleRate = 50000.0;
static const uint32_t kAnalogInputPins = 8;
static const uint32_t kAnalogOutputPins = 2;

static const uint32_t kAccumulatorNumChannels = 2;
static const uint32_t kAccumulatorChannels[] = {0, 1};

struct AnalogPort {
  Port port;
  tAccumulator *accumulator;
};

bool analogSampleRateSet = false;
MUTEX_ID analogRegisterWindowSemaphore = NULL;
tAI* analogInputSystem = NULL;
tAO* analogOutputSystem = NULL;
uint32_t analogNumChannelsToActivate = 0;

// Utility methods defined below.
uint32_t getAnalogNumActiveChannels(int32_t *status);
uint32_t getAnalogNumChannelsToActivate(int32_t *status);
void setAnalogNumChannelsToActivate(uint32_t channels);

bool analogSystemInitialized = false;

/**
 * Initialize the analog System.
 */
void initializeAnalog(int32_t *status) {
  if (analogSystemInitialized) return;
  analogRegisterWindowSemaphore = initializeMutexRecursive();
  analogInputSystem = tAI::create(status);
  analogOutputSystem = tAO::create(status);
  setAnalogNumChannelsToActivate(kAnalogInputPins);
  setAnalogSampleRate(kDefaultSampleRate, status);
  analogSystemInitialized = true;
}

/**
 * Initialize the analog input port using the given port object.
 */
void* initializeAnalogInputPort(void* port_pointer, int32_t *status) {
  initializeAnalog(status);
  Port* port = (Port*) port_pointer;

  // Initialize port structure
  AnalogPort* analog_port = new AnalogPort();
  analog_port->port = *port;
  if (isAccumulatorChannel(analog_port, status)) {
    analog_port->accumulator = tAccumulator::create(port->pin, status);
  } else analog_port->accumulator = NULL;

  // Set default configuration
  analogInputSystem->writeScanList(port->pin, port->pin, status);
  setAnalogAverageBits(analog_port, kDefaultAverageBits, status);
  setAnalogOversampleBits(analog_port, kDefaultOversampleBits, status);
  return analog_port;
}

/**
 * Initialize the analog output port using the given port object.
 */
void* initializeAnalogOutputPort(void* port_pointer, int32_t *status) {
  initializeAnalog(status);
  Port* port = (Port*) port_pointer;

  // Initialize port structure
  AnalogPort* analog_port = new AnalogPort();
  analog_port->port = *port;
  return analog_port;
}


/**
 * Check that the analog module number is valid.
 *
 * @return Analog module is valid and present
 */
bool checkAnalogModule(uint8_t module) {
  return module == 1;
}

/**
 * Check that the analog output channel number is value.
 * Verify that the analog channel number is one of the legal channel numbers. Channel numbers
 * are 0-based.
 *
 * @return Analog channel is valid
 */
bool checkAnalogInputChannel(uint32_t pin) {
  if (pin < kAnalogInputPins)
    return true;
  return false;
}

/**
 * Check that the analog output channel number is value.
 * Verify that the analog channel number is one of the legal channel numbers. Channel numbers
 * are 0-based.
 *
 * @return Analog channel is valid
 */
bool checkAnalogOutputChannel(uint32_t pin) {
  if (pin < kAnalogOutputPins)
    return true;
  return false;
}

void setAnalogOutput(void* analog_port_pointer, double voltage, int32_t *status) {
  AnalogPort* port = (AnalogPort*) analog_port_pointer;

  uint16_t rawValue = (uint16_t)(voltage / 5.0 * 0x1000);

  if(voltage < 0.0) rawValue = 0;
  else if(voltage > 5.0) rawValue = 0x1000;

  analogOutputSystem->writeMXP(port->port.pin, rawValue, status);
}

double getAnalogOutput(void* analog_port_pointer, int32_t *status) {
  AnalogPort* port = (AnalogPort*) analog_port_pointer;

  uint16_t rawValue = analogOutputSystem->readMXP(port->port.pin, status);

  return rawValue * 5.0 / 0x1000;
}

/**
 * Set the sample rate.
 *
 * This is a global setting for the Athena and effects all channels.
 *
 * @param samplesPerSecond The number of samples per channel per second.
 */
void setAnalogSampleRate(double samplesPerSecond, int32_t *status) {
  // TODO: This will change when variable size scan lists are implemented.
  // TODO: Need float comparison with epsilon.
  //wpi_assert(!sampleRateSet || GetSampleRate() == samplesPerSecond);
  analogSampleRateSet = true;

  // Compute the convert rate
  uint32_t ticksPerSample = (uint32_t)((float)kTimebase / samplesPerSecond);
  uint32_t ticksPerConversion = ticksPerSample / getAnalogNumChannelsToActivate(status);
  // ticksPerConversion must be at least 80
  if (ticksPerConversion < 80) {
    if ((*status) >= 0) *status = SAMPLE_RATE_TOO_HIGH;
    ticksPerConversion = 80;
  }

  // Atomically set the scan size and the convert rate so that the sample rate is constant
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
float getAnalogSampleRate(int32_t *status) {
  uint32_t ticksPerConversion = analogInputSystem->readLoopTiming(status);
  uint32_t ticksPerSample = ticksPerConversion * getAnalogNumActiveChannels(status);
  return (float)kTimebase / (float)ticksPerSample;
}

/**
 * Set the number of averaging bits.
 *
 * This sets the number of averaging bits. The actual number of averaged samples is 2**bits.
 * Use averaging to improve the stability of your measurement at the expense of sampling rate.
 * The averaging is done automatically in the FPGA.
 *
 * @param analog_port_pointer Pointer to the analog port to configure.
 * @param bits Number of bits to average.
 */
void setAnalogAverageBits(void* analog_port_pointer, uint32_t bits, int32_t *status) {
  AnalogPort* port = (AnalogPort*) analog_port_pointer;
  analogInputSystem->writeAverageBits(port->port.pin, bits, status);
}

/**
 * Get the number of averaging bits.
 *
 * This gets the number of averaging bits from the FPGA. The actual number of averaged samples is 2**bits.
 * The averaging is done automatically in the FPGA.
 *
 * @param analog_port_pointer Pointer to the analog port to use.
 * @return Bits to average.
 */
uint32_t getAnalogAverageBits(void* analog_port_pointer, int32_t *status) {
  AnalogPort* port = (AnalogPort*) analog_port_pointer;
  uint32_t result = analogInputSystem->readAverageBits(port->port.pin, status);
  return result;
}

/**
 * Set the number of oversample bits.
 *
 * This sets the number of oversample bits. The actual number of oversampled values is 2**bits.
 * Use oversampling to improve the resolution of your measurements at the expense of sampling rate.
 * The oversampling is done automatically in the FPGA.
 *
 * @param analog_port_pointer Pointer to the analog port to use.
 * @param bits Number of bits to oversample.
 */
void setAnalogOversampleBits(void* analog_port_pointer, uint32_t bits, int32_t *status) {
  AnalogPort* port = (AnalogPort*) analog_port_pointer;
  analogInputSystem->writeOversampleBits(port->port.pin, bits, status);
}


/**
 * Get the number of oversample bits.
 *
 * This gets the number of oversample bits from the FPGA. The actual number of oversampled values is
 * 2**bits. The oversampling is done automatically in the FPGA.
 *
 * @param analog_port_pointer Pointer to the analog port to use.
 * @return Bits to oversample.
 */
uint32_t getAnalogOversampleBits(void* analog_port_pointer, int32_t *status) {
  AnalogPort* port = (AnalogPort*) analog_port_pointer;
  uint32_t result = analogInputSystem->readOversampleBits(port->port.pin, status);
  return result;
}

/**
 * Get a sample straight from the channel on this module.
 *
 * The sample is a 12-bit value representing the 0V to 5V range of the A/D converter in the module.
 * The units are in A/D converter codes.  Use GetVoltage() to get the analog value in calibrated units.
 *
 * @param analog_port_pointer Pointer to the analog port to use.
 * @return A sample straight from the channel on this module.
 */
int16_t getAnalogValue(void* analog_port_pointer, int32_t *status) {
  AnalogPort* port = (AnalogPort*) analog_port_pointer;
  int16_t value;
  checkAnalogInputChannel(port->port.pin);

  tAI::tReadSelect readSelect;
  readSelect.Channel = port->port.pin;
  readSelect.Averaged = false;

  {
    Synchronized sync(analogRegisterWindowSemaphore);
    analogInputSystem->writeReadSelect(readSelect, status);
    analogInputSystem->strobeLatchOutput(status);
    value = (int16_t) analogInputSystem->readOutput(status);
  }

  return value;
}

/**
 * Get a sample from the output of the oversample and average engine for the channel.
 *
 * The sample is 12-bit + the value configured in SetOversampleBits().
 * The value configured in SetAverageBits() will cause this value to be averaged 2**bits number of samples.
 * This is not a sliding window.  The sample will not change until 2**(OversamplBits + AverageBits) samples
 * have been acquired from the module on this channel.
 * Use GetAverageVoltage() to get the analog value in calibrated units.
 *
 * @param analog_port_pointer Pointer to the analog port to use.
 * @return A sample from the oversample and average engine for the channel.
 */
int32_t getAnalogAverageValue(void* analog_port_pointer, int32_t *status) {
  AnalogPort* port = (AnalogPort*) analog_port_pointer;
  int32_t value;
  checkAnalogInputChannel(port->port.pin);

  tAI::tReadSelect readSelect;
  readSelect.Channel = port->port.pin;
  readSelect.Averaged = true;

  {
    Synchronized sync(analogRegisterWindowSemaphore);
    analogInputSystem->writeReadSelect(readSelect, status);
    analogInputSystem->strobeLatchOutput(status);
    value = (int32_t) analogInputSystem->readOutput(status);
  }

  return value;
}

/**
 * Get a scaled sample straight from the channel on this module.
 *
 * The value is scaled to units of Volts using the calibrated scaling data from GetLSBWeight() and GetOffset().
 *
 * @param analog_port_pointer Pointer to the analog port to use.
 * @return A scaled sample straight from the channel on this module.
 */
float getAnalogVoltage(void* analog_port_pointer, int32_t *status) {
  int16_t value = getAnalogValue(analog_port_pointer, status);
  uint32_t LSBWeight = getAnalogLSBWeight(analog_port_pointer, status);
  int32_t offset = getAnalogOffset(analog_port_pointer, status);
  float voltage = LSBWeight * 1.0e-9 * value - offset * 1.0e-9;
  return voltage;
}

/**
 * Get a scaled sample from the output of the oversample and average engine for the channel.
 *
 * The value is scaled to units of Volts using the calibrated scaling data from GetLSBWeight() and GetOffset().
 * Using oversampling will cause this value to be higher resolution, but it will update more slowly.
 * Using averaging will cause this value to be more stable, but it will update more slowly.
 *
 * @param analog_port_pointer Pointer to the analog port to use.
 * @return A scaled sample from the output of the oversample and average engine for the channel.
 */
float getAnalogAverageVoltage(void* analog_port_pointer, int32_t *status) {
  int32_t value = getAnalogAverageValue(analog_port_pointer, status);
  uint32_t LSBWeight = getAnalogLSBWeight(analog_port_pointer, status);
  int32_t offset = getAnalogOffset(analog_port_pointer, status);
  uint32_t oversampleBits = getAnalogOversampleBits(analog_port_pointer, status);
  float voltage = ((LSBWeight * 1.0e-9 * value) / (float)(1 << oversampleBits)) - offset * 1.0e-9;
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
int32_t getAnalogVoltsToValue(void* analog_port_pointer, double voltage, int32_t *status) {
  if (voltage > 5.0) {
    voltage = 5.0;
    *status = VOLTAGE_OUT_OF_RANGE;
  }
  if (voltage < 0.0) {
    voltage = 0.0;
    *status = VOLTAGE_OUT_OF_RANGE;
  }
  uint32_t LSBWeight = getAnalogLSBWeight(analog_port_pointer, status);
  int32_t offset = getAnalogOffset(analog_port_pointer, status);
  int32_t value = (int32_t) ((voltage + offset * 1.0e-9) / (LSBWeight * 1.0e-9));
  return value;
}

/**
 * Get the factory scaling least significant bit weight constant.
 * The least significant bit weight constant for the channel that was calibrated in
 * manufacturing and stored in an eeprom in the module.
 *
 * Volts = ((LSB_Weight * 1e-9) * raw) - (Offset * 1e-9)
 *
 * @param analog_port_pointer Pointer to the analog port to use.
 * @return Least significant bit weight.
 */
uint32_t getAnalogLSBWeight(void* analog_port_pointer, int32_t *status) {
  AnalogPort* port = (AnalogPort*) analog_port_pointer;
  uint32_t lsbWeight = FRC_NetworkCommunication_nAICalibration_getLSBWeight(0, port->port.pin, status); // XXX: aiSystemIndex == 0?
  return lsbWeight;
}

/**
 * Get the factory scaling offset constant.
 * The offset constant for the channel that was calibrated in manufacturing and stored
 * in an eeprom in the module.
 *
 * Volts = ((LSB_Weight * 1e-9) * raw) - (Offset * 1e-9)
 *
 * @param analog_port_pointer Pointer to the analog port to use.
 * @return Offset constant.
 */
int32_t getAnalogOffset(void* analog_port_pointer, int32_t *status) {
  AnalogPort* port = (AnalogPort*) analog_port_pointer;
  int32_t offset = FRC_NetworkCommunication_nAICalibration_getOffset(0, port->port.pin, status); // XXX: aiSystemIndex == 0?
  return offset;
}


/**
 * Return the number of channels on the module in use.
 *
 * @return Active channels.
 */
uint32_t getAnalogNumActiveChannels(int32_t *status) {
  uint32_t scanSize = analogInputSystem->readConfig_ScanSize(status);
  if (scanSize == 0)
    return 8;
  return scanSize;
}

/**
 * Get the number of active channels.
 *
 * This is an internal function to allow the atomic update of both the
 * number of active channels and the sample rate.
 *
 * When the number of channels changes, use the new value.  Otherwise,
 * return the curent value.
 *
 * @return Value to write to the active channels field.
 */
uint32_t getAnalogNumChannelsToActivate(int32_t *status) {
  if(analogNumChannelsToActivate == 0) return getAnalogNumActiveChannels(status);
  return analogNumChannelsToActivate;
}

/**
 * Set the number of active channels.
 *
 * Store the number of active channels to set.  Don't actually commit to hardware
 * until SetSampleRate().
 *
 * @param channels Number of active channels.
 */
void setAnalogNumChannelsToActivate(uint32_t channels) {
  analogNumChannelsToActivate = channels;
}

//// Accumulator Stuff

/**
 * Is the channel attached to an accumulator.
 *
 * @return The analog channel is attached to an accumulator.
 */
bool isAccumulatorChannel(void* analog_port_pointer, int32_t *status) {
  AnalogPort* port = (AnalogPort*) analog_port_pointer;
  for (uint32_t i=0; i < kAccumulatorNumChannels; i++) {
    if (port->port.pin == kAccumulatorChannels[i]) return true;
  }
  return false;
}

/**
 * Initialize the accumulator.
 */
void initAccumulator(void* analog_port_pointer, int32_t *status) {
  setAccumulatorCenter(analog_port_pointer, 0, status);
  resetAccumulator(analog_port_pointer, status);
}

/**
 * Resets the accumulator to the initial value.
 */
void resetAccumulator(void* analog_port_pointer, int32_t *status) {
  AnalogPort* port = (AnalogPort*) analog_port_pointer;
  if (port->accumulator == NULL) {
    *status = NULL_PARAMETER;
    return;
  }
  port->accumulator->strobeReset(status);
}

/**
 * Set the center value of the accumulator.
 *
 * The center value is subtracted from each A/D value before it is added to the accumulator. This
 * is used for the center value of devices like gyros and accelerometers to make integration work
 * and to take the device offset into account when integrating.
 *
 * This center value is based on the output of the oversampled and averaged source from channel 1.
 * Because of this, any non-zero oversample bits will affect the size of the value for this field.
 */
void setAccumulatorCenter(void* analog_port_pointer, int32_t center, int32_t *status) {
  AnalogPort* port = (AnalogPort*) analog_port_pointer;
  if (port->accumulator == NULL) {
    *status = NULL_PARAMETER;
    return;
  }
  port->accumulator->writeCenter(center, status);
}

/**
 * Set the accumulator's deadband.
 */
void setAccumulatorDeadband(void* analog_port_pointer, int32_t deadband, int32_t *status) {
  AnalogPort* port = (AnalogPort*) analog_port_pointer;
  if (port->accumulator == NULL) {
    *status = NULL_PARAMETER;
    return;
  }
  port->accumulator->writeDeadband(deadband, status);
}

/**
 * Read the accumulated value.
 *
 * Read the value that has been accumulating on channel 1.
 * The accumulator is attached after the oversample and average engine.
 *
 * @return The 64-bit value accumulated since the last Reset().
 */
int64_t getAccumulatorValue(void* analog_port_pointer, int32_t *status) {
  AnalogPort* port = (AnalogPort*) analog_port_pointer;
  if (port->accumulator == NULL) {
    *status = NULL_PARAMETER;
    return 0;
  }
  int64_t value = port->accumulator->readOutput_Value(status);
  return value;
}

/**
 * Read the number of accumulated values.
 *
 * Read the count of the accumulated values since the accumulator was last Reset().
 *
 * @return The number of times samples from the channel were accumulated.
 */
uint32_t getAccumulatorCount(void* analog_port_pointer, int32_t *status) {
  AnalogPort* port = (AnalogPort*) analog_port_pointer;
  if (port->accumulator == NULL) {
    *status = NULL_PARAMETER;
    return 0;
  }
  return port->accumulator->readOutput_Count(status);
}

/**
 * Read the accumulated value and the number of accumulated values atomically.
 *
 * This function reads the value and count from the FPGA atomically.
 * This can be used for averaging.
 *
 * @param value Pointer to the 64-bit accumulated output.
 * @param count Pointer to the number of accumulation cycles.
 */
void getAccumulatorOutput(void* analog_port_pointer, int64_t *value, uint32_t *count, int32_t *status) {
  AnalogPort* port = (AnalogPort*) analog_port_pointer;
  if (port->accumulator == NULL) {
    *status = NULL_PARAMETER;
    return;
  }
  if (value == NULL || count == NULL) {
    *status = NULL_PARAMETER;
    return;
  }

  tAccumulator::tOutput output = port->accumulator->readOutput(status);

  *value = output.Value;
  *count = output.Count;
}


struct trigger_t {
  tAnalogTrigger* trigger;
  AnalogPort* port;
  uint32_t index;
};
typedef struct trigger_t AnalogTrigger;

static Resource *triggers = NULL;

void* initializeAnalogTrigger(void* port_pointer, uint32_t *index, int32_t *status) {
  Port* port = (Port*) port_pointer;
  Resource::CreateResourceObject(&triggers, tAnalogTrigger::kNumSystems);

  AnalogTrigger* trigger = new AnalogTrigger();
  trigger->port = (AnalogPort*) initializeAnalogInputPort(port, status);
  trigger->index = triggers->Allocate("Analog Trigger");
  *index = trigger->index;
  // TODO: if (index == ~0ul) { CloneError(triggers); return; }

  trigger->trigger = tAnalogTrigger::create(trigger->index, status);
  trigger->trigger->writeSourceSelect_Channel(port->pin, status);

  return trigger;
}

void cleanAnalogTrigger(void* analog_trigger_pointer, int32_t *status) {
  AnalogTrigger* trigger = (AnalogTrigger*) analog_trigger_pointer;
  triggers->Free(trigger->index);
  delete trigger->trigger;
  delete trigger;
}

void setAnalogTriggerLimitsRaw(void* analog_trigger_pointer, int32_t lower, int32_t upper, int32_t *status) {
  AnalogTrigger* trigger = (AnalogTrigger*) analog_trigger_pointer;
  if (lower > upper) {
	*status = ANALOG_TRIGGER_LIMIT_ORDER_ERROR;
  }
  trigger->trigger->writeLowerLimit(lower, status);
  trigger->trigger->writeUpperLimit(upper, status);
}

/**
 * Set the upper and lower limits of the analog trigger.
 * The limits are given as floating point voltage values.
 */
void setAnalogTriggerLimitsVoltage(void* analog_trigger_pointer, double lower, double upper, int32_t *status) {
  AnalogTrigger* trigger = (AnalogTrigger*) analog_trigger_pointer;
  if (lower > upper) {
	*status = ANALOG_TRIGGER_LIMIT_ORDER_ERROR;
  }
  // TODO: This depends on the averaged setting.  Only raw values will work as is.
  trigger->trigger->writeLowerLimit(getAnalogVoltsToValue(trigger->port, lower, status), status);
  trigger->trigger->writeUpperLimit(getAnalogVoltsToValue(trigger->port, upper, status), status);
}

/**
 * Configure the analog trigger to use the averaged vs. raw values.
 * If the value is true, then the averaged value is selected for the analog trigger, otherwise
 * the immediate value is used.
 */
void setAnalogTriggerAveraged(void* analog_trigger_pointer, bool useAveragedValue, int32_t *status) {
  AnalogTrigger* trigger = (AnalogTrigger*) analog_trigger_pointer;
  if (trigger->trigger->readSourceSelect_Filter(status) != 0) {
	*status = INCOMPATIBLE_STATE;
	// TODO: wpi_setWPIErrorWithContext(IncompatibleMode, "Hardware does not support average and filtering at the same time.");
  }
  trigger->trigger->writeSourceSelect_Averaged(useAveragedValue, status);
}

/**
 * Configure the analog trigger to use a filtered value.
 * The analog trigger will operate with a 3 point average rejection filter. This is designed to
 * help with 360 degree pot applications for the period where the pot crosses through zero.
 */
void setAnalogTriggerFiltered(void* analog_trigger_pointer, bool useFilteredValue, int32_t *status) {
  AnalogTrigger* trigger = (AnalogTrigger*) analog_trigger_pointer;
  if (trigger->trigger->readSourceSelect_Averaged(status) != 0) {
	*status = INCOMPATIBLE_STATE;
	// TODO: wpi_setWPIErrorWithContext(IncompatibleMode, "Hardware does not support average and filtering at the same time.");
  }
  trigger->trigger->writeSourceSelect_Filter(useFilteredValue, status);
}

/**
 * Return the InWindow output of the analog trigger.
 * True if the analog input is between the upper and lower limits.
 * @return The InWindow output of the analog trigger.
 */
bool getAnalogTriggerInWindow(void* analog_trigger_pointer, int32_t *status) {
  AnalogTrigger* trigger = (AnalogTrigger*) analog_trigger_pointer;
  return trigger->trigger->readOutput_InHysteresis(trigger->index, status) != 0;
}

/**
 * Return the TriggerState output of the analog trigger.
 * True if above upper limit.
 * False if below lower limit.
 * If in Hysteresis, maintain previous state.
 * @return The TriggerState output of the analog trigger.
 */
bool getAnalogTriggerTriggerState(void* analog_trigger_pointer, int32_t *status) {
  AnalogTrigger* trigger = (AnalogTrigger*) analog_trigger_pointer;
  return trigger->trigger->readOutput_OverLimit(trigger->index, status) != 0;
}

/**
 * Get the state of the analog trigger output.
 * @return The state of the analog trigger output.
 */
bool getAnalogTriggerOutput(void* analog_trigger_pointer, AnalogTriggerType type, int32_t *status) {
  AnalogTrigger* trigger = (AnalogTrigger*) analog_trigger_pointer;
  bool result = false;
  switch(type) {
  case kInWindow:
	result = trigger->trigger->readOutput_InHysteresis(trigger->index, status);
	break; // XXX: Backport
  case kState:
	result = trigger->trigger->readOutput_OverLimit(trigger->index, status);
	break; // XXX: Backport
  case kRisingPulse:
  case kFallingPulse:
	*status = ANALOG_TRIGGER_PULSE_OUTPUT_ERROR;
	return false;
  }
  return result;
}



//// Float JNA Hack
// Float
int getAnalogSampleRateIntHack(int32_t *status) {
  return floatToInt(getAnalogSampleRate(status));
}

int getAnalogVoltageIntHack(void* analog_port_pointer, int32_t *status) {
  return floatToInt(getAnalogVoltage(analog_port_pointer, status));
}

int getAnalogAverageVoltageIntHack(void* analog_port_pointer, int32_t *status) {
  return floatToInt(getAnalogAverageVoltage(analog_port_pointer, status));
}


// Doubles
void setAnalogSampleRateIntHack(int samplesPerSecond, int32_t *status) {
  setAnalogSampleRate(intToFloat(samplesPerSecond), status);
}

int32_t getAnalogVoltsToValueIntHack(void* analog_port_pointer, int voltage, int32_t *status) {
  return getAnalogVoltsToValue(analog_port_pointer, intToFloat(voltage), status);
}

void setAnalogTriggerLimitsVoltageIntHack(void* analog_trigger_pointer, int lower, int upper, int32_t *status) {
  setAnalogTriggerLimitsVoltage(analog_trigger_pointer, intToFloat(lower), intToFloat(upper), status);
}
