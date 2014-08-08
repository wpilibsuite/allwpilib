/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in $(WIND_BASE)/WPILib.  */
/*----------------------------------------------------------------------------*/
#pragma once

#include <stdint.h>
#include <cmath>
#include <pthread.h>
#include <semaphore.h>

/**
 * Interface for Controllers
 * Common interface for controllers. Controllers run control loops, the most common
 * are PID controllers and their variants, but this includes anything that is controlling
 * an actuator in a separate thread.
 */
class Controller
{
public:
	virtual ~Controller() {};
	
	/**
	 * Allows the control loop to run
	 */
	virtual void Enable() = 0;
	
	/** 
	 * Stops the control loop from running until explicitly re-enabled by calling enable()
	 */
	virtual void Disable() = 0;
};

