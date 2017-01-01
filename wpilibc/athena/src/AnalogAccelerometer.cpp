/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008-2017. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "AnalogAccelerometer.h"

#include "HAL/HAL.h"
#include "LiveWindow/LiveWindow.h"
#include "WPIErrors.h"

using namespace frc;

/**
 * Common function for initializing the accelerometer.
 */
void AnalogAccelerometer::InitAccelerometer() {
  HAL_Report(HALUsageReporting::kResourceType_Accelerometer,
             m_analogInput->GetChannel());
  LiveWindow::GetInstance()->AddSensor("Accelerometer",
                                       m_analogInput->GetChannel(), this);
}

/**
 * Create a new instance of an accelerometer.
 *
 * The constructor allocates desired analog input.
 *
 * @param channel The channel number for the analog input the accelerometer is
 *                connected to
 */
AnalogAccelerometer::AnalogAccelerometer(int channel) {
  m_analogInput = std::make_shared<AnalogInput>(channel);
  InitAccelerometer();
}

/**
 * Create a new instance of Accelerometer from an existing AnalogInput.
 *
 * Make a new instance of accelerometer given an AnalogInput. This is
 * particularly useful if the port is going to be read as an analog channel as
 * well as through the Accelerometer class.
 *
 * @param channel The existing AnalogInput object for the analog input the
 *                accelerometer is connected to
 */
AnalogAccelerometer::AnalogAccelerometer(AnalogInput* channel)
    : m_analogInput(channel, NullDeleter<AnalogInput>()) {
  if (channel == nullptr) {
    wpi_setWPIError(NullParameter);
  } else {
    InitAccelerometer();
  }
}

/**
 * Create a new instance of Accelerometer from an existing AnalogInput.
 *
 * Make a new instance of accelerometer given an AnalogInput. This is
 * particularly useful if the port is going to be read as an analog channel as
 * well as through the Accelerometer class.
 *
 * @param channel The existing AnalogInput object for the analog input the
 *                accelerometer is connected to
 */
AnalogAccelerometer::AnalogAccelerometer(std::shared_ptr<AnalogInput> channel)
    : m_analogInput(channel) {
  if (channel == nullptr) {
    wpi_setWPIError(NullParameter);
  } else {
    InitAccelerometer();
  }
}

/**
 * Return the acceleration in Gs.
 *
 * The acceleration is returned units of Gs.
 *
 * @return The current acceleration of the sensor in Gs.
 */
double AnalogAccelerometer::GetAcceleration() const {
  return (m_analogInput->GetAverageVoltage() - m_zeroGVoltage) / m_voltsPerG;
}

/**
 * Set the accelerometer sensitivity.
 *
 * This sets the sensitivity of the accelerometer used for calculating the
 * acceleration. The sensitivity varies by accelerometer model. There are
 * constants defined for various models.
 *
 * @param sensitivity The sensitivity of accelerometer in Volts per G.
 */
void AnalogAccelerometer::SetSensitivity(double sensitivity) {
  m_voltsPerG = sensitivity;
}

/**
 * Set the voltage that corresponds to 0 G.
 *
 * The zero G voltage varies by accelerometer model. There are constants defined
 * for various models.
 *
 * @param zero The zero G voltage.
 */
void AnalogAccelerometer::SetZero(double zero) { m_zeroGVoltage = zero; }

/**
 * Get the Acceleration for the PID Source parent.
 *
 * @return The current acceleration in Gs.
 */
double AnalogAccelerometer::PIDGet() { return GetAcceleration(); }

void AnalogAccelerometer::UpdateTable() {
  if (m_table != nullptr) {
    m_table->PutNumber("Value", GetAcceleration());
  }
}

void AnalogAccelerometer::StartLiveWindowMode() {}

void AnalogAccelerometer::StopLiveWindowMode() {}

std::string AnalogAccelerometer::GetSmartDashboardType() const {
  return "Accelerometer";
}

void AnalogAccelerometer::InitTable(std::shared_ptr<ITable> subTable) {
  m_table = subTable;
  UpdateTable();
}

std::shared_ptr<ITable> AnalogAccelerometer::GetTable() const {
  return m_table;
}
