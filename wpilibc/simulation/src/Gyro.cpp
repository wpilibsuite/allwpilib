/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008. All Rights Reserved.							  */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in $(WIND_BASE)/WPILib.  */
/*----------------------------------------------------------------------------*/

#include "Gyro.h"
#include "Timer.h"
#include "WPIErrors.h"
#include "LiveWindow/LiveWindow.h"

const uint32_t Gyro::kOversampleBits = 10;
const uint32_t Gyro::kAverageBits = 0;
const float Gyro::kSamplesPerSecond = 50.0;
const float Gyro::kCalibrationSampleTime = 5.0;
const float Gyro::kDefaultVoltsPerDegreePerSecond = 0.007;

/**
 * Initialize the gyro.
 * Calibrate the gyro by running for a number of samples and computing the center value for this
 * part. Then use the center value as the Accumulator center value for subsequent measurements.
 * It's important to make sure that the robot is not moving while the centering calculations are
 * in progress, this is typically done when the robot is first turned on while it's sitting at
 * rest before the competition starts.
 */
void Gyro::InitGyro(int channel)
{
	SetPIDSourceType(PIDSourceType::kDisplacement);

	char buffer[50];
	int n = sprintf(buffer, "analog/%d", channel);
	impl = new SimGyro(buffer);

	LiveWindow::GetInstance()->AddSensor("Gyro", channel, this);
}

/**
 * Gyro constructor with only a channel..
 *
 * @param channel The analog channel the gyro is connected to.
 */
Gyro::Gyro(uint32_t channel)
{
    InitGyro(channel);
}

/**
 * Reset the gyro.
 * Resets the gyro to a heading of zero. This can be used if there is significant
 * drift in the gyro and it needs to be recalibrated after it has been running.
 */
void Gyro::Reset()
{
    impl->Reset();
}

/**
 * Return the actual angle in degrees that the robot is currently facing.
 *
 * The angle is based on the current accumulator value corrected by the oversampling rate, the
 * gyro type and the A/D calibration values.
 * The angle is continuous, that is can go beyond 360 degrees. This make algorithms that wouldn't
 * want to see a discontinuity in the gyro output as it sweeps past 0 on the second time around.
 *
 * @return the current heading of the robot in degrees. This heading is based on integration
 * of the returned rate from the gyro.
 */
float Gyro::GetAngle() const
{
    return impl->GetAngle();
}


/**
 * Return the rate of rotation of the gyro
 *
 * The rate is based on the most recent reading of the gyro analog value
 *
 * @return the current rate in degrees per second
 */
double Gyro::GetRate() const
{
    return impl->GetVelocity();
}

void Gyro::SetPIDSourceType(PIDSourceType pidSource)
{
    m_pidSource = pidSource;
}

/**
 * Get the angle in degrees for the PIDSource base object.
 *
 * @return The angle in degrees.
 */
double Gyro::PIDGet()
{
	switch(GetPIDSourceType()){
	case PIDSourceType::kRate:
		return GetRate();
	case PIDSourceType::kDisplacement:
		return GetAngle();
	default:
		return 0;
	}
}

void Gyro::UpdateTable() {
	if (m_table != nullptr) {
		m_table->PutNumber("Value", GetAngle());
	}
}

void Gyro::StartLiveWindowMode() {

}

void Gyro::StopLiveWindowMode() {

}

std::string Gyro::GetSmartDashboardType() const {
	return "Gyro";
}

void Gyro::InitTable(std::shared_ptr<ITable> subTable) {
	m_table = subTable;
	UpdateTable();
}

std::shared_ptr<ITable> Gyro::GetTable() const {
	return m_table;
}
