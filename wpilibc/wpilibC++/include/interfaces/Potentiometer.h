/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008. All Rights Reserved.							  */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in $(WIND_BASE)/WPILib.  */
/*----------------------------------------------------------------------------*/

#ifndef INTERFACES_POTENTIOMETER_H
#define INTERFACES_POTENTIOMETER_H

#include "PIDSource.h"

/**
 * Interface for potentiometers.
 */
class Potentiometer : public PIDSource
{
public:
	virtual ~Potentiometer() {};
	/**
	 * Common interface for getting the current value of a potentiometer.
	 * 
	 * @return The current set speed.  Value is between -1.0 and 1.0.
	 */
	virtual double Get() = 0;
};

#endif
