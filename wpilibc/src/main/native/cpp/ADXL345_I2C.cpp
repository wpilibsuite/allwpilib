// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/ADXL345_I2C.h"

#include <hal/UsageReporting.h>
#include <networktables/DoubleTopic.h>
#include <networktables/NTSendableBuilder.h>
#include <wpi/sendable/SendableRegistry.h>

using namespace frc;

ADXL345_I2C::ADXL345_I2C(I2C::Port port, Range range, int deviceAddress)
    : m_i2c(port, deviceAddress),
      m_simDevice("Accel:ADXL345_I2C", port, deviceAddress) {
  if (m_simDevice) {
    m_simRange = m_simDevice.CreateEnumDouble("range", hal::SimDevice::OUTPUT,
                                              {"2G", "4G", "8G", "16G"},
                                              {2.0, 4.0, 8.0, 16.0}, 0);
    m_simX = m_simDevice.CreateDouble("x", hal::SimDevice::INPUT, 0.0);
    m_simY = m_simDevice.CreateDouble("y", hal::SimDevice::INPUT, 0.0);
    m_simZ = m_simDevice.CreateDouble("z", hal::SimDevice::INPUT, 0.0);
  }
  // Turn on the measurements
  m_i2c.Write(POWER_CTL_REGISTER, POWER_CTL_MEASURE);
  // Specify the data format to read
  SetRange(range);

  HAL_ReportUsage(
      fmt::format("I2C[{}][{}]", static_cast<int>(port), deviceAddress),
      "ADXL345");

  wpi::SendableRegistry::Add(this, "ADXL345_I2C", port);
}

I2C::Port ADXL345_I2C::GetI2CPort() const {
  return m_i2c.GetPort();
}

int ADXL345_I2C::GetI2CDeviceAddress() const {
  return m_i2c.GetDeviceAddress();
}

void ADXL345_I2C::SetRange(Range range) {
  m_i2c.Write(DATA_FORMAT_REGISTER,
              DATA_FORMAT_FULL_RES | static_cast<uint8_t>(range));
}

double ADXL345_I2C::GetX() {
  return GetAcceleration(AXIS_X);
}

double ADXL345_I2C::GetY() {
  return GetAcceleration(AXIS_Y);
}

double ADXL345_I2C::GetZ() {
  return GetAcceleration(AXIS_Z);
}

double ADXL345_I2C::GetAcceleration(ADXL345_I2C::Axes axis) {
  if (axis == AXIS_X && m_simX) {
    return m_simX.Get();
  }
  if (axis == AXIS_Y && m_simY) {
    return m_simY.Get();
  }
  if (axis == AXIS_Z && m_simZ) {
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
    data.XAxis = m_simX.Get();
    data.YAxis = m_simY.Get();
    data.ZAxis = m_simZ.Get();
    return data;
  }
  int16_t rawData[3];
  m_i2c.Read(DATA_REGISTER, sizeof(rawData),
             reinterpret_cast<uint8_t*>(rawData));

  data.XAxis = rawData[0] * GS_PER_LSB;
  data.YAxis = rawData[1] * GS_PER_LSB;
  data.ZAxis = rawData[2] * GS_PER_LSB;
  return data;
}

void ADXL345_I2C::InitSendable(nt::NTSendableBuilder& builder) {
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
