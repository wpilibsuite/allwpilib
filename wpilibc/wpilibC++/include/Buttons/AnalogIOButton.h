/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2011. All Rights Reserved.							  */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in $(WIND_BASE)/WPILib.  */
/*----------------------------------------------------------------------------*/

#ifndef __ANALOG_IO_BUTTON_H__
#define __ANALOG_IO_BUTTON_H__

#include "Buttons/Button.h"

class AnalogIOButton : public Trigger
{
public:
	static const double kThreshold;

	AnalogIOButton(int port);
	virtual ~AnalogIOButton() {}
	virtual bool Get();

private:
	int m_port;
};

#endif

