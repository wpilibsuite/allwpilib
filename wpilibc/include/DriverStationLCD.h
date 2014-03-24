/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008. All Rights Reserved.							  */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in $(WIND_BASE)/WPILib.  */
/*----------------------------------------------------------------------------*/

#ifndef __DRIVER_STATION_LCD_H__
#define __DRIVER_STATION_LCD_H__

#include "SensorBase.h"
#include "HAL/Semaphore.h"
#include <stdarg.h>

/**
 * Provide access to "LCD" on the Driver Station.
 * This is the Messages box on the DS Operation tab.
 * 
 * Buffer the printed data locally and then send it
 * when UpdateLCD is called.
 */
class DriverStationLCD : public SensorBase
{
public:
	static const uint32_t kSyncTimeout_ms = 20;
	static const uint16_t kFullDisplayTextCommand = 0x9FFF;
	static const int32_t kLineLength = 21;
	static const int32_t kNumLines = 6;
	enum Line {kMain_Line6=0, kUser_Line1=0, kUser_Line2=1, kUser_Line3=2, kUser_Line4=3, kUser_Line5=4, kUser_Line6=5};

	virtual ~DriverStationLCD();
	static DriverStationLCD *GetInstance();

	void UpdateLCD();
	void Printf(Line line, int32_t startingColumn, const char *writeFmt, ...);
	void VPrintf(Line line, int32_t startingColumn, const char *writeFmt, va_list args);
	void PrintfLine(Line line, const char *writeFmt, ...);
	void VPrintfLine(Line line, const char *writeFmt, va_list args);
 
	void Clear();

protected:
	DriverStationLCD();

private:
	static void InitTask(DriverStationLCD *ds);
	static DriverStationLCD *m_instance;
	DISALLOW_COPY_AND_ASSIGN(DriverStationLCD);

	char *m_textBuffer;
	MUTEX_ID m_textBufferSemaphore;
};

#endif

