/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008. All Rights Reserved.							  */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in $(WIND_BASE)/WPILib.  */
/*----------------------------------------------------------------------------*/

#include "Accelerometer.h"
//#include "NetworkCommunication/UsageReporting.h"
#include "WPIErrors.h"
#include "LiveWindow/LiveWindow.h"

/**
 * Common function for initializing the accelerometer.
 */
void Accelerometer::InitAccelerometer()
{
	m_table = NULL;
	m_voltsPerG = 1.0;
	m_zeroGVoltage = 2.5;
	HALReport(HALUsageReporting::kResourceType_Accelerometer, m_AnalogInput->GetChannel());
	LiveWindow::GetInstance()->AddSensor("Accelerometer", m_AnalogInput->GetChannel(), this);
}

/**
 * Create a new instance of an accelerometer.
 *
 * The constructor allocates desired analog input.
 */
Accelerometer::Accelerometer(uint32_t channel)
{
	m_AnalogInput = new AnalogInput(channel);
	m_allocatedChannel = true;
	InitAccelerometer();
}

/**
 * Create a new instance of Accelerometer from an existing AnalogInput.
 * Make a new instance of accelerometer given an AnalogInput. This is particularly
 * useful if the port is going to be read as an analog channel as well as through
 * the Accelerometer class.
 */
Accelerometer::Accelerometer(AnalogInput *channel)
{
	if (channel == NULL)
	{
		wpi_setWPIError(NullParameter);
	}
	else
	{
		m_AnalogInput = channel;
		InitAccelerometer();
	}
	m_allocatedChannel = false;
}

/**
 * Delete the analog components used for the accelerometer.
 */
Accelerometer::~Accelerometer()
{
	if (m_allocatedChannel)
	{
		delete m_AnalogInput;
	}
}

/**
 * Return the acceleration in Gs.
 *
 * The acceleration is returned units of Gs.
 *
 * @return The current acceleration of the sensor in Gs.
 */
float Accelerometer::GetAcceleration()
{
	return (m_AnalogInput->GetAverageVoltage() - m_zeroGVoltage) / m_voltsPerG;
}

/**
 * Set the accelerometer sensitivity.
 *
 * This sets the sensitivity of the accelerometer used for calculating the acceleration.
 * The sensitivity varys by accelerometer model. There are constants defined for various models.
 *
 * @param sensitivity The sensitivity of accelerometer in Volts per G.
 */
void Accelerometer::SetSensitivity(float sensitivity)
{
	m_voltsPerG = sensitivity;
}

/**
 * Set the voltage that corresponds to 0 G.
 *
 * The zero G voltage varys by accelerometer model. There are constants defined for various models.
 *
 * @param zero The zero G voltage.
 */
void Accelerometer::SetZero(float zero)
{
	m_zeroGVoltage = zero;
}

/**
 * Get the Acceleration for the PID Source parent.
 *
 * @return The current acceleration in Gs.
 */
double Accelerometer::PIDGet()
{
	return GetAcceleration();
}

void Accelerometer::UpdateTable() {
	if (m_table != NULL) {
		m_table->PutNumber("Value", GetAcceleration());
	}
}

void Accelerometer::StartLiveWindowMode() {
}

void Accelerometer::StopLiveWindowMode() {
}

std::string Accelerometer::GetSmartDashboardType() {
	return "Accelerometer";
}

void Accelerometer::InitTable(ITable *subTable) {
	m_table = subTable;
	UpdateTable();
}

ITable * Accelerometer::GetTable() {
	return m_table;
}
