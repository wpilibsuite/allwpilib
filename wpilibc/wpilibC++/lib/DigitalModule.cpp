/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008. All Rights Reserved.							  */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in $(WIND_BASE)/WPILib.  */
/*----------------------------------------------------------------------------*/

#include "DigitalModule.h"
#include "I2C.h"
#include "PWM.h"
#include "HAL/cpp/Synchronized.hpp"
#include "WPIErrors.h"
#include <math.h>

/**
 * Get an instance of an Digital Module.
 * Singleton digital module creation where a module is allocated on the first use
 * and the same module is returned on subsequent uses.
 *
 * @param moduleNumber The digital module to get (1 or 2).
 */
DigitalModule* DigitalModule::GetInstance(uint8_t moduleNumber)
{
	if (checkDigitalModule(moduleNumber))
	{
		return (DigitalModule *)GetModule(nLoadOut::kModuleType_Digital, moduleNumber);
	}

	// If this wasn't caught before now, make sure we say what's wrong before we crash
	char buf[64];
	snprintf(buf, 64, "Digital Module %d", moduleNumber);
	wpi_setGlobalWPIErrorWithContext(ModuleIndexOutOfRange, buf);

	return NULL;
}

/**
 * Create a new instance of an digital module.
 * Create an instance of the digital module object. Initialize all the parameters
 * to reasonable values on start.
 * Setting a global value on an digital module can be done only once unless subsequent
 * values are set the previously set value.
 * Digital modules are a singleton, so the constructor is never called outside of this class.
 *
 * @param moduleNumber The digital module to create (1 or 2).
 */
DigitalModule::DigitalModule(uint8_t moduleNumber)
	: Module(nLoadOut::kModuleType_Digital, moduleNumber)
{
	// TODO: Refactor out common logic
  	m_module = moduleNumber;
  	for (uint32_t i = 0; i < kDigitalChannels; i++)
	{
	  void* port = getPortWithModule(moduleNumber, i);
	  int32_t status = 0;
	  m_digital_ports[i] = initializeDigitalPort(port, &status);
	  wpi_setErrorWithContext(status, getHALErrorMessage(status));
	}
  	for (uint32_t i = 0; i < kRelayChannels; i++)
	{
	  void* port = getPortWithModule(moduleNumber, i);
	  int32_t status = 0;
	  m_relay_ports[i] = initializeDigitalPort(port, &status);
	  wpi_setErrorWithContext(status, getHALErrorMessage(status));
	}
  	for (uint32_t i = 0; i < kPwmChannels; i++)
	{
	  void* port = getPortWithModule(moduleNumber, i);
	  int32_t status = 0;
	  m_pwm_ports[i] = initializeDigitalPort(port, &status);
	  wpi_setErrorWithContext(status, getHALErrorMessage(status));
	}
	AddToSingletonList();
}

DigitalModule::~DigitalModule()
{
}

/**
 * Set a PWM channel to the desired value. The values range from 0 to 255 and the period is controlled
 * by the PWM Period and MinHigh registers.
 * 
 * @param channel The PWM channel to set.
 * @param value The PWM value to set.
 */
void DigitalModule::SetPWM(uint32_t channel, unsigned short value)
{
  	int32_t status = 0;
	setPWM(m_pwm_ports[channel], value, &status);
	wpi_setErrorWithContext(status, getHALErrorMessage(status));
}

/**
 * Get a value from a PWM channel.
 * 
 * @param channel The PWM channel to read from.
 * @return The raw PWM value.
 */
unsigned short DigitalModule::GetPWM(uint32_t channel)
{
  	int32_t status = 0;
	uint16_t value = getPWM(m_pwm_ports[channel], &status);
	wpi_setErrorWithContext(status, getHALErrorMessage(status));
	return value;
}

/**
 * Set how how often the PWM signal is squelched, thus scaling the period.
 * 
 * @param channel The PWM channel to configure.
 * @param squelchMask The 2-bit mask of outputs to squelch.
 */
void DigitalModule::SetPWMPeriodScale(uint32_t channel, uint32_t squelchMask)
{
	int32_t status = 0;
	setPWMPeriodScale(m_pwm_ports[channel], squelchMask, &status);
	wpi_setErrorWithContext(status, getHALErrorMessage(status));
}

/**
 * Set the state of a relay.
 * Set the state of a relay output to be forward. Relays have two outputs and each is
 * independently set to 0v or 12v.
 */
void DigitalModule::SetRelayForward(uint32_t channel, bool on)
{
	int32_t status = 0;
	setRelayForward(m_relay_ports[channel], on, &status);
	wpi_setErrorWithContext(status, getHALErrorMessage(status));
}

/**
 * Set the state of a relay.
 * Set the state of a relay output to be reverse. Relays have two outputs and each is
 * independently set to 0v or 12v.
 */
void DigitalModule::SetRelayReverse(uint32_t channel, bool on)
{
	int32_t status = 0;
	setRelayReverse(m_relay_ports[channel], on, &status);
	wpi_setErrorWithContext(status, getHALErrorMessage(status));
}

/**
 * Get the current state of the forward relay channel
 */
bool DigitalModule::GetRelayForward(uint32_t channel)
{
	int32_t status = 0;
	bool on = getRelayForward(m_relay_ports[channel], &status);
	wpi_setErrorWithContext(status, getHALErrorMessage(status));
	return on;
}

/**
 * Get the current state of all of the forward relay channels on this module.
 */
uint8_t DigitalModule::GetRelayForward()
{
	uint8_t value = 0;
	for (unsigned int i = 0; i < kRelayChannels; i++) {
		value |= GetRelayForward(i) << i;
	}
	return value;
}

/**
 * Get the current state of the reverse relay channel
 */
bool DigitalModule::GetRelayReverse(uint32_t channel)
{
	int32_t status = 0;
	bool on = getRelayReverse(m_relay_ports[channel], &status);
	wpi_setErrorWithContext(status, getHALErrorMessage(status));
	return on;
}

/**
 * Get the current state of all of the reverse relay channels on this module.
 */
uint8_t DigitalModule::GetRelayReverse()
{
	uint8_t value = 0;
	for (unsigned int i = 0; i < kRelayChannels; i++) {
		value |= GetRelayReverse(i) << i;
	}
	return value;
}


/**
 * Allocate Digital I/O channels.
 * Allocate channels so that they are not accidently reused. Also the direction is set at the
 * time of the allocation.
 * 
 * @param channel The Digital I/O channel
 * @param input If true open as input; if false open as output
 * @return Was successfully allocated
 */
bool DigitalModule::AllocateDIO(uint32_t channel, bool input)
{
	int32_t status = 0;
	bool allocated = allocateDIO(m_digital_ports[channel], input, &status);
	wpi_setErrorWithContext(status, getHALErrorMessage(status));
	return allocated;
}

/**
 * Free the resource associated with a digital I/O channel.
 * 
 * @param channel The Digital I/O channel to free
 */
void DigitalModule::FreeDIO(uint32_t channel)
{
	int32_t status = 0;
	freeDIO(m_digital_ports[channel], &status);
	wpi_setErrorWithContext(status, getHALErrorMessage(status));
}

/**
 * Write a digital I/O bit to the FPGA.
 * Set a single value on a digital I/O channel.
 * 
 * @param channel The Digital I/O channel
 * @param value The state to set the digital channel (if it is configured as an output)
 */
void DigitalModule::SetDIO(uint32_t channel, short value)
{
	int32_t status = 0;
	setDIO(m_digital_ports[channel], value, &status);
	wpi_setErrorWithContext(status, getHALErrorMessage(status));
}

/**
 * Read a digital I/O bit from the FPGA.
 * Get a single value from a digital I/O channel.
 * 
 * @param channel The digital I/O channel
 * @return The state of the specified channel
 */
bool DigitalModule::GetDIO(uint32_t channel)
{
	int32_t status = 0;
	bool value = getDIO(m_digital_ports[channel], &status);
	wpi_setErrorWithContext(status, getHALErrorMessage(status));
	return value;
}

/**
 * Read the state of all the Digital I/O lines from the FPGA
 * These are not remapped to logical order.  They are still in hardware order.
 */
uint16_t DigitalModule::GetDIO()
{
	uint16_t value = 0;
	for (unsigned int i = 0; i < kDigitalChannels; i++) {
		value |= GetDIO(i) << i;
	}
	return value;
}

/**
 * Read the direction of a the Digital I/O lines
 * A 1 bit means output and a 0 bit means input.
 * 
 * @param channel The digital I/O channel
 * @return The direction of the specified channel
 */
bool DigitalModule::GetDIODirection(uint32_t channel)
{
	int32_t status = 0;
	bool value = getDIODirection(m_digital_ports[channel], &status);
	wpi_setErrorWithContext(status, getHALErrorMessage(status));
	return value;
}

/**
 * Read the direction of all the Digital I/O lines from the FPGA
 * A 1 bit means output and a 0 bit means input.
 * These are not remapped to logical order.  They are still in hardware order.
 */
uint16_t DigitalModule::GetDIODirection()
{
	uint16_t value = 0;
	for (unsigned int i = 0; i < kDigitalChannels; i++) {
		value |= GetDIODirection(i) << i;
	}
	return value;
}

/**
 * Generate a single pulse.
 * Write a pulse to the specified digital output channel. There can only be a single pulse going at any time.
 * 
 * @param channel The Digital Output channel that the pulse should be output on
 * @param pulseLength The active length of the pulse (in seconds)
 */
void DigitalModule::Pulse(uint32_t channel, float pulseLength)
{
	int32_t status = 0;
	pulse(m_digital_ports[channel], pulseLength, &status);
	wpi_setErrorWithContext(status, getHALErrorMessage(status));
}

/**
 * Check a DIO line to see if it is currently generating a pulse.
 * 
 * @return A pulse is in progress
 */
bool DigitalModule::IsPulsing(uint32_t channel)
{
	int32_t status = 0;
	bool value = isPulsing(m_digital_ports[channel], &status);
	wpi_setErrorWithContext(status, getHALErrorMessage(status));
	return value;
}

/**
 * Check if any DIO line is currently generating a pulse.
 * 
 * @return A pulse on some line is in progress
 */
bool DigitalModule::IsPulsing()
{
	int32_t status = 0;
	bool value = isAnyPulsing(&status);
	wpi_setErrorWithContext(status, getHALErrorMessage(status));
	return value;
}

/**
 * Allocate a DO PWM Generator.
 * Allocate PWM generators so that they are not accidently reused.
 * 
 * @return PWM Generator refnum
 */
uint32_t DigitalModule::AllocateDO_PWM()
{
	int32_t status = 0;
	uint32_t value = *((uint32_t*) allocatePWMWithModule(m_module, &status)); // TODO: Hacky
	wpi_setErrorWithContext(status, getHALErrorMessage(status));
	return value;
}

/**
 * Free the resource associated with a DO PWM generator.
 * 
 * @param pwmGenerator The pwmGen to free that was allocated with AllocateDO_PWM()
 */
void DigitalModule::FreeDO_PWM(uint32_t pwmGenerator) // Note: should become void*
{
  	int32_t status = 0;
	void* generator = &pwmGenerator;
	freePWMWithModule(m_module, generator, &status);
	wpi_setErrorWithContext(status, getHALErrorMessage(status));
}

/**
 * Change the frequency of the DO PWM generator.
 * 
 * The valid range is from 0.6 Hz to 19 kHz.  The frequency resolution is logarithmic.
 * 
 * @param rate The frequency to output all digital output PWM signals on this module.
 */
void DigitalModule::SetDO_PWMRate(float rate)
{
  	int32_t status = 0;
	setPWMRateWithModule(m_module, rate, &status);
	wpi_setErrorWithContext(status, getHALErrorMessage(status));
}

/**
 * Configure which DO channel the PWM siganl is output on
 * 
 * @param pwmGenerator The generator index reserved by AllocateDO_PWM()
 * @param channel The Digital Output channel to output on
 */
void DigitalModule::SetDO_PWMOutputChannel(uint32_t pwmGenerator, uint32_t channel) // pwmGenerator should become void*
{
  	int32_t status = 0;
	void* generator = &pwmGenerator;
	setPWMOutputChannelWithModule(m_module, generator, channel, &status);
	wpi_setErrorWithContext(status, getHALErrorMessage(status));
}

/**
 * Configure the duty-cycle of the PWM generator
 * 
 * @param pwmGenerator The generator index reserved by AllocateDO_PWM()
 * @param dutyCycle The percent duty cycle to output [0..1].
 */
void DigitalModule::SetDO_PWMDutyCycle(uint32_t pwmGenerator, float dutyCycle)
{
  	int32_t status = 0;
	void* generator = &pwmGenerator;
	setPWMDutyCycleWithModule(m_module, generator, dutyCycle, &status);
	wpi_setErrorWithContext(status, getHALErrorMessage(status));
}

/**
 * Get the loop timing of the Digital Module
 * 
 * @return The loop time
 */
uint16_t DigitalModule::GetLoopTiming()
{
  	int32_t status = 0;
	uint16_t timing = getLoopTimingWithModule(m_module, &status);
	wpi_setErrorWithContext(status, getHALErrorMessage(status));
	return timing;
}


