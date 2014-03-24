/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008. All Rights Reserved.							  */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in $(WIND_BASE)/WPILib.  */
/*----------------------------------------------------------------------------*/

#include "AnalogModule.h"
#include "Timer.h"
#include "WPIErrors.h"

const long AnalogModule::kTimebase; ///< 40 MHz clock
const long AnalogModule::kDefaultOversampleBits;
const long AnalogModule::kDefaultAverageBits;
constexpr float AnalogModule::kDefaultSampleRate;

/**
 * Get an instance of an Analog Module.
 * 
 * Singleton analog module creation where a module is allocated on the first use
 * and the same module is returned on subsequent uses.
 * 
 * @param moduleNumber The analog module to get (1 or 2).
 * @return A pointer to the AnalogModule.
 */
AnalogModule* AnalogModule::GetInstance(uint8_t moduleNumber)
{
	if (checkAnalogModule(moduleNumber))
	{
		return (AnalogModule*)GetModule(nLoadOut::kModuleType_Analog, moduleNumber);
	}

	// If this wasn't caught before now, make sure we say what's wrong before we crash
	char buf[64];
	snprintf(buf, 64, "Analog Module %d", moduleNumber);
	wpi_setGlobalWPIErrorWithContext(ModuleIndexOutOfRange, buf);

	return NULL;
}

/**
 * Create a new instance of an analog module.
 * 
 * Create an instance of the analog module object. Initialize all the parameters
 * to reasonable values on start.
 * Setting a global value on an analog module can be done only once unless subsequent
 * values are set the previously set value.
 * Analog modules are a singleton, so the constructor is never called outside of this class.
 * 
 * @param moduleNumber The analog module to create (1 or 2).
 */
AnalogModule::AnalogModule(uint8_t moduleNumber)
	: Module(nLoadOut::kModuleType_Analog, moduleNumber)
	, m_moduleNumber(moduleNumber)
	, m_ports()
{
	for (uint32_t i = 0; i < kAnalogChannels; i++)
	{
	  void* port = getPortWithModule(moduleNumber, i+1);
	  int32_t status = 0;
	  m_ports[i] = initializeAnalogPort(port, &status);
	  wpi_setErrorWithContext(status, getHALErrorMessage(status));
	}

	AddToSingletonList();
}

/**
 * Destructor for AnalogModule.
 */
AnalogModule::~AnalogModule()
{

}

/**
 * Set the sample rate on the module.
 * 
 * This is a global setting for the module and effects all channels.
 * 
 * @param samplesPerSecond The number of samples per channel per second.
 */
void AnalogModule::SetSampleRate(float samplesPerSecond)
{
	int32_t status = 0;
	setAnalogSampleRateWithModule(m_moduleNumber, samplesPerSecond, &status);
	wpi_setErrorWithContext(status, getHALErrorMessage(status));
}

/**
 * Get the current sample rate on the module.
 * 
 * This assumes one entry in the scan list.
 * This is a global setting for the module and effects all channels.
 * 
 * @return Sample rate.
 */
float AnalogModule::GetSampleRate()
{
	int32_t status = 0;
	float sampleRate = getAnalogSampleRateWithModule(m_moduleNumber, &status);
	wpi_setErrorWithContext(status, getHALErrorMessage(status));
	return sampleRate;
}

/**
 * Set the number of averaging bits.
 * 
 * This sets the number of averaging bits. The actual number of averaged samples is 2**bits.
 * Use averaging to improve the stability of your measurement at the expense of sampling rate.
 * The averaging is done automatically in the FPGA.
 * 
 * @param channel Analog channel to configure.
 * @param bits Number of bits to average.
 */
void AnalogModule::SetAverageBits(uint32_t channel, uint32_t bits)
{
	int32_t status = 0;
	setAnalogAverageBits(m_ports[channel-1], bits, &status);
	wpi_setErrorWithContext(status, getHALErrorMessage(status));
}

/**
 * Get the number of averaging bits.
 * 
 * This gets the number of averaging bits from the FPGA. The actual number of averaged samples is 2**bits.
 * The averaging is done automatically in the FPGA.
 * 
 * @param channel Channel to address.
 * @return Bits to average.
 */
uint32_t AnalogModule::GetAverageBits(uint32_t channel)
{
	int32_t status = 0;
	int32_t averageBits = getAnalogAverageBits(m_ports[channel-1], &status);
	wpi_setErrorWithContext(status, getHALErrorMessage(status));
	return averageBits;
}

/**
 * Set the number of oversample bits.
 * 
 * This sets the number of oversample bits. The actual number of oversampled values is 2**bits.
 * Use oversampling to improve the resolution of your measurements at the expense of sampling rate.
 * The oversampling is done automatically in the FPGA.
 * 
 * @param channel Analog channel to configure.
 * @param bits Number of bits to oversample.
 */
void AnalogModule::SetOversampleBits(uint32_t channel, uint32_t bits)
{
	int32_t status = 0;
	setAnalogOversampleBits(m_ports[channel-1], bits, &status);
	wpi_setErrorWithContext(status, getHALErrorMessage(status));
}

/**
 * Get the number of oversample bits.
 * 
 * This gets the number of oversample bits from the FPGA. The actual number of oversampled values is
 * 2**bits. The oversampling is done automatically in the FPGA.
 * 
 * @param channel Channel to address.
 * @return Bits to oversample.
 */
uint32_t AnalogModule::GetOversampleBits(uint32_t channel)
{
	int32_t status = 0;
	int32_t oversampleBits = getAnalogOversampleBits(m_ports[channel-1], &status);
	wpi_setErrorWithContext(status, getHALErrorMessage(status));
	return oversampleBits;
}

/**
 * Get a sample straight from the channel on this module.
 * 
 * The sample is a 12-bit value representing the -10V to 10V range of the A/D converter in the module.
 * The units are in A/D converter codes.  Use GetVoltage() to get the analog value in calibrated units.
 * 
 * @return A sample straight from the channel on this module.
 */
int16_t AnalogModule::GetValue(uint32_t channel)
{
	int32_t status = 0;
	int16_t value = getAnalogValue(m_ports[channel-1], &status);
	wpi_setErrorWithContext(status, getHALErrorMessage(status));
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
 * @param channel Channel number to read.
 * @return A sample from the oversample and average engine for the channel.
 */
int32_t AnalogModule::GetAverageValue(uint32_t channel)
{
	int32_t status = 0;
	int32_t value = getAnalogAverageValue(m_ports[channel-1], &status);
	wpi_setErrorWithContext(status, getHALErrorMessage(status));
	return value;
}

/**
 * Convert a voltage to a raw value for a specified channel.
 * 
 * This process depends on the calibration of each channel, so the channel
 * must be specified.
 * 
 * @todo This assumes raw values.  Oversampling not supported as is.
 * 
 * @param channel The channel to convert for.
 * @param voltage The voltage to convert.
 * @return The raw value for the channel.
 */
int32_t AnalogModule::VoltsToValue(int32_t channel, float voltage)
{
	int32_t status = 0;
	int32_t value = getAnalogVoltsToValue(m_ports[channel-1], voltage, &status);
	wpi_setErrorWithContext(status, getHALErrorMessage(status));
	return value;
}

/**
 * Get a scaled sample straight from the channel on this module.
 * 
 * The value is scaled to units of Volts using the calibrated scaling data from GetLSBWeight() and GetOffset().
 * 
 * @param channel The channel to read.
 * @return A scaled sample straight from the channel on this module.
 */
float AnalogModule::GetVoltage(uint32_t channel)
{
	int32_t status = 0;
	float voltage = getAnalogVoltage(m_ports[channel-1], &status);
	wpi_setErrorWithContext(status, getHALErrorMessage(status));
	return voltage;
}

/**
 * Get a scaled sample from the output of the oversample and average engine for the channel.
 * 
 * The value is scaled to units of Volts using the calibrated scaling data from GetLSBWeight() and GetOffset().
 * Using oversampling will cause this value to be higher resolution, but it will update more slowly.
 * Using averaging will cause this value to be more stable, but it will update more slowly.
 * 
 * @param channel The channel to read.
 * @return A scaled sample from the output of the oversample and average engine for the channel.
 */
float AnalogModule::GetAverageVoltage(uint32_t channel)
{
	int32_t status = 0;
	float voltage = getAnalogAverageVoltage(m_ports[channel-1], &status);
	wpi_setErrorWithContext(status, getHALErrorMessage(status));
	return voltage;
}

/**
 * Get the factory scaling least significant bit weight constant.
 * The least significant bit weight constant for the channel that was calibrated in
 * manufacturing and stored in an eeprom in the module.
 * 
 * Volts = ((LSB_Weight * 1e-9) * raw) - (Offset * 1e-9)
 * 
 * @param channel The channel to get calibration data for.
 * @return Least significant bit weight.
 */
uint32_t AnalogModule::GetLSBWeight(uint32_t channel) 
{
	int32_t status = 0;
	int32_t lsbWeight = getAnalogLSBWeight(m_ports[channel-1], &status);
	wpi_setErrorWithContext(status, getHALErrorMessage(status));
	return lsbWeight;
}

/**
 * Get the factory scaling offset constant.
 * The offset constant for the channel that was calibrated in manufacturing and stored
 * in an eeprom in the module.
 * 
 * Volts = ((LSB_Weight * 1e-9) * raw) - (Offset * 1e-9)
 * 
 * @param channel The channel to get calibration data for.
 * @return Offset constant.
 */
int32_t AnalogModule::GetOffset(uint32_t channel)
{
	int32_t status = 0;
	int32_t offset = getAnalogOffset(m_ports[channel-1], &status);
	wpi_setErrorWithContext(status, getHALErrorMessage(status));
	return offset;
}


