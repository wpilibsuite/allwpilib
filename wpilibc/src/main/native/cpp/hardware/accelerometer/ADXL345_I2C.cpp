// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/hardware/accelerometer/ADXL345_I2C.hpp"

#include "wpi/hal/UsageReporting.hpp"
#include "wpi/nt/DoubleTopic.hpp"
#include "wpi/nt/NTSendableBuilder.hpp"
#include "wpi/util/sendable/SendableRegistry.hpp"

using namespace wpi;

static constexpr int POWER_CTL_REGISTER = 0x2D;
static constexpr int DATA_FORMAT_REGISTER = 0x31;
static constexpr int DATA_REGISTER = 0x32;
static constexpr double GS_PER_LSB = 0.00390625;

// static constexpr uint8_t POWER_CTL_LINK = 0x20;
// static constexpr uint8_t POWER_CTL_AUTO_SLEEP = 0x10;
static constexpr uint8_t POWER_CTL_MEASURE = 0x08;
// static constexpr uint8_t POWER_CTL_SLEEP = 0x04;

// static constexpr uint8_t DATA_FORMAT_SELF_TEST = 0x80;
// static constexpr uint8_t DATA_FORMAT_SPI = 0x40;
// static constexpr uint8_t DATA_FORMAT_INT_INVERT = 0x20;
static constexpr uint8_t DATA_FORMAT_FULL_RES = 0x08;
// static constexpr uint8_t DATA_FORMAT_JUSTIFY = 0x04;

ADXL345_I2C::ADXL345_I2C(I2C::Port port, int range, int deviceAddress)
    : m_i2c(port, deviceAddress),
      m_simDevice("Accel:ADXL345_I2C", static_cast<int>(port), deviceAddress) {
  if (m_simDevice) {
    m_simRange = m_simDevice.CreateEnumDouble(
        "range", wpi::hal::SimDevice::Direction::OUTPUT,
        {"2G", "4G", "8G", "16G"}, {2.0, 4.0, 8.0, 16.0}, 0);
    m_simX = m_simDevice.CreateDouble(
        "x", wpi::hal::SimDevice::Direction::INPUT, 0.0);
    m_simY = m_simDevice.CreateDouble(
        "y", wpi::hal::SimDevice::Direction::INPUT, 0.0);
    m_simZ = m_simDevice.CreateDouble(
        "z", wpi::hal::SimDevice::Direction::INPUT, 0.0);
  }
  // Turn on the measurements
  m_i2c.Write(POWER_CTL_REGISTER, POWER_CTL_MEASURE);
  // Specify the data format to read
  SetRange(range);

  HAL_ReportUsage(
      fmt::format("I2C[{}][{}]", static_cast<int>(port), deviceAddress),
      "ADXL345");

  wpi::util::SendableRegistry::Add(this, "ADXL345_I2C", static_cast<int>(port));
}

I2C::Port ADXL345_I2C::GetI2CPort() const {
  return m_i2c.GetPort();
}

int ADXL345_I2C::GetI2CDeviceAddress() const {
  return m_i2c.GetDeviceAddress();
}

void ADXL345_I2C::SetRange(int range) {
  uint8_t value;
  switch (range) {
    case 2:
      value = 0;
      break;
    case 4:
      value = 1;
      break;
    case 8:
      value = 2;
      break;
    case 16:
      value = 3;
      break;
    default:
      value = 0;  // default to 2G if invalid value is passed in
  }
  m_i2c.Write(DATA_FORMAT_REGISTER, DATA_FORMAT_FULL_RES | value);
  if (m_simRange) {
    m_simRange.Set(range);
  }
}

double ADXL345_I2C::GetX() {
  return GetAcceleration(Axis::X);
}

double ADXL345_I2C::GetY() {
  return GetAcceleration(Axis::Y);
}

double ADXL345_I2C::GetZ() {
  return GetAcceleration(Axis::Z);
}

double ADXL345_I2C::GetAcceleration(ADXL345_I2C::Axis axis) {
  if (axis == Axis::X && m_simX) {
    return m_simX.Get();
  }
  if (axis == Axis::Y && m_simY) {
    return m_simY.Get();
  }
  if (axis == Axis::Z && m_simZ) {
    return m_simZ.Get();
  }
  int16_t rawAccel = 0;
  m_i2c.Read(DATA_REGISTER + static_cast<int>(axis), sizeof(rawAccel),
             reinterpret_cast<uint8_t*>(&rawAccel));
  return rawAccel * GS_PER_LSB;
}

ADXL345_I2C::AllAxes ADXL345_I2C::GetAccelerations() {
  AllAxes data;
  if (m_simX && m_simY && m_simZ) {
    data.x = m_simX.Get();
    data.y = m_simY.Get();
    data.z = m_simZ.Get();
    return data;
  }
  int16_t rawData[3];
  m_i2c.Read(DATA_REGISTER, sizeof(rawData),
             reinterpret_cast<uint8_t*>(rawData));

  data.x = rawData[0] * GS_PER_LSB;
  data.y = rawData[1] * GS_PER_LSB;
  data.z = rawData[2] * GS_PER_LSB;
  return data;
}

void ADXL345_I2C::InitSendable(wpi::nt::NTSendableBuilder& builder) {
  builder.SetSmartDashboardType("3AxisAccelerometer");
  builder.SetUpdateTable(
      [this, x = wpi::nt::DoubleTopic{builder.GetTopic("X")}.Publish(),
       y = wpi::nt::DoubleTopic{builder.GetTopic("Y")}.Publish(),
       z = wpi::nt::DoubleTopic{builder.GetTopic("Z")}.Publish()]() mutable {
        auto data = GetAccelerations();
        x.Set(data.x);
        y.Set(data.y);
        z.Set(data.z);
      });
}
