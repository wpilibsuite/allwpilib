// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/ADXL345_I2C.h"

#include <hal/UsageReporting.h>
#include <wpi/telemetry/TelemetryTable.h>

using namespace frc;

ADXL345_I2C::ADXL345_I2C(I2C::Port port, Range range, int deviceAddress)
    : m_i2c(port, deviceAddress),
      m_simDevice("Accel:ADXL345_I2C", port, deviceAddress) {
  if (m_simDevice) {
    m_simRange = m_simDevice.CreateEnumDouble("range", hal::SimDevice::kOutput,
                                              {"2G", "4G", "8G", "16G"},
                                              {2.0, 4.0, 8.0, 16.0}, 0);
    m_simX = m_simDevice.CreateDouble("x", hal::SimDevice::kInput, 0.0);
    m_simY = m_simDevice.CreateDouble("y", hal::SimDevice::kInput, 0.0);
    m_simZ = m_simDevice.CreateDouble("z", hal::SimDevice::kInput, 0.0);
  }
  // Turn on the measurements
  m_i2c.Write(kPowerCtlRegister, kPowerCtl_Measure);
  // Specify the data format to read
  SetRange(range);

  HAL_ReportUsage(
      fmt::format("I2C[{}][{}]", static_cast<int>(port), deviceAddress),
      "ADXL345");
}

I2C::Port ADXL345_I2C::GetI2CPort() const {
  return m_i2c.GetPort();
}

int ADXL345_I2C::GetI2CDeviceAddress() const {
  return m_i2c.GetDeviceAddress();
}

void ADXL345_I2C::SetRange(Range range) {
  m_i2c.Write(kDataFormatRegister,
              kDataFormat_FullRes | static_cast<uint8_t>(range));
}

double ADXL345_I2C::GetX() {
  return GetAcceleration(kAxis_X);
}

double ADXL345_I2C::GetY() {
  return GetAcceleration(kAxis_Y);
}

double ADXL345_I2C::GetZ() {
  return GetAcceleration(kAxis_Z);
}

double ADXL345_I2C::GetAcceleration(ADXL345_I2C::Axes axis) const {
  if (axis == kAxis_X && m_simX) {
    return m_simX.Get();
  }
  if (axis == kAxis_Y && m_simY) {
    return m_simY.Get();
  }
  if (axis == kAxis_Z && m_simZ) {
    return m_simZ.Get();
  }
  int16_t rawAccel = 0;
  m_i2c.Read(kDataRegister + static_cast<int>(axis), sizeof(rawAccel),
             reinterpret_cast<uint8_t*>(&rawAccel));
  return rawAccel * kGsPerLSB;
}

ADXL345_I2C::AllAxes ADXL345_I2C::GetAccelerations() const {
  if (m_simX && m_simY && m_simZ) {
    return AllAxes{m_simX.Get(), m_simY.Get(), m_simZ.Get()};
  }
  int16_t rawData[3];
  m_i2c.Read(kDataRegister, sizeof(rawData),
             reinterpret_cast<uint8_t*>(rawData));
  return AllAxes{rawData[0] * kGsPerLSB, rawData[1] * kGsPerLSB,
                 rawData[2] * kGsPerLSB};
}

void ADXL345_I2C::UpdateTelemetry(wpi::TelemetryTable& table) const {
  table.Log("Value", GetAccelerations());
}

std::string_view ADXL345_I2C::GetTelemetryType() const {
  return "3AxisAccelerometer";
}

frc::ADXL345_I2C::AllAxes wpi::Struct<frc::ADXL345_I2C::AllAxes>::Unpack(
    std::span<const uint8_t> data) {
  return ADXL345_I2C::AllAxes{wpi::UnpackStruct<double, 0>(data),
                              wpi::UnpackStruct<double, 8>(data),
                              wpi::UnpackStruct<double, 16>(data)};
}

void wpi::Struct<frc::ADXL345_I2C::AllAxes>::Pack(
    std::span<uint8_t> data, const frc::ADXL345_I2C::AllAxes& value) {
  wpi::PackStruct<0>(data, value.x);
  wpi::PackStruct<8>(data, value.y);
  wpi::PackStruct<16>(data, value.z);
}
