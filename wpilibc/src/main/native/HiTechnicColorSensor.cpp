/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008. All Rights Reserved.							  */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in $(WIND_BASE)/WPILib.  */
/*----------------------------------------------------------------------------*/

#include "HiTechnicColorSensor.h"
#include "DigitalModule.h"
#include "I2C.h"
#include "NetworkCommunication/UsageReporting.h"
#include "networktables2/type/NumberArray.h"
#include "WPIErrors.h"

const uint8_t HiTechnicColorSensor::kAddress;
const uint8_t HiTechnicColorSensor::kManufacturerBaseRegister;
const uint8_t HiTechnicColorSensor::kManufacturerSize;
const uint8_t HiTechnicColorSensor::kSensorTypeBaseRegister;
const uint8_t HiTechnicColorSensor::kSensorTypeSize;
const uint8_t HiTechnicColorSensor::kModeRegister;
const uint8_t HiTechnicColorSensor::kColorRegister;
const uint8_t HiTechnicColorSensor::kRedRegister;
const uint8_t HiTechnicColorSensor::kGreenRegister;
const uint8_t HiTechnicColorSensor::kBlueRegister;
const uint8_t HiTechnicColorSensor::kRawRedRegister;
const uint8_t HiTechnicColorSensor::kRawGreenRegister;
const uint8_t HiTechnicColorSensor::kRawBlueRegister;

/**
 * Constructor.
 * 
 * @param moduleNumber The digital module that the sensor is plugged into (1 or 2).
 */
HiTechnicColorSensor::HiTechnicColorSensor(uint8_t moduleNumber)
	: m_i2c (NULL)
{
	m_table = NULL;
	DigitalModule *module = DigitalModule::GetInstance(moduleNumber);
	m_mode = kActive;
	
	if (module)
	{
		m_i2c = module->GetI2C(kAddress);
	
		// Verify Sensor
		const uint8_t kExpectedManufacturer[] = "HiTechnc";
		const uint8_t kExpectedSensorType[] = "ColorPD ";
		if ( ! m_i2c->VerifySensor(kManufacturerBaseRegister, kManufacturerSize, kExpectedManufacturer) )
		{
			wpi_setWPIError(CompassManufacturerError);
			return;
		}
		if ( ! m_i2c->VerifySensor(kSensorTypeBaseRegister, kSensorTypeSize, kExpectedSensorType) )
		{
			wpi_setWPIError(CompassTypeError);
		}
		
		nUsageReporting::report(nUsageReporting::kResourceType_HiTechnicColorSensor, moduleNumber - 1);
	}
}

/**
 * Destructor.
 */
HiTechnicColorSensor::~HiTechnicColorSensor()
{
	delete m_i2c;
	m_i2c = NULL;
}

/**
 * Get the estimated color.
 *
 * Gets a color estimate from the sensor corresponding to the
 * table found with the sensor or at the following site:
 * http://www.hitechnic.com/cgi-bin/commerce.cgi?preadd=action&key=NCO1038
 *
 * @return The estimated color.
 */
uint8_t HiTechnicColorSensor::GetColor()
{
	uint8_t color = 0;
	
	if(m_mode != kActive)
	{
		SetMode(kActive);
	}
	if (m_i2c)
	{
		m_i2c->Read(kColorRegister, sizeof(color), &color);
	}
	return color;
}

/**
 * Get the Red value.
 *
 * Gets the (0-255) red value from the sensor.
 * 
 * The sensor must be in active mode to access the regular RGB data
 * if the sensor is not in active mode, it will be placed into active
 * mode by this method.
 *
 * @return The Red sensor value.
 */
uint8_t HiTechnicColorSensor::GetRed()
{
	uint8_t red = 0;
	
	if(m_mode != kActive)
	{
		SetMode(kActive);
	}
	if (m_i2c)
	{
		m_i2c->Read(kRedRegister, sizeof(red), &red);
	}
	return red;
}

/**
 * Get the Green value.
 *
 * Gets the(0-255) green value from the sensor.
 * 
 * The sensor must be in active mode to access the regular RGB data
 * if the sensor is not in active mode, it will be placed into active
 * mode by this method.
 * 
 * @return The Green sensor value.
 */
uint8_t HiTechnicColorSensor::GetGreen()
{
	uint8_t green = 0;
	
	if(m_mode != kActive)
	{
		SetMode(kActive);
	}
	if (m_i2c)
	{
		m_i2c->Read(kGreenRegister, sizeof(green), &green);
	}
	return green;
}

/**
 * Get the Blue value.
 *
 * Gets the raw (0-255) blue value from the sensor.
 * 
 * The sensor must be in active mode to access the regular RGB data
 * if the sensor is not in active mode, it will be placed into active
 * mode by this method.
 * 
 * @return The Blue sensor value.
 */
uint8_t HiTechnicColorSensor::GetBlue()
{
	uint8_t blue = 0;
	
	if(m_mode != kActive)
	{
		SetMode(kActive);
	}
	if (m_i2c)
	{
		m_i2c->Read(kBlueRegister, sizeof(blue), &blue);
	}
	return blue;
}

/**
 * Get the value of all three colors from a single sensor reading.
 * Using this method ensures that all three values come from the
 * same sensor reading, using the individual color methods provides
 * no such guarantee.
 * 
 * The sensor must be in active mode to access the regular RGB data.
 * If the sensor is not in active mode, it will be placed into active
 * mode by this method.
 * 
 * @return RGB object with the three color values
 */
HiTechnicColorSensor::RGB HiTechnicColorSensor::GetRGB()
{
	uint8_t colors[3] = {0,0,0};
	RGB result;
	
	if(m_mode != kActive)
	{
		SetMode(kActive);
	}
	if(m_i2c)
	{
		m_i2c->Read(kRawRedRegister, sizeof(colors), (uint8_t*)&colors);
	}
	
	result.red = colors[0];
	result.green = colors[1];
	result.blue = colors[2];
	
	return result;
}

/**
 * Get the Raw Red value.
 *
 * Gets the (0-65536) raw red value from the sensor.
 * 
 * The sensor must be in raw or passive mode to access the regular RGB data
 * if the sensor is not in raw or passive mode, it will be placed into raw
 * mode by this method.
 *
 * @return The Raw Red sensor value.
 */
uint16_t HiTechnicColorSensor::GetRawRed()
{
	uint16_t rawRed = 0;
	
	if(m_mode == kActive)
	{
		SetMode(kRaw);
	}
	if (m_i2c)
	{
		m_i2c->Read(kRawRedRegister, sizeof(rawRed), (uint8_t *)&rawRed);
	}
	return rawRed;
}

/**
   * Get the Raw Green value.
   *
   * Gets the (0-65536) raw green value from the sensor.
   * 
   * The sensor must be in raw or passive mode to access the regular RGB data
   * if the sensor is not in raw or passive mode, it will be placed into raw
   * mode by this method.
   *
   * @return The Raw Green sensor value.
   */
uint16_t HiTechnicColorSensor::GetRawGreen()
{
	uint16_t rawGreen = 0;
	
	if(m_mode == kActive)
	{
		SetMode(kRaw);
	}
	if (m_i2c)
	{
		m_i2c->Read(kRawGreenRegister, sizeof(rawGreen), (uint8_t *)&rawGreen);
	}
	return rawGreen;
}

/**
 * Get the Raw Blue value.
 *
 * Gets the (0-65536) raw blue value from the sensor.
 * 
 * The sensor must be in raw or passive mode to access the regular RGB data
 * if the sensor is not in raw or passive mode, it will be placed into raw
 * mode by this method.
 *
 * @return The Raw Blue sensor value.
 */
uint16_t HiTechnicColorSensor::GetRawBlue()
{
	uint16_t rawBlue = 0;
	
	if(m_mode == kActive)
	{
		SetMode(kRaw);
	}
	if (m_i2c)
	{
		m_i2c->Read(kRawBlueRegister, sizeof(rawBlue), (uint8_t *)&rawBlue);
	}
	return rawBlue;
}

/**
 * Get the raw value of all three colors from a single sensor reading.
 * Using this method ensures that all three values come from the
 * same sensor reading, using the individual color methods provides
 * no such guarantee.
 *
 * Gets the (0-65536) raw color values from the sensor.
 * 
 * The sensor must be in raw or passive mode to access the regular RGB data
 * if the sensor is not in raw or passive mode, it will be placed into raw
 * mode by this method.
 *
 * @return An RGB object with the raw sensor values.
 */
HiTechnicColorSensor::RGB HiTechnicColorSensor::GetRawRGB()
{
	uint8_t colors[6] = {0,0,0,0,0,0};
	RGB result;
	
	if(m_mode != kActive)
	{
		SetMode(kActive);
	}
	if(m_i2c)
	{
		m_i2c->Read(kRedRegister, sizeof(colors), (uint8_t*)&colors);
	}
	
	result.red = (colors[0]<<8) + colors[1];
	result.green = (colors[2]<<8) + colors[3];
	result.blue = (colors[4]<<8) + colors[5];
	
	return result;
}

/**
 * Set the Mode of the color sensor
 * This method is used to set the color sensor to one of the three modes,
 * active, passive or raw. The sensor defaults to active mode which uses the
 * internal LED and returns an interpreted color value and 3 8-bit RGB channel
 * values. Raw mode uses the internal LED and returns 3 16-bit RGB channel values.
 * Passive mode disables the internal LED and returns 3 16-bit RGB channel values.
 * @param mode The mode to set
 */
void HiTechnicColorSensor::SetMode(tColorMode mode)
{
	if(m_i2c)
	{
		m_i2c->Write(kModeRegister, (uint8_t)mode);
	}
}

/*
 * Live Window code, only does anything if live window is activated.
 */
std::string HiTechnicColorSensor::GetType()
{
    return "Compass";
}

/**
 * {@inheritDoc}
 */
void HiTechnicColorSensor::InitTable(ITable *subtable) {
    m_table = subtable;
    UpdateTable();
}

/**
 * {@inheritDoc}
 */
void HiTechnicColorSensor::UpdateTable() {
    if (m_table != NULL) {
        m_table->PutNumber("Value", GetColor());
		NumberArray* rgb = new NumberArray();
		rgb->add(GetRed());
		rgb->add(GetGreen());
		rgb->add(GetBlue());
		m_table->PutValue("RGB", *rgb);
		delete rgb;
    }
}

/**
 * {@inheritDoc}
 */
ITable* HiTechnicColorSensor::GetTable()
{
    return m_table;
}

/**
 * {@inheritDoc}
 */
void HiTechnicColorSensor::StartLiveWindowMode()
{
	
}

/**
 * {@inheritDoc}
 */
void HiTechnicColorSensor::StopLiveWindowMode()
{
	
}
