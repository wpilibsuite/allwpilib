/*----------------------------------------------------------------------------*/
/* Copyright (c) 2008-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "frc/ADXL345_I2C.h"

#include <hal/HAL.h>

#include "frc/smartdashboard/SendableBuilder.h"

using namespace frc;

ADXL345_I2C::ADXL345_I2C(I2C::Port port, Range range, int deviceAddress)
    : m_i2c(port, deviceAddress) {
  // Turn on the measurements
  m_i2c.Write(kPowerCtlRegister, kPowerCtl_Measure);
  // Specify the data format to read
  SetRange(range);

  HAL_Report(HALUsageReporting::kResourceType_ADXL345,
             HALUsageReporting::kADXL345_I2C, 0);
  SetName("ADXL345_I2C", port);
}

void ADXL345_I2C::SetRange(Range range) {
  m_i2c.Write(kDataFormatRegister,
              kDataFormat_FullRes | static_cast<uint8_t>(range));
}

double ADXL345_I2C::GetX() { return GetAcceleration(kAxis_X); }

double ADXL345_I2C::GetY() { return GetAcceleration(kAxis_Y); }

double ADXL345_I2C::GetZ() { return GetAcceleration(kAxis_Z); }

double ADXL345_I2C::GetAcceleration(ADXL345_I2C::Axes axis) {
  int16_t rawAccel = 0;
  m_i2c.Read(kDataRegister + static_cast<int>(axis), sizeof(rawAccel),
             reinterpret_cast<uint8_t*>(&rawAccel));
  return rawAccel * kGsPerLSB;
}

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

void ADXL345_I2C::InitSendable(SendableBuilder& builder) {
  builder.SetSmartDashboardType("3AxisAccelerometer");
  auto x = builder.GetEntry("X").GetHandle();
  auto y = builder.GetEntry("Y").GetHandle();
  auto z = builder.GetEntry("Z").GetHandle();
  builder.SetUpdateTable([=]() {
    auto data = GetAccelerations();
    nt::NetworkTableEntry(x).SetDouble(data.XAxis);
    nt::NetworkTableEntry(y).SetDouble(data.YAxis);
    nt::NetworkTableEntry(z).SetDouble(data.ZAxis);
  });
}
