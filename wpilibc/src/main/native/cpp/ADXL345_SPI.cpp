// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/ADXL345_SPI.h"

#include <hal/FRCUsageReporting.h>
#include <networktables/DoubleTopic.h>
#include <networktables/NTSendableBuilder.h>
#include <wpi/sendable/SendableRegistry.h>

using namespace frc;

ADXL345_SPI::ADXL345_SPI(SPI::Port port, ADXL345_SPI::Range range)
    : m_spi(port), m_simDevice("Accel:ADXL345_SPI", port) {
  if (m_simDevice) {
    m_simRange = m_simDevice.CreateEnumDouble("range", hal::SimDevice::kOutput,
                                              {"2G", "4G", "8G", "16G"},
                                              {2.0, 4.0, 8.0, 16.0}, 0);
    m_simX = m_simDevice.CreateDouble("x", hal::SimDevice::kInput, 0.0);
    m_simY = m_simDevice.CreateDouble("y", hal::SimDevice::kInput, 0.0);
    m_simZ = m_simDevice.CreateDouble("z", hal::SimDevice::kInput, 0.0);
  }
  m_spi.SetClockRate(500000);
  m_spi.SetMode(frc::SPI::Mode::kMode3);
  m_spi.SetChipSelectActiveHigh();

  uint8_t commands[2];
  // Turn on the measurements
  commands[0] = kPowerCtlRegister;
  commands[1] = kPowerCtl_Measure;
  m_spi.Transaction(commands, commands, 2);

  SetRange(range);

  HAL_Report(HALUsageReporting::kResourceType_ADXL345,
             HALUsageReporting::kADXL345_SPI);

  wpi::SendableRegistry::AddLW(this, "ADXL345_SPI", port);
}

SPI::Port ADXL345_SPI::GetSpiPort() const {
  return m_spi.GetPort();
}

void ADXL345_SPI::SetRange(Range range) {
  uint8_t commands[2];

  // Specify the data format to read
  commands[0] = kDataFormatRegister;
  commands[1] = kDataFormat_FullRes | static_cast<uint8_t>(range & 0x03);
  m_spi.Transaction(commands, commands, 2);

  if (m_simRange) {
    m_simRange.Set(range);
  }
}

double ADXL345_SPI::GetX() {
  return GetAcceleration(kAxis_X);
}

double ADXL345_SPI::GetY() {
  return GetAcceleration(kAxis_Y);
}

double ADXL345_SPI::GetZ() {
  return GetAcceleration(kAxis_Z);
}

double ADXL345_SPI::GetAcceleration(ADXL345_SPI::Axes axis) {
  if (axis == kAxis_X && m_simX) {
    return m_simX.Get();
  }
  if (axis == kAxis_Y && m_simY) {
    return m_simY.Get();
  }
  if (axis == kAxis_Z && m_simZ) {
    return m_simZ.Get();
  }
  uint8_t buffer[3];
  uint8_t command[3] = {0, 0, 0};
  command[0] = (kAddress_Read | kAddress_MultiByte | kDataRegister) +
               static_cast<uint8_t>(axis);
  m_spi.Transaction(command, buffer, 3);

  // Sensor is little endian... swap bytes
  int16_t rawAccel = buffer[2] << 8 | buffer[1];
  return rawAccel * kGsPerLSB;
}

ADXL345_SPI::AllAxes ADXL345_SPI::GetAccelerations() {
  AllAxes data;
  if (m_simX && m_simY && m_simZ) {
    data.XAxis = m_simX.Get();
    data.YAxis = m_simY.Get();
    data.ZAxis = m_simZ.Get();
    return data;
  }

  uint8_t dataBuffer[7] = {0, 0, 0, 0, 0, 0, 0};
  int16_t rawData[3];

  // Select the data address.
  dataBuffer[0] = (kAddress_Read | kAddress_MultiByte | kDataRegister);
  m_spi.Transaction(dataBuffer, dataBuffer, 7);

  for (int i = 0; i < 3; i++) {
    // Sensor is little endian... swap bytes
    rawData[i] = dataBuffer[i * 2 + 2] << 8 | dataBuffer[i * 2 + 1];
  }

  data.XAxis = rawData[0] * kGsPerLSB;
  data.YAxis = rawData[1] * kGsPerLSB;
  data.ZAxis = rawData[2] * kGsPerLSB;

  return data;
}

void ADXL345_SPI::InitSendable(nt::NTSendableBuilder& builder) {
  builder.SetSmartDashboardType("3AxisAccelerometer");
  builder.SetUpdateTable(
      [this, x = nt::DoubleTopic{builder.GetTopic("X")}.Publish(),
       y = nt::DoubleTopic{builder.GetTopic("Y")}.Publish(),
       z = nt::DoubleTopic{builder.GetTopic("Z")}.Publish()]() mutable {
        auto data = GetAccelerations();
        x.Set(data.XAxis);
        y.Set(data.YAxis);
        z.Set(data.ZAxis);
      });
}
