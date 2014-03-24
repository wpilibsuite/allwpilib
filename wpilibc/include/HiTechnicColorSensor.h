/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008. All Rights Reserved.							  */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in $(WIND_BASE)/WPILib.  */
/*----------------------------------------------------------------------------*/

#ifndef __HiTechnicColorSensor_h__
#define __HiTechnicColorSensor_h__

#include "SensorBase.h"
#include "LiveWindow/LiveWindowSendable.h"

class I2C;

/**
 * HiTechnic NXT Color Sensor.
 *
 * This class allows access to a HiTechnic NXT Color Sensor on an I2C bus.
 * These sensors do not allow changing addresses so you cannot have more
 * than one on a single bus.
 *
 * Details on the sensor can be found here:
 *   http://www.hitechnic.com/index.html?lang=en-us&target=d17.html
 *
 */
class HiTechnicColorSensor : public SensorBase
{
public:
	enum tColorMode {kActive = 0, kPassive = 1, kRaw = 3};
	typedef struct{
		uint16_t red;
		uint16_t blue;
		uint16_t green;
	}RGB;
	explicit HiTechnicColorSensor(uint8_t moduleNumber);
	virtual ~HiTechnicColorSensor();
	uint8_t GetColor();
	uint8_t GetRed();
	uint8_t GetGreen();
	uint8_t GetBlue();
	RGB GetRGB();
	uint16_t GetRawRed();
	uint16_t GetRawGreen();
	uint16_t GetRawBlue();
	RGB GetRawRGB();
	void SetMode(tColorMode mode);
	
	
	//LiveWindowSendable interface
	virtual std::string GetType();
	virtual void InitTable(ITable *subtable);
	virtual void UpdateTable();
	virtual ITable* GetTable();
	virtual void StartLiveWindowMode();
	virtual void StopLiveWindowMode(); 

private:
	static const uint8_t kAddress = 0x02;
	static const uint8_t kManufacturerBaseRegister = 0x08;
	static const uint8_t kManufacturerSize = 0x08;
	static const uint8_t kSensorTypeBaseRegister = 0x10;
	static const uint8_t kSensorTypeSize = 0x08;
	static const uint8_t kModeRegister = 0x41;
	static const uint8_t kColorRegister = 0x42;
	static const uint8_t kRedRegister = 0x43;
	static const uint8_t kGreenRegister = 0x44;
	static const uint8_t kBlueRegister = 0x45;
	static const uint8_t kRawRedRegister = 0x43;
	static const uint8_t kRawGreenRegister = 0x45;
	static const uint8_t kRawBlueRegister = 0x47;
	
	int m_mode;
	I2C* m_i2c;
	ITable *m_table;
};

#endif

