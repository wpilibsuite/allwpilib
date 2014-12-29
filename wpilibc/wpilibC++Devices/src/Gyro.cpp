/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008. All Rights Reserved.							  */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in $(WIND_BASE)/WPILib.  */
/*----------------------------------------------------------------------------*/

#include "Gyro.h"
#include "AnalogInput.h"
//#include "NetworkCommunication/UsageReporting.h"
#include "Timer.h"
#include "WPIErrors.h"
#include "LiveWindow/LiveWindow.h"
#include <climits>
const uint32_t Gyro::kOversampleBits;
const uint32_t Gyro::kAverageBits;
constexpr float Gyro::kSamplesPerSecond;
constexpr float Gyro::kCalibrationSampleTime;
constexpr float Gyro::kDefaultVoltsPerDegreePerSecond;

/**
 * Initialize the gyro.
 * Calibrate the gyro by running for a number of samples and computing the center value. 
 * Then use the center value as the Accumulator center value for subsequent measurements.
 * It's important to make sure that the robot is not moving while the centering calculations are
 * in progress, this is typically done when the robot is first turned on while it's sitting at
 * rest before the competition starts.
 */
void Gyro::InitGyro()
{
	m_table = NULL;
	if (!m_analog->IsAccumulatorChannel())
	{
		wpi_setWPIErrorWithContext(ParameterOutOfRange,
            " channel (must be accumulator channel)");
		if (m_channelAllocated)
		{
			delete m_analog;
			m_analog = NULL;
		}
		return;
	}

	m_voltsPerDegreePerSecond = kDefaultVoltsPerDegreePerSecond;
	m_analog->SetAverageBits(kAverageBits);
	m_analog->SetOversampleBits(kOversampleBits);
	float sampleRate = kSamplesPerSecond *
		(1 << (kAverageBits + kOversampleBits));
	m_analog->SetSampleRate(sampleRate);
	Wait(1.0);

	m_analog->InitAccumulator();

	Wait(kCalibrationSampleTime);

	int64_t value;
	uint32_t count;
	m_analog->GetAccumulatorOutput(&value, &count);

	m_center = (uint32_t)((float)value / (float)count + .5);

	m_offset = ((float)value / (float)count) - (float)m_center;
	m_analog->SetAccumulatorCenter(m_center);
	m_analog->ResetAccumulator();

	SetDeadband(0.0f);

	SetPIDSourceParameter(kAngle);

	HALReport(HALUsageReporting::kResourceType_Gyro, m_analog->GetChannel());
	LiveWindow::GetInstance()->AddSensor("Gyro", m_analog->GetChannel(), this);
}

/**
 * Gyro constructor using the Analog Input channel number.
 *
 * @param channel The analog channel the gyro is connected to. Gyros 
	              can only be used on on-board Analog Inputs 0-1.
 */
Gyro::Gyro(int32_t channel)
{
	m_analog = new AnalogInput(channel);
	m_channelAllocated = true;
	InitGyro();
}

/**
 * Gyro constructor with a precreated AnalogInput object.
 * Use this constructor when the analog channel needs to be shared.
 * This object will not clean up the AnalogInput object when using this constructor. 
 * Gyros can only be used on on-board channels 0-1.
 * @param channel A pointer to the AnalogInput object that the gyro is connected to.
 */
Gyro::Gyro(AnalogInput *channel)
{
	m_analog = channel;
	m_channelAllocated = false;
	if (channel == NULL)
	{
		wpi_setWPIError(NullParameter);
	}
	else
	{
		InitGyro();
	}
}

/**
 * Gyro constructor with a precreated AnalogInput object.
 * Use this constructor when the analog channel needs to be shared.
 * This object will not clean up the AnalogInput object when using this constructor
 * @param channel A reference to the AnalogInput object that the gyro is connected to.
 */
Gyro::Gyro(AnalogInput &channel)
{
	m_analog = &channel;
	m_channelAllocated = false;
	InitGyro();
}

/**
 * Reset the gyro.
 * Resets the gyro to a heading of zero. This can be used if there is significant
 * drift in the gyro and it needs to be recalibrated after it has been running.
 */
void Gyro::Reset()
{
	m_analog->ResetAccumulator();
}

/**
 * Delete (free) the accumulator and the analog components used for the gyro.
 */
Gyro::~Gyro()
{
	if (m_channelAllocated)
		delete m_analog;
}

/**
 * Return the actual angle in degrees that the robot is currently facing.
 *
 * The angle is based on the current accumulator value corrected by the oversampling rate, the
 * gyro type and the A/D calibration values.
 * The angle is continuous, that is it will continue from 360->361 degrees. This allows algorithms that wouldn't
 * want to see a discontinuity in the gyro output as it sweeps from 360 to 0 on the second time around.
 *
 * @return the current heading of the robot in degrees. This heading is based on integration
 * of the returned rate from the gyro.
 */
float Gyro::GetAngle( void )
{
	int64_t rawValue;
	uint32_t count;
	m_analog->GetAccumulatorOutput(&rawValue, &count);

	int64_t value = rawValue - (int64_t)((float)count * m_offset);

	double scaledValue = value * 1e-9 * (double)m_analog->GetLSBWeight() * (double)(1 << m_analog->GetAverageBits()) /
		(m_analog->GetSampleRate() * m_voltsPerDegreePerSecond);

	return (float)scaledValue;
}


/**
 * Return the rate of rotation of the gyro
 *
 * The rate is based on the most recent reading of the gyro analog value
 *
 * @return the current rate in degrees per second
 */
double Gyro::GetRate( void )
{
	return (m_analog->GetAverageValue() - ((double)m_center + m_offset)) * 1e-9 * m_analog->GetLSBWeight()
			/ ((1 << m_analog->GetOversampleBits()) * m_voltsPerDegreePerSecond);
}


/**
 * Set the gyro sensitivity.
 * This takes the number of volts/degree/second sensitivity of the gyro and uses it in subsequent
 * calculations to allow the code to work with multiple gyros. This value is typically found in 
 * the gyro datasheet.
 *
 * @param voltsPerDegreePerSecond The sensitivity in Volts/degree/second
 */
void Gyro::SetSensitivity( float voltsPerDegreePerSecond )
{
	m_voltsPerDegreePerSecond = voltsPerDegreePerSecond;
}

/**
 * Set the size of the neutral zone.  Any voltage from the gyro less than this
 * amount from the center is considered stationary.  Setting a deadband will
 * decrease the amount of drift when the gyro isn't rotating, but will make it
 * less accurate.
 *
 * @param volts The size of the deadband in volts
 */
void Gyro::SetDeadband( float volts ) {
	int32_t deadband = volts * 1e9 / m_analog->GetLSBWeight() * (1 << m_analog->GetOversampleBits());
	m_analog->SetAccumulatorDeadband(deadband);
}


/**
 * Sets the type of output to use with the WPILib PID class
 * The gyro supports using either rate or angle for PID calculations
 */
void Gyro::SetPIDSourceParameter(PIDSourceParameter pidSource)
{
	if(pidSource == 0 || pidSource > 2)
		wpi_setWPIErrorWithContext(ParameterOutOfRange, "Gyro pidSource");
    m_pidSource = pidSource;
}

/**
 * Get the PIDOutput for the PIDSource base object. Can be set to return
 * angle or rate using SetPIDSourceParameter(). Defaults to angle.
 *
 * @return The PIDOutput (angle or rate, defaults to angle)
 */
double Gyro::PIDGet()
{
	switch(m_pidSource){
	case kRate:
		return GetRate();
	case kAngle:
		return GetAngle();
	default:
		return 0;
	}
}

void Gyro::UpdateTable() {
	if (m_table != NULL) {
		m_table->PutNumber("Value", GetAngle());
	}
}

void Gyro::StartLiveWindowMode() {

}

void Gyro::StopLiveWindowMode() {

}

std::string Gyro::GetSmartDashboardType() {
	return "Gyro";
}

void Gyro::InitTable(ITable *subTable) {
	m_table = subTable;
	UpdateTable();
}

ITable * Gyro::GetTable() {
	return m_table;
}
