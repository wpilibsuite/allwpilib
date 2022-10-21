// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/ADXL362.h"

#include <hal/FRCUsageReporting.h>
#include <networktables/DoubleTopic.h>
#include <networktables/NTSendableBuilder.h>
#include <wpi/sendable/SendableRegistry.h>

#include "frc/Errors.h"

using namespace frc;

static constexpr int kRegWrite = 0x0A;
static constexpr int kRegRead = 0x0B;

static constexpr int kPartIdRegister = 0x02;
static constexpr int kDataRegister = 0x0E;
static constexpr int kFilterCtlRegister = 0x2C;
static constexpr int kPowerCtlRegister = 0x2D;

// static constexpr int kFilterCtl_Range2G = 0x00;
// static constexpr int kFilterCtl_Range4G = 0x40;
// static constexpr int kFilterCtl_Range8G = 0x80;
static constexpr int kFilterCtl_ODR_100Hz = 0x03;

static constexpr int kPowerCtl_UltraLowNoise = 0x20;
// static constexpr int kPowerCtl_AutoSleep = 0x04;
static constexpr int kPowerCtl_Measure = 0x02;

ADXL362::ADXL362(Range range) : ADXL362(SPI::Port::kOnboardCS1, range) {}

ADXL362::ADXL362(SPI::Port port, Range range)
    : m_spi(port), m_simDevice("Accel:ADXL362", port) {
  if (m_simDevice) {
    m_simRange = m_simDevice.CreateEnumDouble("range", hal::SimDevice::kOutput,
                                              {"2G", "4G", "8G", "16G"},
                                              {2.0, 4.0, 8.0, 16.0}, 0);
    m_simX = m_simDevice.CreateDouble("x", hal::SimDevice::kInput, 0.0);
    m_simY = m_simDevice.CreateDouble("y", hal::SimDevice::kInput, 0.0);
    m_simZ = m_simDevice.CreateDouble("z", hal::SimDevice::kInput, 0.0);
  }

  m_spi.SetClockRate(3000000);
  m_spi.SetMode(frc::SPI::Mode::kMode3);
  m_spi.SetChipSelectActiveLow();

  uint8_t commands[3];
  if (!m_simDevice) {
    // Validate the part ID
    commands[0] = kRegRead;
    commands[1] = kPartIdRegister;
    commands[2] = 0;
    m_spi.Transaction(commands, commands, 3);
    if (commands[2] != 0xF2) {
      FRC_ReportError(err::Error, "could not find ADXL362");
      m_gsPerLSB = 0.0;
      return;
    }
  }

  SetRange(range);

  // Turn on the measurements
  commands[0] = kRegWrite;
  commands[1] = kPowerCtlRegister;
  commands[2] = kPowerCtl_Measure | kPowerCtl_UltraLowNoise;
  m_spi.Write(commands, 3);

  HAL_Report(HALUsageReporting::kResourceType_ADXL362, port + 1);

  wpi::SendableRegistry::AddLW(this, "ADXL362", port);
}

SPI::Port ADXL362::GetSpiPort() const {
  return m_spi.GetPort();
}

void ADXL362::SetRange(Range range) {
  if (m_gsPerLSB == 0.0) {
    return;
  }

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

  if (m_simRange) {
    m_simRange.Set(range);
  }
}

double ADXL362::GetX() {
  return GetAcceleration(kAxis_X);
}

double ADXL362::GetY() {
  return GetAcceleration(kAxis_Y);
}

double ADXL362::GetZ() {
  return GetAcceleration(kAxis_Z);
}

double ADXL362::GetAcceleration(ADXL362::Axes axis) {
  if (m_gsPerLSB == 0.0) {
    return 0.0;
  }

  if (axis == kAxis_X && m_simX) {
    return m_simX.Get();
  }
  if (axis == kAxis_Y && m_simY) {
    return m_simY.Get();
  }
  if (axis == kAxis_Z && m_simZ) {
    return m_simZ.Get();
  }

  uint8_t buffer[4];
  uint8_t command[4] = {0, 0, 0, 0};
  command[0] = kRegRead;
  command[1] = kDataRegister + static_cast<uint8_t>(axis);
  m_spi.Transaction(command, buffer, 4);

  // Sensor is little endian... swap bytes
  int16_t rawAccel = buffer[3] << 8 | buffer[2];
  return rawAccel * m_gsPerLSB;
}

ADXL362::AllAxes ADXL362::GetAccelerations() {
  AllAxes data;
  if (m_gsPerLSB == 0.0) {
    data.XAxis = data.YAxis = data.ZAxis = 0.0;
    return data;
  }
  if (m_simX && m_simY && m_simZ) {
    data.XAxis = m_simX.Get();
    data.YAxis = m_simY.Get();
    data.ZAxis = m_simZ.Get();
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

void ADXL362::InitSendable(nt::NTSendableBuilder& builder) {
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
