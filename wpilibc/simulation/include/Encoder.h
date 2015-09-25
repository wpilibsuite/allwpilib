/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008. All Rights Reserved.							  */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in $(WIND_BASE)/WPILib.  */
/*----------------------------------------------------------------------------*/
#pragma once

#include "simulation/SimEncoder.h"
#include "CounterBase.h"
#include "SensorBase.h"
#include "Counter.h"
#include "PIDSource.h"
#include "LiveWindow/LiveWindowSendable.h"

#include <memory>

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
	// TODO: [Not Supported] Encoder(DigitalSource *aSource, DigitalSource *bSource, bool reverseDirection=false, EncodingType encodingType = k4X);
	// TODO: [Not Supported] Encoder(DigitalSource &aSource, DigitalSource &bSource, bool reverseDirection=false, EncodingType encodingType = k4X);
	virtual ~Encoder() = default;

	// CounterBase interface
	int32_t Get() const override;
	int32_t GetRaw() const;
	int32_t GetEncodingScale() const;
	void Reset() override;
	double GetPeriod() const override;
	void SetMaxPeriod(double maxPeriod) override;
	bool GetStopped() const override;
	bool GetDirection() const override;

	double GetDistance() const;
	double GetRate() const;
	void SetMinRate(double minRate);
	void SetDistancePerPulse(double distancePerPulse);
	void SetReverseDirection(bool reverseDirection);
	void SetSamplesToAverage(int samplesToAverage);
	int GetSamplesToAverage() const;
	void SetPIDSourceType(PIDSourceType pidSource);
	double PIDGet() override;

	void UpdateTable() override;
	void StartLiveWindowMode() override;
	void StopLiveWindowMode() override;
	std::string GetSmartDashboardType() const override;
	void InitTable(std::shared_ptr<ITable> subTable) override;
	std::shared_ptr<ITable> GetTable() const override;

	int32_t FPGAEncoderIndex() const
	{
		return 0;
	}

private:
	void InitEncoder(int channelA, int channelB, bool _reverseDirection, EncodingType encodingType);
	double DecodingScaleFactor() const;

	// TODO: [Not Supported] DigitalSource *m_aSource;		// the A phase of the quad encoder
	// TODO: [Not Supported] DigitalSource *m_bSource;		// the B phase of the quad encoder
    // TODO: [Not Supported] bool m_allocatedASource;		// was the A source allocated locally?
    // TODO: [Not Supported] bool m_allocatedBSource;		// was the B source allocated locally?
	int channelA, channelB;
	double m_distancePerPulse;		// distance of travel for each encoder tick
	EncodingType m_encodingType;	// Encoding type
	int32_t m_encodingScale;		// 1x, 2x, or 4x, per the encodingType
	bool m_reverseDirection;
	SimEncoder* impl;

	std::shared_ptr<ITable> m_table;
};
