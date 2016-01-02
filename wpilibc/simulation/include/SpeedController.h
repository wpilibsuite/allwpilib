/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008-2016. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include "PIDOutput.h"

/**
 * Interface for speed controlling devices.
 */
class SpeedController : public PIDOutput
{
public:
	virtual ~SpeedController() = default;
	/**
	 * Common interface for setting the speed of a speed controller.
	 * 
	 * @param speed The speed to set.  Value should be between -1.0 and 1.0.
	 * @param syncGroup The update group to add this Set() to, pending UpdateSyncGroup().  If 0, update immediately.
	 */
	virtual void Set(float speed, uint8_t syncGroup = 0) = 0;
	/**
	 * Common interface for getting the current set speed of a speed controller.
	 * 
	 * @return The current set speed.  Value is between -1.0 and 1.0.
	 */
	virtual float Get() const = 0;
	/**
	 * Common interface for disabling a motor.
	 */
	virtual void Disable() = 0;
};
