/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008. All Rights Reserved.							  */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in $(WIND_BASE)/WPILib.  */
/*----------------------------------------------------------------------------*/
#pragma once

#include "HAL/HAL.hpp"
#include "CounterBase.h"
#include "SensorBase.h"
#include "Counter.h"
#include "PIDSource.h"
#include "LiveWindow/LiveWindowSendable.h"

class DigitalSource;

/**
 * Class to read quad encoders.
 * Quadrature encoders are devices that count shaft rotation and can sense direction. The output of
 * the QuadEncoder class is an integer that can count either up or down, and can go negative for
 * reverse direction counting. When creating QuadEncoders, a direction is supplied that changes the
 * sense of the output to make code more readable if the encoder is mounted such that forward movement
 * generates negative values. Quadrature encoders have two digital outputs, an A Channel and a B Channel
 * that are out of phase with each other to allow the FPGA to do direction sensing.
 *
 * All encoders will immediately start counting - Reset() them if you need them
 * to be zeroed before use.
 */
class Encoder : public SensorBase, public CounterBase, public PIDSource, public LiveWindowSendable
{
public:

	Encoder(uint32_t aChannel, uint32_t bChannel, bool reverseDirection = false,
			EncodingType encodingType = k4X);
	Encoder(DigitalSource *aSource, DigitalSource *bSource, bool reverseDirection = false,
			EncodingType encodingType = k4X);
	Encoder(DigitalSource &aSource, DigitalSource &bSource, bool reverseDirection = false,
			EncodingType encodingType = k4X);
	virtual ~Encoder();

	// CounterBase interface
	int32_t Get();
	int32_t GetRaw();
	int32_t GetEncodingScale();
	void Reset();
	double GetPeriod();
	void SetMaxPeriod(double maxPeriod);
	bool GetStopped();
	bool GetDirection();
	double GetDistance();
	double GetRate();
	void SetMinRate(double minRate);
	void SetDistancePerPulse(double distancePerPulse);
	void SetReverseDirection(bool reverseDirection);
	void SetSamplesToAverage(int samplesToAverage);
	int GetSamplesToAverage();
	void SetPIDSourceParameter(PIDSourceParameter pidSource);
	double PIDGet();

	void UpdateTable();
	void StartLiveWindowMode();
	void StopLiveWindowMode();
	std::string GetSmartDashboardType();
	void InitTable(ITable *subTable);
	ITable * GetTable();

	int32_t GetFPGAIndex()
	{
		return m_index;
	}

private:
	void InitEncoder(bool _reverseDirection, EncodingType encodingType);
	double DecodingScaleFactor();

	DigitalSource *m_aSource;		// the A phase of the quad encoder
	DigitalSource *m_bSource;		// the B phase of the quad encoder
	bool m_allocatedASource;		// was the A source allocated locally?
	bool m_allocatedBSource;		// was the B source allocated locally?
	void* m_encoder;
	int32_t m_index;				// The encoder's FPGA index.
	double m_distancePerPulse;		// distance of travel for each encoder tick
	Counter *m_counter;				// Counter object for 1x and 2x encoding
	EncodingType m_encodingType;	// Encoding type
	int32_t m_encodingScale;		// 1x, 2x, or 4x, per the encodingType
	PIDSourceParameter m_pidSource;	// Encoder parameter that sources a PID controller

	ITable *m_table;
};
