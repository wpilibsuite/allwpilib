/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008. All Rights Reserved.							  */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in $(WIND_BASE)/WPILib.  */
/*----------------------------------------------------------------------------*/

#include "GearTooth.h"
#include "LiveWindow/LiveWindow.h"

constexpr double GearTooth::kGearToothThreshold;

/**
 * Common code called by the constructors.
 */
void GearTooth::EnableDirectionSensing(bool directionSensitive)
{
	if (directionSensitive)
	{
		SetPulseLengthMode(kGearToothThreshold);
	}
}

/**
 * Construct a GearTooth sensor given a channel.
 * 
 * The default module is assumed.
 * 
 * @param channel The GPIO channel on the digital module that the sensor is connected to.
 * @param directionSensitive Enable the pulse length decoding in hardware to specify count direction.
 */
GearTooth::GearTooth(uint32_t channel, bool directionSensitive)
	: Counter(channel)
{
	EnableDirectionSensing(directionSensitive);
}

/**
 * Construct a GearTooth sensor given a channel and module.
 * 
 * @param moduleNumber The digital module (1 or 2).
 * @param channel The GPIO channel on the digital module that the sensor is connected to.
 * @param directionSensitive Enable the pulse length decoding in hardware to specify count direction.
 */
GearTooth::GearTooth(uint8_t moduleNumber, uint32_t channel, bool directionSensitive)
	: Counter(moduleNumber, channel)
{
	EnableDirectionSensing(directionSensitive);
	LiveWindow::GetInstance()->AddSensor("GearTooth", moduleNumber, channel, this);
}

/**
 * Construct a GearTooth sensor given a digital input.
 * This should be used when sharing digial inputs.
 * 
 * @param source An object that fully descibes the input that the sensor is connected to.
 * @param directionSensitive Enable the pulse length decoding in hardware to specify count direction.
 */
GearTooth::GearTooth(DigitalSource *source, bool directionSensitive)
	: Counter(source)
{
	EnableDirectionSensing(directionSensitive);
}

GearTooth::GearTooth(DigitalSource &source, bool directionSensitive): Counter(source)
{
	EnableDirectionSensing(directionSensitive);
}

/**
 * Free the resources associated with a gear tooth sensor.
 */
GearTooth::~GearTooth()
{
}


std::string GearTooth::GetSmartDashboardType() {
	return "GearTooth";
}

