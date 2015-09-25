/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008. All Rights Reserved.
 */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in $(WIND_BASE)/WPILib.  */
/*----------------------------------------------------------------------------*/

#include "ADXL345_SPI.h"
#include "DigitalInput.h"
#include "DigitalOutput.h"
#include "LiveWindow/LiveWindow.h"

const uint8_t ADXL345_SPI::kPowerCtlRegister;
const uint8_t ADXL345_SPI::kDataFormatRegister;
const uint8_t ADXL345_SPI::kDataRegister;
constexpr double ADXL345_SPI::kGsPerLSB;

/**
 * Constructor.
 *
 * @param port The SPI port the accelerometer is attached to
 * @param range The range (+ or -) that the accelerometer will measure.
 */
ADXL345_SPI::ADXL345_SPI(SPI::Port port, ADXL345_SPI::Range range) : SPI(port) {
  SetClockRate(500000);
  SetMSBFirst();
  SetSampleDataOnFalling();
  SetClockActiveLow();
  SetChipSelectActiveHigh();

  uint8_t commands[2];
  // Turn on the measurements
  commands[0] = kPowerCtlRegister;
  commands[1] = kPowerCtl_Measure;
  Transaction(commands, commands, 2);

  SetRange(range);

  HALReport(HALUsageReporting::kResourceType_ADXL345,
            HALUsageReporting::kADXL345_SPI);

  LiveWindow::GetInstance()->AddSensor("ADXL345_SPI", port, this);
}

/** {@inheritdoc} */
void ADXL345_SPI::SetRange(Range range) {
  uint8_t commands[2];

  // Specify the data format to read
  commands[0] = kDataFormatRegister;
  commands[1] = kDataFormat_FullRes | (uint8_t)(range & 0x03);
  Transaction(commands, commands, 2);
}

/** {@inheritdoc} */
double ADXL345_SPI::GetX() { return GetAcceleration(kAxis_X); }

/** {@inheritdoc} */
double ADXL345_SPI::GetY() { return GetAcceleration(kAxis_Y); }

/** {@inheritdoc} */
double ADXL345_SPI::GetZ() { return GetAcceleration(kAxis_Z); }

/**
 * Get the acceleration of one axis in Gs.
 *
 * @param axis The axis to read from.
 * @return Acceleration of the ADXL345 in Gs.
 */
double ADXL345_SPI::GetAcceleration(ADXL345_SPI::Axes axis) {
  uint8_t buffer[3];
  uint8_t command[3] = {0, 0, 0};
  command[0] =
      (kAddress_Read | kAddress_MultiByte | kDataRegister) + (uint8_t)axis;
  Transaction(command, buffer, 3);

  // Sensor is little endian... swap bytes
  int16_t rawAccel = buffer[2] << 8 | buffer[1];
  return rawAccel * kGsPerLSB;
}

/**
 * Get the acceleration of all axes in Gs.
 *
 * @return An object containing the acceleration measured on each axis of the
 * ADXL345 in Gs.
 */
ADXL345_SPI::AllAxes ADXL345_SPI::GetAccelerations() {
  AllAxes data = AllAxes();
  uint8_t dataBuffer[7] = {0, 0, 0, 0, 0, 0, 0};
  int16_t rawData[3];

  // Select the data address.
  dataBuffer[0] = (kAddress_Read | kAddress_MultiByte | kDataRegister);
  Transaction(dataBuffer, dataBuffer, 7);

  for (int32_t i = 0; i < 3; i++) {
    // Sensor is little endian... swap bytes
    rawData[i] = dataBuffer[i * 2 + 2] << 8 | dataBuffer[i * 2 + 1];
  }

  data.XAxis = rawData[0] * kGsPerLSB;
  data.YAxis = rawData[1] * kGsPerLSB;
  data.ZAxis = rawData[2] * kGsPerLSB;

  return data;
}

std::string ADXL345_SPI::GetSmartDashboardType() const {
  return "3AxisAccelerometer";
}

void ADXL345_SPI::InitTable(std::shared_ptr<ITable> subtable) {
  m_table = subtable;
  UpdateTable();
}

void ADXL345_SPI::UpdateTable() {
  if (m_table != nullptr) {
    m_table->PutNumber("X", GetX());
    m_table->PutNumber("Y", GetY());
    m_table->PutNumber("Z", GetZ());
  }
}

std::shared_ptr<ITable> ADXL345_SPI::GetTable() const { return m_table; }
