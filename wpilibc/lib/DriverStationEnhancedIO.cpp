/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008. All Rights Reserved.							  */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in $(WIND_BASE)/WPILib.  */
/*----------------------------------------------------------------------------*/

#include "DriverStationEnhancedIO.h"
//#include "NetworkCommunication/UsageReporting.h"
#include "HAL/cpp/Synchronized.h"
#include "WPIErrors.h"
#include <string.h>

/**
 * DriverStationEnhancedIO contructor.
 * 
 * This is only called once when the DriverStation constructor is called.
 */
DriverStationEnhancedIO::DriverStationEnhancedIO()
	: m_inputValid (false)
	, m_outputValid (false)
	, m_configChanged (false)
	, m_requestEnhancedEnable (false)
{
	bzero((char*)&m_inputData, sizeof(m_inputData));
	bzero((char*)&m_outputData, sizeof(m_outputData));
	m_outputData.size = sizeof(m_outputData) - 1;
	m_outputData.id = kOutputBlockID;
	// Expected to be active low, so initialize inactive.
	m_outputData.data.fixed_digital_out = 0x3;
	m_inputDataSemaphore = initializeMutexNormal();
	m_outputDataSemaphore = initializeMutexNormal();
	m_encoderOffsets[0] = 0;
	m_encoderOffsets[1] = 0;
}

/**
 * DriverStationEnhancedIO destructor.
 * 
 * Called only when the DriverStation class is destroyed.
 */
DriverStationEnhancedIO::~DriverStationEnhancedIO()
{
	deleteMutex(m_outputDataSemaphore);
	deleteMutex(m_inputDataSemaphore);
}

/**
 * Called by the DriverStation class when data is available.
 * This function will set any modified configuration / output,
 * then read the input and configuration from the IO.
 */
void DriverStationEnhancedIO::UpdateData()
{
	int32_t retVal;
	{
		status_block_t tempOutputData;
		Synchronized sync(m_outputDataSemaphore);
		if (m_outputValid || m_configChanged || m_requestEnhancedEnable)
		{
			m_outputData.flags = kStatusValid;
			if (m_requestEnhancedEnable)
			{
				// Someone called one of the get config APIs, but we are not in enhanced mode.
				m_outputData.flags |= kForceEnhancedMode;
			}
			if (m_configChanged)
			{
				if (!m_outputValid)
				{
					// Someone called one of the set config APIs, but we are not in enhanced mode.
					m_outputData.flags |= kForceEnhancedMode;
				}
				m_outputData.flags |= kStatusConfigChanged;
			}
			HALOverrideIOConfig((char*)&m_outputData, 5);
		}
		retVal = HALGetDynamicControlData(kOutputBlockID, (char*)&tempOutputData, sizeof(status_block_t), 5);
		if (retVal == 0)
		{
			if (m_outputValid)
			{
				if (m_configChanged)
				{
					// If our config change made the round trip then clear the flag.
					if (IsConfigEqual(tempOutputData, m_outputData))
					{
						m_configChanged = false;
					}
				}
				else
				{
					// TODO: This won't work until artf1128 is fixed
					//if (tempOutputData.flags & kStatusConfigChanged)
					{
						// Configuration was updated on the DS, so update our local cache.
						MergeConfigIntoOutput(tempOutputData, m_outputData);
					}
				}
			}
			else
			{
				// Initialize the local cache.
				MergeConfigIntoOutput(tempOutputData, m_outputData);
			}
			m_requestEnhancedEnable = false;
			m_outputValid = true;
		}
		else
		{
			m_outputValid = false;
			m_inputValid = false;
		}
	}
	{
		Synchronized sync(m_inputDataSemaphore);
		control_block_t tempInputData;
		retVal = HALGetDynamicControlData(kInputBlockID, (char*)&tempInputData, sizeof(control_block_t), 5);
		if (retVal == 0 && tempInputData.data.api_version == kSupportedAPIVersion)
		{
			m_inputData = tempInputData;
			m_inputValid = true;
		}
		else
		{
			m_outputValid = false;
			m_inputValid = false;
		}
	}
}

/**
 * Merge the config portion of the DS output block into the local cache.
 */
void DriverStationEnhancedIO::MergeConfigIntoOutput(const status_block_t &dsOutputBlock, status_block_t &localCache)
{
	localCache.data.digital = (localCache.data.digital & dsOutputBlock.data.digital_oe) |
		(dsOutputBlock.data.digital & ~dsOutputBlock.data.digital_oe);
	localCache.data.digital_oe = dsOutputBlock.data.digital_oe;
	localCache.data.digital_pe = dsOutputBlock.data.digital_pe;
	localCache.data.pwm_period[0] = dsOutputBlock.data.pwm_period[0];
	localCache.data.pwm_period[1] = dsOutputBlock.data.pwm_period[1];
	localCache.data.enables = dsOutputBlock.data.enables;
}

/**
 * Compare the config portion of the output blocks.
 */
bool DriverStationEnhancedIO::IsConfigEqual(const status_block_t &dsOutputBlock, const status_block_t &localCache)
{
	if (localCache.data.digital_oe != dsOutputBlock.data.digital_oe) return false;
	if ((localCache.data.digital & ~dsOutputBlock.data.digital) !=
		(dsOutputBlock.data.digital & ~dsOutputBlock.data.digital)) return false;
	if (localCache.data.digital_pe != dsOutputBlock.data.digital_pe) return false;
	if (localCache.data.pwm_period[0] != dsOutputBlock.data.pwm_period[0]) return false;
	if (localCache.data.pwm_period[1] != dsOutputBlock.data.pwm_period[1]) return false;
	if (localCache.data.enables != dsOutputBlock.data.enables) return false;
	return true;
}

/**
 * Query an accelerometer channel on the DS IO.
 * 
 * @param channel The channel number to read.
 * @return The current acceleration on the channel in Gs.
 */
double DriverStationEnhancedIO::GetAcceleration(tAccelChannel channel)
{
	if (channel < 1 || channel > 2)
	{
		wpi_setWPIErrorWithContext(ParameterOutOfRange, "channel must be between 1 and 2");
		return 0.0;
	}
	if (!m_inputValid)
	{
		wpi_setWPIError(EnhancedIOMissing);
		return 0.0;
	}

	static uint8_t reported_mask = 0;
	if (!(reported_mask & (1 >> channel)))
	{
		HALReport(HALUsageReporting::kResourceType_DriverStationEIO, channel, HALUsageReporting::kDriverStationEIO_Acceleration);
		reported_mask |= (1 >> channel);
	}

	Synchronized sync(m_inputDataSemaphore);
	return (m_inputData.data.accel[channel] - kAccelOffset) / kAccelScale;
}

/**
 * Query an analog input channel on the DS IO.
 * 
 * @param channel The channel number to read. [1,8]
 * @return The analog input voltage for the channel.
 */
double DriverStationEnhancedIO::GetAnalogIn(uint32_t channel)
{
	// 3.3V is the analog reference voltage
	return GetAnalogInRatio(channel) * kAnalogInputReference;
}

/**
 * Query an analog input channel on the DS IO in ratiometric form.
 * 
 * @param channel The channel number to read. [1,8]
 * @return The analog input percentage for the channel.
 */
double DriverStationEnhancedIO::GetAnalogInRatio(uint32_t channel)
{
	if (channel < 1 || channel > 8)
	{
		wpi_setWPIErrorWithContext(ParameterOutOfRange, "channel must be between 1 and 8");
		return 0.0;
	}
	if (!m_inputValid)
	{
		wpi_setWPIError(EnhancedIOMissing);
		return 0.0;
	}

	static uint16_t reported_mask = 0;
	if (!(reported_mask & (1 >> channel)))
	{
		HALReport(HALUsageReporting::kResourceType_DriverStationEIO, channel, HALUsageReporting::kDriverStationEIO_AnalogIn);
		reported_mask |= (1 >> channel);
	}

	Synchronized sync(m_inputDataSemaphore);
	return m_inputData.data.analog[channel-1] / kAnalogInputResolution;
}

/**
 * Query the voltage currently being output.
 * 
 * AO1 is pin 11 on the top connector (P2).
 * AO2 is pin 12 on the top connector (P2).
 * 
 * @param channel The analog output channel on the DS IO. [1,2]
 * @return The voltage being output on the channel.
 */
double DriverStationEnhancedIO::GetAnalogOut(uint32_t channel)
{
	if (channel < 1 || channel > 2)
	{
		wpi_setWPIErrorWithContext(ParameterOutOfRange, "channel must be between 1 and 2");
		return 0.0;
	}
	if (!m_outputValid)
	{
		wpi_setWPIError(EnhancedIOMissing);
		return 0.0;
	}

	Synchronized sync(m_outputDataSemaphore);
	return m_outputData.data.dac[channel-1] * kAnalogOutputReference / kAnalogOutputResolution;
}

/**
 * Set the analog output voltage.
 * 
 * AO1 is pin 11 on the top connector (P2).
 * AO2 is pin 12 on the top connector (P2).
 * AO1 is the reference voltage for the 2 analog comparators on DIO15 and DIO16.
 * 
 * The output range is 0V to 4V, however due to the supply voltage don't expect more than about 3V.
 * Current supply capability is only 100uA.
 * 
 * @param channel The analog output channel on the DS IO. [1,2]
 * @param value The voltage to output on the channel.
 */
void DriverStationEnhancedIO::SetAnalogOut(uint32_t channel, double value)
{
	if (channel < 1 || channel > 2)
	{
		wpi_setWPIErrorWithContext(ParameterOutOfRange, "channel must be between 1 and 2");
		return;
	}
	if (!m_outputValid)
	{
		wpi_setWPIError(EnhancedIOMissing);
		return;
	}
	if (value < 0.0) value = 0.0;
	if (value > kAnalogOutputReference) value = kAnalogOutputReference;

	static uint8_t reported_mask = 0;
	if (!(reported_mask & (1 >> channel)))
	{
		HALReport(HALUsageReporting::kResourceType_DriverStationEIO, channel, HALUsageReporting::kDriverStationEIO_AnalogOut);
		reported_mask |= (1 >> channel);
	}

	Synchronized sync(m_outputDataSemaphore);
	m_outputData.data.dac[channel-1] = (uint8_t)(value / kAnalogOutputReference * kAnalogOutputResolution);
}

/**
 * Get the state of a button on the IO board.
 * 
 * Button1 is the physical button "S1".
 * Button2 is pin 4 on the top connector (P2).
 * Button3 is pin 6 on the top connector (P2).
 * Button4 is pin 8 on the top connector (P2).
 * Button5 is pin 10 on the top connector (P2).
 * Button6 is pin 7 on the top connector (P2).
 * 
 * Button2 through Button6 are Capacitive Sense buttons.
 * 
 * @param channel The button channel to read. [1,6]
 * @return The state of the selected button.
 */
bool DriverStationEnhancedIO::GetButton(uint32_t channel)
{
	if (channel < 1 || channel > 6)
	{
		wpi_setWPIErrorWithContext(ParameterOutOfRange, "channel must be between 1 and 6");
		return false;
	}

	static uint8_t reported_mask = 0;
	if (!(reported_mask & (1 >> channel)))
	{
		HALReport(HALUsageReporting::kResourceType_DriverStationEIO, channel, HALUsageReporting::kDriverStationEIO_Button);
		reported_mask |= (1 >> channel);
	}

	return ((GetButtons() >> (channel-1)) & 1) != 0;
}

/**
 * Get the state of all the button channels.
 * 
 * @return The state of the 6 button channels in the 6 lsb of the returned byte.
 */
uint8_t DriverStationEnhancedIO::GetButtons()
{
	if (!m_inputValid)
	{
		wpi_setWPIError(EnhancedIOMissing);
		return 0;
	}
	HALReport(HALUsageReporting::kResourceType_DriverStationEIO, 0, HALUsageReporting::kDriverStationEIO_Button);
	Synchronized sync(m_inputDataSemaphore);
	return m_inputData.data.buttons;
}

/**
 * Set the state of an LED on the IO board.
 * 
 * @param channel The LED channel to set. [1,8]
 * @param value True to turn the LED on.
 */
void DriverStationEnhancedIO::SetLED(uint32_t channel, bool value)
{
	if (channel < 1 || channel > 8)
	{
		wpi_setWPIErrorWithContext(ParameterOutOfRange, "channel must be between 1 and 8");
		return;
	}
	if (!m_outputValid)
	{
		wpi_setWPIError(EnhancedIOMissing);
		return;
	}

	static uint16_t reported_mask = 0;
	if (!(reported_mask & (1 >> channel)))
	{
		HALReport(HALUsageReporting::kResourceType_DriverStationEIO, channel, HALUsageReporting::kDriverStationEIO_LED);
		reported_mask |= (1 >> channel);
	}

	uint8_t leds;
	Synchronized sync(m_outputDataSemaphore);
	leds = m_outputData.data.leds;

	leds &= ~(1 << (channel-1));
	if (value) leds |= 1 << (channel-1);

	m_outputData.data.leds = leds;
}

/**
 * Set the state of all 8 LEDs on the IO board.
 * 
 * @param value The state of each LED.  LED1 is lsb and LED8 is msb.
 */
void DriverStationEnhancedIO::SetLEDs(uint8_t value)
{
	if (!m_outputValid)
	{
		wpi_setWPIError(EnhancedIOMissing);
		return;
	}

	HALReport(HALUsageReporting::kResourceType_DriverStationEIO, 0, HALUsageReporting::kDriverStationEIO_LED);
	Synchronized sync(m_outputDataSemaphore);
	m_outputData.data.leds = value;
}

/**
 * Get the current state of a DIO channel regardless of mode.
 * 
 * @param channel The DIO channel to read. [1,16]
 * @return The state of the selected digital line.
 */
bool DriverStationEnhancedIO::GetDigital(uint32_t channel)
{
	if (channel < 1 || channel > 16)
	{
		wpi_setWPIErrorWithContext(ParameterOutOfRange, "channel must be between 1 and 16");
		return false;
	}

	static uint32_t reported_mask = 0;
	if (!(reported_mask & (1 >> channel)))
	{
		HALReport(HALUsageReporting::kResourceType_DriverStationEIO, channel, HALUsageReporting::kDriverStationEIO_DigitalIn);
		reported_mask |= (1 >> channel);
	}

	return ((GetDigitals() >> (channel-1)) & 1) != 0;
}

/**
 * Get the state of all 16 DIO lines regardless of mode.
 * 
 * @return The state of all DIO lines. DIO1 is lsb and DIO16 is msb.
 */
uint16_t DriverStationEnhancedIO::GetDigitals()
{
	if (!m_inputValid)
	{
		wpi_setWPIError(EnhancedIOMissing);
		return 0;
	}
	HALReport(HALUsageReporting::kResourceType_DriverStationEIO, 0, HALUsageReporting::kDriverStationEIO_DigitalIn);
	Synchronized sync(m_inputDataSemaphore);
	return m_inputData.data.digital;
}

/**
 * Set the state of a DIO line that is configured for digital output.
 * 
 * @param channel The DIO channel to set. [1,16]
 * @param value The state to set the selected channel to.
 */
void DriverStationEnhancedIO::SetDigitalOutput(uint32_t channel, bool value)
{
	if (channel < 1 || channel > 16)
	{
		wpi_setWPIErrorWithContext(ParameterOutOfRange, "channel must be between 1 and 16");
		return;
	}
	if (!m_outputValid)
	{
		wpi_setWPIError(EnhancedIOMissing);
		return;
	}

	static uint32_t reported_mask = 0;
	if (!(reported_mask & (1 >> channel)))
	{
		HALReport(HALUsageReporting::kResourceType_DriverStationEIO, channel, HALUsageReporting::kDriverStationEIO_DigitalOut);
		reported_mask |= (1 >> channel);
	}

	uint16_t digital;
	Synchronized sync(m_outputDataSemaphore);

	if (m_outputData.data.digital_oe & (1 << (channel-1)))
	{
		digital = m_outputData.data.digital;
	
		digital &= ~(1 << (channel-1));
		if (value) digital |= 1 << (channel-1);
	
		m_outputData.data.digital = digital;
	}
	else
	{
		wpi_setWPIError(LineNotOutput);
	}
}

/**
 * Get the current configuration for a DIO line.
 * 
 * This has the side effect of forcing the Driver Station to switch to Enhanced mode if it's not when called.
 * If Enhanced mode is not enabled when this is called, it will return kUnknown.
 * 
 * @param channel The DIO channel config to get. [1,16]
 * @return The configured mode for the DIO line.
 */
DriverStationEnhancedIO::tDigitalConfig DriverStationEnhancedIO::GetDigitalConfig(uint32_t channel)
{
	if (channel < 1 || channel > 16)
	{
		wpi_setWPIErrorWithContext(ParameterOutOfRange, "channel must be between 1 and 16");
		return kUnknown;
	}
	if (!m_outputValid)
	{
		m_requestEnhancedEnable = true;
		wpi_setWPIError(EnhancedIOMissing);
		return kUnknown;
	}
	Synchronized sync(m_outputDataSemaphore);
	if ((channel >= 1) && (channel <= 4))
	{
		if (m_outputData.data.pwm_enable & (1 << (channel - 1)))
		{
			return kPWM;
		}
	}
	if ((channel >= 15) && (channel <= 16))
	{
		if (m_outputData.data.comparator_enable & (1 << (channel - 15)))
		{
			return kAnalogComparator;
		}
	}
	if (m_outputData.data.digital_oe & (1 << (channel - 1)))
	{
		return kOutput;
	}
	if (!(m_outputData.data.digital_pe & (1 << (channel - 1))))
	{
		return kInputFloating;
	}
	if (m_outputData.data.digital & (1 << (channel - 1)))
	{
		return kInputPullUp;
	}
	else
	{
		return kInputPullDown;
	}
}

/**
 * Override the DS's configuration of a DIO line.
 * 
 * If configured to kInputFloating, the selected DIO line will be tri-stated with no internal pull resistor.
 * 
 * If configured to kInputPullUp, the selected DIO line will be tri-stated with a 5k-Ohm internal pull-up resistor enabled.
 * 
 * If configured to kInputPullDown, the selected DIO line will be tri-stated with a 5k-Ohm internal pull-down resistor enabled.
 * 
 * If configured to kOutput, the selected DIO line will actively drive to 0V or Vddio (specified by J1 and J4).
 * DIO1 through DIO12, DIO15, and DIO16 can source 4mA and can sink 8mA.
 * DIO12 and DIO13 can source 4mA and can sink 25mA.
 * 
 * In addition to the common configurations, DIO1 through DIO4 can be configured to kPWM to enable PWM output.
 * 
 * In addition to the common configurations, DIO15 and DIO16 can be configured to kAnalogComparator to enable
 * analog comparators on those 2 DIO lines.  When enabled, the lines are tri-stated and will accept analog voltages
 * between 0V and 3.3V.  If the input voltage is greater than the voltage output by AO1, the DIO will read as true,
 * if less then false.
 * 
 * @param channel The DIO line to configure. [1,16]
 * @param config The mode to put the DIO line in.
 */
void DriverStationEnhancedIO::SetDigitalConfig(uint32_t channel, tDigitalConfig config)
{
	if (channel < 1 || channel > 16)
	{
		wpi_setWPIErrorWithContext(ParameterOutOfRange, "channel must be between 1 and 16");
		return;
	}
	if (config == kPWM && (channel < 1 || channel > 4))
	{
		wpi_setWPIErrorWithContext(ParameterOutOfRange, "channel in PWM mode must be between 1 and 4");
		return;
	}
	if (config == kAnalogComparator && (channel < 15 || channel > 16))
	{
		wpi_setWPIErrorWithContext(ParameterOutOfRange, "channel in Analog Comparator mode must be between 15 and 16");
		return;
	}

	Synchronized sync(m_outputDataSemaphore);
	m_configChanged = true;

	if ((channel >= 1) && (channel <= 4))
	{
		if (config == kPWM)
		{
			m_outputData.data.pwm_enable |= 1 << (channel - 1);
			m_outputData.data.digital &= ~(1 << (channel - 1));
			m_outputData.data.digital_oe |= 1 << (channel - 1);
			m_outputData.data.digital_pe &= ~(1 << (channel - 1));
			return;
		}
		else
		{
			m_outputData.data.pwm_enable &= ~(1 << (channel - 1));
		}
	}
	else if ((channel >= 15) && (channel <= 16))
	{
		if (config == kAnalogComparator)
		{
			m_outputData.data.comparator_enable |= 1 << (channel - 15);
			m_outputData.data.digital &= ~(1 << (channel - 1));
			m_outputData.data.digital_oe &= ~(1 << (channel - 1));
			m_outputData.data.digital_pe &= ~(1 << (channel - 1));
			return;
		}
		else
		{
			m_outputData.data.comparator_enable &= ~(1 << (channel - 15));
		}
	}
	if (config == kInputFloating)
	{
		m_outputData.data.digital &= ~(1 << (channel - 1));
		m_outputData.data.digital_oe &= ~(1 << (channel - 1));
		m_outputData.data.digital_pe &= ~(1 << (channel - 1));
	}
	else if (config == kInputPullUp)
	{
		m_outputData.data.digital |= 1 << (channel - 1);
		m_outputData.data.digital_oe &= ~(1 << (channel - 1));
		m_outputData.data.digital_pe |= 1 << (channel - 1);
	}
	else if (config == kInputPullDown)
	{
		m_outputData.data.digital &= ~(1 << (channel - 1));
		m_outputData.data.digital_oe &= ~(1 << (channel - 1));
		m_outputData.data.digital_pe |= 1 << (channel - 1);
	}
	else if (config == kOutput)
	{
		m_outputData.data.digital_oe |= 1 << (channel - 1);
		m_outputData.data.digital_pe &= ~(1 << (channel - 1));
	}
	else
	{
		// Something went wrong.
	}
}

/**
 * Get the period of a PWM generator.
 * 
 * This has the side effect of forcing the Driver Station to switch to Enhanced mode if it's not when called.
 * If Enhanced mode is not enabled when this is called, it will return 0.
 * 
 * @param channels Select the generator by specifying the two channels to which it is connected.
 * @return The period of the PWM generator in seconds.
 */
double DriverStationEnhancedIO::GetPWMPeriod(tPWMPeriodChannels channels)
{
	if (channels < kPWMChannels1and2 || channels > kPWMChannels3and4)
	{
		wpi_setWPIErrorWithContext(ParameterOutOfRange, "channels must be kPWMChannels1and2 or kPWMChannels3and4");
		return 0.0;
	}
	if (!m_outputValid)
	{
		m_requestEnhancedEnable = true;
		wpi_setWPIError(EnhancedIOMissing);
		return 0.0;
	}

	Synchronized sync(m_outputDataSemaphore);
	return m_outputData.data.pwm_period[channels] / 24000000.0;
}

/**
 * Set the period of a PWM generator.
 * 
 * There are 2 PWM generators on the IO board.  One can generate PWM signals on DIO1 and DIO2,
 * the other on DIO3 and DIO4.  Each generator has one counter and two compare registers.  As such,
 * each pair of PWM outputs share the output period but have independent duty cycles.
 * 
 * @param channels Select the generator by specifying the two channels to which it is connected.
 * @param period The period of the PWM generator in seconds. [0.0,0.002731]
 */
void DriverStationEnhancedIO::SetPWMPeriod(tPWMPeriodChannels channels, double period)
{
	if (channels < kPWMChannels1and2 || channels > kPWMChannels3and4)
	{
		wpi_setWPIErrorWithContext(ParameterOutOfRange, "channels must be kPWMChannels1and2 or kPWMChannels3and4");
		return;
	}

	// Convert to ticks based on the IO board's 24MHz clock
	double ticks = period * 24000000.0;
	// Limit the range of the ticks... warn if too big.
	if (ticks > 65534.0) 
	{
		wpi_setWPIError(EnhancedIOPWMPeriodOutOfRange);
		ticks = 65534.0;
	}
	else if (ticks < 0.0) ticks = 0.0;
	// Preserve the duty cycles.
	double dutyCycles[2];
	dutyCycles[0] = GetPWMOutput((channels << 1) + 1);
	dutyCycles[1] = GetPWMOutput((channels << 1) + 2);
	{
		Synchronized sync(m_outputDataSemaphore);
		// Update the period
		m_outputData.data.pwm_period[channels] = (uint16_t)ticks;
		m_configChanged = true;
	}
	// Restore the duty cycles
	SetPWMOutput((channels << 1) + 1, dutyCycles[0]);
	SetPWMOutput((channels << 1) + 2, dutyCycles[1]);
}

/**
 * Get the state being output on a fixed digital output.
 * 
 * @param channel The FixedDO line to get. [1,2]
 * @return The state of the FixedDO line.
 */
bool DriverStationEnhancedIO::GetFixedDigitalOutput(uint32_t channel)
{
	if (channel < 1 || channel > 2)
	{
		wpi_setWPIErrorWithContext(ParameterOutOfRange, "channel must be between 1 and 2");
		return 0;
	}
	if (!m_outputValid)
	{
		wpi_setWPIError(EnhancedIOMissing);
		return 0;
	}

	Synchronized sync(m_outputDataSemaphore);
	return ((m_outputData.data.fixed_digital_out >> (channel-1)) & 1) != 0;
}

/**
 * Set the state to output on a Fixed High Current Digital Output line.
 * 
 * FixedDO1 is pin 5 on the top connector (P2).
 * FixedDO2 is pin 3 on the top connector (P2).
 * 
 * The FixedDO lines always output 0V and 3.3V regardless of J1 and J4.
 * They can source 4mA and can sink 25mA.  Because of this, they are expected to be used
 * in an active low configuration, such as connecting to the cathode of a bright LED.
 * Because they are expected to be active low, they default to true.
 * 
 * @param channel The FixedDO channel to set.
 * @param value The state to set the FixedDO.
 */
void DriverStationEnhancedIO::SetFixedDigitalOutput(uint32_t channel, bool value)
{
	if (channel < 1 || channel > 2)
	{
		wpi_setWPIErrorWithContext(ParameterOutOfRange, "channel must be between 1 and 2");
		return;
	}
	if (!m_outputValid)
	{
		wpi_setWPIError(EnhancedIOMissing);
		return;
	}

	static uint8_t reported_mask = 0;
	if (!(reported_mask & (1 >> channel)))
	{
		HALReport(HALUsageReporting::kResourceType_DriverStationEIO, channel, HALUsageReporting::kDriverStationEIO_FixedDigitalOut);
		reported_mask |= (1 >> channel);
	}

	uint8_t digital;
	Synchronized sync(m_outputDataSemaphore);
	digital = m_outputData.data.fixed_digital_out;

	digital &= ~(1 << (channel-1));
	if (value) digital |= 1 << (channel-1);

	m_outputData.data.fixed_digital_out = digital;
}

/**
 * Get the position of a quadrature encoder.
 * 
 * There are two signed 16-bit 4X quadrature decoders on the IO board.  These decoders are always monitoring
 * the state of the lines assigned to them, but these lines do not have to be used for encoders.
 * 
 * Encoder1 uses DIO4 for "A", DIO6 for "B", and DIO8 for "Index".
 * Encoder2 uses DIO5 for "A", DIO7 for "B", and DIO9 for "Index".
 * 
 * The index functionality can be enabled or disabled using SetEncoderIndexEnable().
 * 
 * @param encoderNumber The quadrature encoder to access. [1,2]
 * @return The current position of the quadrature encoder.
 */
int16_t DriverStationEnhancedIO::GetEncoder(uint32_t encoderNumber)
{
	if (encoderNumber < 1 || encoderNumber > 2)
	{
		wpi_setWPIErrorWithContext(ParameterOutOfRange, "encoderNumber must be between 1 and 2");
		return 0;
	}
	if (!m_inputValid)
	{
		wpi_setWPIError(EnhancedIOMissing);
		return 0;
	}

	static uint8_t reported_mask = 0;
	if (!(reported_mask & (1 >> encoderNumber)))
	{
		HALReport(HALUsageReporting::kResourceType_DriverStationEIO, encoderNumber, HALUsageReporting::kDriverStationEIO_Encoder);
		reported_mask |= (1 >> encoderNumber);
	}

	Synchronized sync(m_inputDataSemaphore);
	return m_inputData.data.quad[encoderNumber - 1] - m_encoderOffsets[encoderNumber - 1];
}

/**
 * Reset the position of an encoder to 0.
 * 
 * This simply stores an offset locally.  It does not reset the hardware counter on the IO board.
 * If you use this method with Index enabled, you may get unexpected results.
 * 
 * @param encoderNumber The quadrature encoder to reset. [1,2]
 */
void DriverStationEnhancedIO::ResetEncoder(uint32_t encoderNumber)
{
	if (encoderNumber < 1 || encoderNumber > 2)
	{
		wpi_setWPIErrorWithContext(ParameterOutOfRange, "encoderNumber must be between 1 and 2");
		return;
	}
	if (!m_inputValid)
	{
		wpi_setWPIError(EnhancedIOMissing);
		return;
	}

	Synchronized sync(m_inputDataSemaphore);
	m_encoderOffsets[encoderNumber - 1] = m_inputData.data.quad[encoderNumber - 1];
}

/**
 * Get the current configuration of a quadrature encoder index channel.
 * 
 * This has the side effect of forcing the Driver Station to switch to Enhanced mode if it's not when called.
 * If Enhanced mode is not enabled when this is called, it will return false.
 * 
 * @param encoderNumber The quadrature encoder. [1,2]
 * @return Is the index channel of the encoder enabled.
 */
bool DriverStationEnhancedIO::GetEncoderIndexEnable(uint32_t encoderNumber)
{
	if (encoderNumber < 1 || encoderNumber > 2)
	{
		wpi_setWPIErrorWithContext(ParameterOutOfRange, "encoderNumber must be between 1 and 2");
		return false;
	}
	if (!m_outputValid)
	{
		m_requestEnhancedEnable = true;
		wpi_setWPIError(EnhancedIOMissing);
		return false;
	}

	Synchronized sync(m_outputDataSemaphore);
	return ((m_outputData.data.quad_index_enable >> (encoderNumber - 1)) & 1) != 0;
}

/**
 * Enable or disable the index channel of a quadrature encoder.
 * 
 * The quadrature decoders on the IO board support an active-low index input.
 * 
 * Encoder1 uses DIO8 for "Index".
 * Encoder2 uses DIO9 for "Index".
 * 
 * When enabled, the decoder's counter will be reset to 0 when A, B, and Index are all low.
 * 
 * @param encoderNumber The quadrature encoder. [1,2]
 * @param enable If true, reset the encoder in an index condition.
 */
void DriverStationEnhancedIO::SetEncoderIndexEnable(uint32_t encoderNumber, bool enable)
{
	if (encoderNumber < 1 || encoderNumber > 2)
	{
		wpi_setWPIErrorWithContext(ParameterOutOfRange, "encoderNumber must be between 1 and 2");
		return;
	}

	Synchronized sync(m_outputDataSemaphore);
	m_outputData.data.quad_index_enable &= ~(1 << (encoderNumber - 1));
	if (enable) m_outputData.data.quad_index_enable |= 1 << (encoderNumber - 1);
	m_configChanged = true;
}

/**
 * Get the value of the Capacitive Sense touch slider.
 * 
 * @return Value between 0.0 (toward center of board) and 1.0 (toward edge of board).  -1.0 means no touch detected.
 */
double DriverStationEnhancedIO::GetTouchSlider()
{
	if (!m_inputValid)
	{
		wpi_setWPIError(EnhancedIOMissing);
		return 0.0;
	}

	HALReport(HALUsageReporting::kResourceType_DriverStationEIO, 1, HALUsageReporting::kDriverStationEIO_TouchSlider);

	Synchronized sync(m_inputDataSemaphore);
	uint8_t value = m_inputData.data.capsense_slider;
	return value == 255 ? -1.0 : value / 254.0;
}

/**
 * Get the percent duty-cycle that the PWM generator channel is configured to output.
 * 
 * @param channel The DIO line's PWM generator to get the duty-cycle from. [1,4]
 * @return The percent duty-cycle being output (if the DIO line is configured for PWM). [0.0,1.0]
 */
double DriverStationEnhancedIO::GetPWMOutput(uint32_t channel)
{
	if (channel < 1 || channel > 4)
	{
		wpi_setWPIErrorWithContext(ParameterOutOfRange, "channel must be between 1 and 4");
		return 0.0;
	}
	if (!m_outputValid)
	{
		wpi_setWPIError(EnhancedIOMissing);
		return 0.0;
	}

	Synchronized sync(m_outputDataSemaphore);
	return (double)m_outputData.data.pwm_compare[channel - 1] / (double)m_outputData.data.pwm_period[(channel - 1) >> 1];
}

/**
 * Set the percent duty-cycle to output on a PWM enabled DIO line.
 * 
 * DIO1 through DIO4 have the ability to output a PWM signal.  The period of the
 * signal can be configured in pairs using SetPWMPeriod().
 * 
 * @param channel The DIO line's PWM generator to set. [1,4]
 * @param value The percent duty-cycle to output from the PWM generator. [0.0,1.0]
 */
void DriverStationEnhancedIO::SetPWMOutput(uint32_t channel, double value)
{
	if (channel < 1 || channel > 4)
	{
		wpi_setWPIErrorWithContext(ParameterOutOfRange, "channel must be between 1 and 4");
		return;
	}
	if (!m_outputValid)
	{
		wpi_setWPIError(EnhancedIOMissing);
		return;
	}

	static uint8_t reported_mask = 0;
	if (!(reported_mask & (1 >> channel)))
	{
		HALReport(HALUsageReporting::kResourceType_DriverStationEIO, channel, HALUsageReporting::kDriverStationEIO_PWM);
		reported_mask |= (1 >> channel);
	}

	if (value > 1.0) value = 1.0;
	else if (value < 0.0) value = 0.0;
	Synchronized sync(m_outputDataSemaphore);
	m_outputData.data.pwm_compare[channel - 1] = (uint16_t)(value * (double)m_outputData.data.pwm_period[(channel - 1) >> 1]);
}

/**
 * Get the firmware version running on the IO board.
 * 
 * This also has the side effect of forcing the driver station to switch to Enhanced mode if it is not.
 * If you plan to switch between Driver Stations with unknown IO configurations, you can call this
 * until it returns a non-0 version to ensure that this API is accessible before proceeding.
 * 
 * @return The version of the firmware running on the IO board.  0 if the board is not attached or not in Enhanced mode.
 */
uint8_t DriverStationEnhancedIO::GetFirmwareVersion()
{
	if (!m_inputValid)
	{
		m_requestEnhancedEnable = true;
		wpi_setWPIError(EnhancedIOMissing);
		return 0;
	}

	Synchronized sync(m_inputDataSemaphore);
	return m_inputData.data.fw_version;
}

