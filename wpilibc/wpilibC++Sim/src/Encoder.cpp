/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008. All Rights Reserved.							  */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in $(WIND_BASE)/WPILib.  */
/*----------------------------------------------------------------------------*/

#include "Encoder.h"
#include "Resource.h"
#include "WPIErrors.h"
#include "LiveWindow/LiveWindow.h"

/**
 * Common initialization code for Encoders.
 * This code allocates resources for Encoders and is common to all constructors.
 *
 * The counter will start counting immediately.
 *
 * @param reverseDirection If true, counts down instead of up (this is all relative)
 * @param encodingType either k1X, k2X, or k4X to indicate 1X, 2X or 4X decoding. If 4X is
 * selected, then an encoder FPGA object is used and the returned counts will be 4x the encoder
 * spec'd value since all rising and falling edges are counted. If 1X or 2X are selected then
 * a counter object will be used and the returned value will either exactly match the spec'd count
 * or be double (2x) the spec'd count.
 */
void Encoder::InitEncoder(int channelA, int channelB, bool reverseDirection, EncodingType encodingType)
{
	m_table = nullptr;
	this->channelA = channelA;
	this->channelB = channelB;
	m_encodingType = encodingType;
	m_encodingScale = encodingType == k4X ? 4
		: encodingType == k2X ? 2
		: 1;

	int32_t index = 0;
	m_distancePerPulse = 1.0;

	LiveWindow::GetInstance()->AddSensor("Encoder", channelA, this);

	if (channelB < channelA) { // Swap ports
		int channel = channelB;
		channelB = channelA;
		channelA = channel;
		m_reverseDirection = !reverseDirection;
	} else {
		m_reverseDirection = reverseDirection;
	}
	char buffer[50];
	int n = sprintf(buffer, "dio/%d/%d", channelA, channelB);
	impl = new SimEncoder(buffer);
	impl->Start();
}

/**
 * Encoder constructor.
 * Construct a Encoder given a and b channels.
 *
 * The counter will start counting immediately.
 *
 * @param aChannel The a channel digital input channel.
 * @param bChannel The b channel digital input channel.
 * @param reverseDirection represents the orientation of the encoder and inverts the output values
 * if necessary so forward represents positive values.
 * @param encodingType either k1X, k2X, or k4X to indicate 1X, 2X or 4X decoding. If 4X is
 * selected, then an encoder FPGA object is used and the returned counts will be 4x the encoder
 * spec'd value since all rising and falling edges are counted. If 1X or 2X are selected then
 * a counter object will be used and the returned value will either exactly match the spec'd count
 * or be double (2x) the spec'd count.
 */
Encoder::Encoder(uint32_t aChannel, uint32_t bChannel, bool reverseDirection, EncodingType encodingType)
{
    InitEncoder(aChannel, bChannel, reverseDirection, encodingType);
}

/**
 * Encoder constructor.
 * Construct a Encoder given a and b channels as digital inputs. This is used in the case
 * where the digital inputs are shared. The Encoder class will not allocate the digital inputs
 * and assume that they already are counted.
 *
 * The counter will start counting immediately.
 *
 * @param aSource The source that should be used for the a channel.
 * @param bSource the source that should be used for the b channel.
 * @param reverseDirection represents the orientation of the encoder and inverts the output values
 * if necessary so forward represents positive values.
 * @param encodingType either k1X, k2X, or k4X to indicate 1X, 2X or 4X decoding. If 4X is
 * selected, then an encoder FPGA object is used and the returned counts will be 4x the encoder
 * spec'd value since all rising and falling edges are counted. If 1X or 2X are selected then
 * a counter object will be used and the returned value will either exactly match the spec'd count
 * or be double (2x) the spec'd count.
 */
/* TODO: [Not Supported] Encoder::Encoder(DigitalSource *aSource, DigitalSource *bSource, bool reverseDirection, EncodingType encodingType) :
	m_encoder(nullptr),
	m_counter(nullptr)
{
	m_aSource = aSource;
	m_bSource = bSource;
	m_allocatedASource = false;
	m_allocatedBSource = false;
	if (m_aSource == nullptr || m_bSource == nullptr)
		wpi_setWPIError(NullParameter);
	else
		InitEncoder(reverseDirection, encodingType);
        }*/

/**
 * Encoder constructor.
 * Construct a Encoder given a and b channels as digital inputs. This is used in the case
 * where the digital inputs are shared. The Encoder class will not allocate the digital inputs
 * and assume that they already are counted.
 *
 * The counter will start counting immediately.
 *
 * @param aSource The source that should be used for the a channel.
 * @param bSource the source that should be used for the b channel.
 * @param reverseDirection represents the orientation of the encoder and inverts the output values
 * if necessary so forward represents positive values.
 * @param encodingType either k1X, k2X, or k4X to indicate 1X, 2X or 4X decoding. If 4X is
 * selected, then an encoder FPGA object is used and the returned counts will be 4x the encoder
 * spec'd value since all rising and falling edges are counted. If 1X or 2X are selected then
 * a counter object will be used and the returned value will either exactly match the spec'd count
 * or be double (2x) the spec'd count.
 */
/*// TODO: [Not Supported] Encoder::Encoder(DigitalSource &aSource, DigitalSource &bSource, bool reverseDirection, EncodingType encodingType) :
	m_encoder(nullptr),
	m_counter(nullptr)
{
	m_aSource = &aSource;
	m_bSource = &bSource;
	m_allocatedASource = false;
	m_allocatedBSource = false;
	InitEncoder(reverseDirection, encodingType);
    }*/

/**
 * Reset the Encoder distance to zero.
 * Resets the current count to zero on the encoder.
 */
void Encoder::Reset()
{
    impl->Reset();
}

/**
 * Determine if the encoder is stopped.
 * Using the MaxPeriod value, a boolean is returned that is true if the encoder is considered
 * stopped and false if it is still moving. A stopped encoder is one where the most recent pulse
 * width exceeds the MaxPeriod.
 * @return True if the encoder is considered stopped.
 */
bool Encoder::GetStopped() const
{
    throw "Simulation doesn't currently support this method.";
}

/**
 * The last direction the encoder value changed.
 * @return The last direction the encoder value changed.
 */
bool Encoder::GetDirection() const
{
    throw "Simulation doesn't currently support this method.";
}

/**
 * The scale needed to convert a raw counter value into a number of encoder pulses.
 */
double Encoder::DecodingScaleFactor() const
{
	switch (m_encodingType)
	{
	case k1X:
		return 1.0;
	case k2X:
		return 0.5;
	case k4X:
		return 0.25;
	default:
		return 0.0;
	}
}

/**
 * The encoding scale factor 1x, 2x, or 4x, per the requested encodingType.
 * Used to divide raw edge counts down to spec'd counts.
 */
int32_t Encoder::GetEncodingScale() const { return m_encodingScale; }

/**
 * Gets the raw value from the encoder.
 * The raw value is the actual count unscaled by the 1x, 2x, or 4x scale
 * factor.
 * @return Current raw count from the encoder
 */
int32_t Encoder::GetRaw() const
{
	throw "Simulation doesn't currently support this method.";
}

/**
 * Gets the current count.
 * Returns the current count on the Encoder.
 * This method compensates for the decoding type.
 *
 * @return Current count from the Encoder adjusted for the 1x, 2x, or 4x scale factor.
 */
int32_t Encoder::Get() const
{
	throw "Simulation doesn't currently support this method.";
}

/**
 * Returns the period of the most recent pulse.
 * Returns the period of the most recent Encoder pulse in seconds.
 * This method compenstates for the decoding type.
 *
 * @deprecated Use GetRate() in favor of this method.  This returns unscaled periods and GetRate() scales using value from SetDistancePerPulse().
 *
 * @return Period in seconds of the most recent pulse.
 */
double Encoder::GetPeriod() const
{
	throw "Simulation doesn't currently support this method.";
}

/**
 * Sets the maximum period for stopped detection.
 * Sets the value that represents the maximum period of the Encoder before it will assume
 * that the attached device is stopped. This timeout allows users to determine if the wheels or
 * other shaft has stopped rotating.
 * This method compensates for the decoding type.
 *
 * @deprecated Use SetMinRate() in favor of this method.  This takes unscaled periods and SetMinRate() scales using value from SetDistancePerPulse().
 *
 * @param maxPeriod The maximum time between rising and falling edges before the FPGA will
 * report the device stopped. This is expressed in seconds.
 */
void Encoder::SetMaxPeriod(double maxPeriod)
{
	throw "Simulation doesn't currently support this method.";
}

/**
 * Get the distance the robot has driven since the last reset.
 *
 * @return The distance driven since the last reset as scaled by the value from SetDistancePerPulse().
 */
double Encoder::GetDistance() const
{
    return m_distancePerPulse * impl->GetPosition();
}

/**
 * Get the current rate of the encoder.
 * Units are distance per second as scaled by the value from SetDistancePerPulse().
 *
 * @return The current rate of the encoder.
 */
double Encoder::GetRate() const
{
    return m_distancePerPulse * impl->GetVelocity();
}

/**
 * Set the minimum rate of the device before the hardware reports it stopped.
 *
 * @param minRate The minimum rate.  The units are in distance per second as scaled by the value from SetDistancePerPulse().
 */
void Encoder::SetMinRate(double minRate)
{
    throw "Simulation doesn't currently support this method.";
}

/**
 * Set the distance per pulse for this encoder.
 * This sets the multiplier used to determine the distance driven based on the count value
 * from the encoder.
 * Do not include the decoding type in this scale.  The library already compensates for the decoding type.
 * Set this value based on the encoder's rated Pulses per Revolution and
 * factor in gearing reductions following the encoder shaft.
 * This distance can be in any units you like, linear or angular.
 *
 * @param distancePerPulse The scale factor that will be used to convert pulses to useful units.
 */
void Encoder::SetDistancePerPulse(double distancePerPulse)
{
	if (m_reverseDirection) {
		m_distancePerPulse = -distancePerPulse;
	} else {
		m_distancePerPulse = distancePerPulse;
	}
}

/**
 * Set the direction sensing for this encoder.
 * This sets the direction sensing on the encoder so that it could count in the correct
 * software direction regardless of the mounting.
 * @param reverseDirection true if the encoder direction should be reversed
 */
void Encoder::SetReverseDirection(bool reverseDirection)
{
    throw "Simulation doesn't currently support this method.";
}

/**
 * Set which parameter of the encoder you are using as a process control variable.
 *
 * @param pidSource An enum to select the parameter.
 */
void Encoder::SetPIDSourceType(PIDSourceType pidSource)
{
	m_pidSource = pidSource;
}

/**
 * Implement the PIDSource interface.
 *
 * @return The current value of the selected source parameter.
 */
double Encoder::PIDGet()
{
	switch (m_pidSource)
	{
	case PIDSourceType::kDisplacement:
		return GetDistance();
	case PIDSourceType::kRate:
		return GetRate();
	default:
		return 0.0;
	}
}

void Encoder::UpdateTable() {
	if (m_table != nullptr) {
        m_table->PutNumber("Speed", GetRate());
        m_table->PutNumber("Distance", GetDistance());
        m_table->PutNumber("Distance per Tick", m_reverseDirection ? -m_distancePerPulse : m_distancePerPulse);
	}
}

void Encoder::StartLiveWindowMode() {

}

void Encoder::StopLiveWindowMode() {

}

std::string Encoder::GetSmartDashboardType() const {
	if (m_encodingType == k4X)
		return "Quadrature Encoder";
	else
		return "Encoder";
}

void Encoder::InitTable(std::shared_ptr<ITable> subTable) {
	m_table = subTable;
	UpdateTable();
}

std::shared_ptr<ITable> Encoder::GetTable() const {
	return m_table;
}
