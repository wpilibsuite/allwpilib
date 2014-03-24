/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008. All Rights Reserved.							  */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in $(WIND_BASE)/WPILib.  */
/*----------------------------------------------------------------------------*/

#ifndef CPPCOUNTER_BASE_H_
#define CPPCOUNTER_BASE_H_

/**
 * Interface for counting the number of ticks on a digital input channel.
 * Encoders, Gear tooth sensors, and counters should all subclass this so it can be used to
 * build more advanced classes for control and driving.
 */
class CounterBase
{
public:
	typedef enum {k1X, k2X, k4X} EncodingType;

	virtual ~CounterBase() {}
	virtual void Start() = 0;
	virtual int32_t Get() = 0;
	virtual void Reset() = 0;
	virtual void Stop() = 0;
	virtual double GetPeriod() = 0;
	virtual void SetMaxPeriod(double maxPeriod) = 0;
	virtual bool GetStopped() = 0;
	virtual bool GetDirection() = 0;
};

#endif
