/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008-2017. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "ADXL362.h"

#include "DigitalInput.h"
#include "DigitalOutput.h"
#include "DriverStation.h"
#include "HAL/HAL.h"
#include "LiveWindow/LiveWindow.h"

using namespace frc;

static int kRegWrite = 0x0A;
static int kRegRead = 0x0B;

static int kPartIdRegister = 0x02;
static int kDataRegister = 0x0E;
static int kFilterCtlRegister = 0x2C;
static int kPowerCtlRegister = 0x2D;

// static int kFilterCtl_Range2G = 0x00;
// static int kFilterCtl_Range4G = 0x40;
// static int kFilterCtl_Range8G = 0x80;
static int kFilterCtl_ODR_100Hz = 0x03;

static int kPowerCtl_UltraLowNoise = 0x20;
// static int kPowerCtl_AutoSleep = 0x04;
static int kPowerCtl_Measure = 0x02;

/**
 * Constructor.  Uses the onboard CS1.
 *
 * @param range The range (+ or -) that the accelerometer will measure.
 */
ADXL362::ADXL362(Range range) : ADXL362(SPI::Port::kOnboardCS1, range) {}

/**
 * Constructor.
 *
 * @param port  The SPI port the accelerometer is attached to
 * @param range The range (+ or -) that the accelerometer will measure.
 */
ADXL362::ADXL362(SPI::Port port, Range range) : m_spi(port) {
  m_spi.SetClockRate(3000000);
  m_spi.SetMSBFirst();
  m_spi.SetSampleDataOnFalling();
  m_spi.SetClockActiveLow();
  m_spi.SetChipSelectActiveLow();

  // Validate the part ID
  uint8_t commands[3];
  commands[0] = kRegRead;
  commands[1] = kPartIdRegister;
  commands[2] = 0;
  m_spi.Transaction(commands, commands, 3);
  if (commands[2] != 0xF2) {
    DriverStation::ReportError("could not find ADXL362");
    m_gsPerLSB = 0.0;
    return;
  }

  SetRange(range);

  // Turn on the measurements
  commands[0] = kRegWrite;
  commands[1] = kPowerCtlRegister;
  commands[2] = kPowerCtl_Measure | kPowerCtl_UltraLowNoise;
  m_spi.Write(commands, 3);

  HAL_Report(HALUsageReporting::kResourceType_ADXL362, port);

  LiveWindow::GetInstance()->AddSensor("ADXL362", port, this);
}

void ADXL362::SetRange(Range range) {
  if (m_gsPerLSB == 0.0) return;

  uint8_t commands[3];

  switch (range) {
    case kRange_2G:
      m_gsPerLSB = 0.001;
      break;
    case kRange_4G:
      m_gsPerLSB = 0.002;
      break;
    case kRange_8G:
    case kRange_16G:  // 16G not supported; treat as 8G
      m_gsPerLSB = 0.004;
      break;
  }

  // Specify the data format to read
  commands[0] = kRegWrite;
  commands[1] = kFilterCtlRegister;
  commands[2] =
      kFilterCtl_ODR_100Hz | static_cast<uint8_t>((range & 0x03) << 6);
  m_spi.Write(commands, 3);
}

double ADXL362::GetX() { return GetAcceleration(kAxis_X); }

double ADXL362::GetY() { return GetAcceleration(kAxis_Y); }

double ADXL362::GetZ() { return GetAcceleration(kAxis_Z); }

/**
 * Get the acceleration of one axis in Gs.
 *
 * @param axis The axis to read from.
 * @return Acceleration of the ADXL362 in Gs.
 */
double ADXL362::GetAcceleration(ADXL362::Axes axis) {
  if (m_gsPerLSB == 0.0) return 0.0;

  uint8_t buffer[4];
  uint8_t command[4] = {0, 0, 0, 0};
  command[0] = kRegRead;
  command[1] = kDataRegister + static_cast<uint8_t>(axis);
  m_spi.Transaction(command, buffer, 4);

  // Sensor is little endian... swap bytes
  int16_t rawAccel = buffer[3] << 8 | buffer[2];
  return rawAccel * m_gsPerLSB;
}

/**
 * Get the acceleration of all axes in Gs.
 *
 * @return An object containing the acceleration measured on each axis of the
 *         ADXL362 in Gs.
 */
ADXL362::AllAxes ADXL362::GetAccelerations() {
  AllAxes data = AllAxes();
  if (m_gsPerLSB == 0.0) {
    data.XAxis = data.YAxis = data.ZAxis = 0.0;
    return data;
  }

  uint8_t dataBuffer[8] = {0, 0, 0, 0, 0, 0, 0, 0};
  int16_t rawData[3];

  // Select the data address.
  dataBuffer[0] = kRegRead;
  dataBuffer[1] = kDataRegister;
  m_spi.Transaction(dataBuffer, dataBuffer, 8);

  for (int i = 0; i < 3; i++) {
    // Sensor is little endian... swap bytes
    rawData[i] = dataBuffer[i * 2 + 3] << 8 | dataBuffer[i * 2 + 2];
  }

  data.XAxis = rawData[0] * m_gsPerLSB;
  data.YAxis = rawData[1] * m_gsPerLSB;
  data.ZAxis = rawData[2] * m_gsPerLSB;

  return data;
}

std::string ADXL362::GetSmartDashboardType() const {
  return "3AxisAccelerometer";
}

void ADXL362::InitTable(std::shared_ptr<ITable> subtable) {
  m_table = subtable;
  UpdateTable();
}

void ADXL362::UpdateTable() {
  if (m_table != nullptr) {
    m_table->PutNumber("X", GetX());
    m_table->PutNumber("Y", GetY());
    m_table->PutNumber("Z", GetZ());
  }
}

std::shared_ptr<ITable> ADXL362::GetTable() const { return m_table; }
