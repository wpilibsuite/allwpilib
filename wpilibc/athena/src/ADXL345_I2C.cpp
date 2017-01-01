/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008-2017. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "ADXL345_I2C.h"

#include "HAL/HAL.h"
#include "I2C.h"
#include "LiveWindow/LiveWindow.h"

using namespace frc;

const int ADXL345_I2C::kAddress;
const int ADXL345_I2C::kPowerCtlRegister;
const int ADXL345_I2C::kDataFormatRegister;
const int ADXL345_I2C::kDataRegister;
constexpr double ADXL345_I2C::kGsPerLSB;

/**
 * Constructs the ADXL345 Accelerometer over I2C.
 *
 * @param port          The I2C port the accelerometer is attached to
 * @param range         The range (+ or -) that the accelerometer will measure
 * @param deviceAddress The I2C address of the accelerometer (0x1D or 0x53)
 */
ADXL345_I2C::ADXL345_I2C(I2C::Port port, Range range, int deviceAddress)
    : m_i2c(port, deviceAddress) {
  // Turn on the measurements
  m_i2c.Write(kPowerCtlRegister, kPowerCtl_Measure);
  // Specify the data format to read
  SetRange(range);

  HAL_Report(HALUsageReporting::kResourceType_ADXL345,
             HALUsageReporting::kADXL345_I2C, 0);
  LiveWindow::GetInstance()->AddSensor("ADXL345_I2C", port, this);
}

void ADXL345_I2C::SetRange(Range range) {
  m_i2c.Write(kDataFormatRegister,
              kDataFormat_FullRes | static_cast<uint8_t>(range));
}

double ADXL345_I2C::GetX() { return GetAcceleration(kAxis_X); }

double ADXL345_I2C::GetY() { return GetAcceleration(kAxis_Y); }

double ADXL345_I2C::GetZ() { return GetAcceleration(kAxis_Z); }

/**
 * Get the acceleration of one axis in Gs.
 *
 * @param axis The axis to read from.
 * @return Acceleration of the ADXL345 in Gs.
 */
double ADXL345_I2C::GetAcceleration(ADXL345_I2C::Axes axis) {
  int16_t rawAccel = 0;
  m_i2c.Read(kDataRegister + static_cast<int>(axis), sizeof(rawAccel),
             reinterpret_cast<uint8_t*>(&rawAccel));
  return rawAccel * kGsPerLSB;
}

/**
 * Get the acceleration of all axes in Gs.
 *
 * @return An object containing the acceleration measured on each axis of the
 *         ADXL345 in Gs.
 */
ADXL345_I2C::AllAxes ADXL345_I2C::GetAccelerations() {
  AllAxes data = AllAxes();
  int16_t rawData[3];
  m_i2c.Read(kDataRegister, sizeof(rawData),
             reinterpret_cast<uint8_t*>(rawData));

  data.XAxis = rawData[0] * kGsPerLSB;
  data.YAxis = rawData[1] * kGsPerLSB;
  data.ZAxis = rawData[2] * kGsPerLSB;
  return data;
}

std::string ADXL345_I2C::GetSmartDashboardType() const {
  return "3AxisAccelerometer";
}

void ADXL345_I2C::InitTable(std::shared_ptr<ITable> subtable) {
  m_table = subtable;
  UpdateTable();
}

void ADXL345_I2C::UpdateTable() {
  m_table->PutNumber("X", GetX());
  m_table->PutNumber("Y", GetY());
  m_table->PutNumber("Z", GetZ());
}

std::shared_ptr<ITable> ADXL345_I2C::GetTable() const { return m_table; }
