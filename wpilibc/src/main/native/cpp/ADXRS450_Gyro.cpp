// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/ADXRS450_Gyro.h"

#include <hal/FRCUsageReporting.h>
#include <wpi/sendable/SendableBuilder.h>
#include <wpi/sendable/SendableRegistry.h>

#include "frc/Errors.h"
#include "frc/Timer.h"

using namespace frc;

static constexpr auto kSamplePeriod = 0.5_ms;
static constexpr auto kCalibrationSampleTime = 5_s;
static constexpr double kDegreePerSecondPerLSB = 0.0125;

// static constexpr int kRateRegister = 0x00;
// static constexpr int kTemRegister = 0x02;
// static constexpr int kLoCSTRegister = 0x04;
// static constexpr int kHiCSTRegister = 0x06;
// static constexpr int kQuadRegister = 0x08;
// static constexpr int kFaultRegister = 0x0A;
static constexpr int kPIDRegister = 0x0C;
// static constexpr int kSNHighRegister = 0x0E;
// static constexpr int kSNLowRegister = 0x10;

ADXRS450_Gyro::ADXRS450_Gyro() : ADXRS450_Gyro(SPI::kOnboardCS0) {}

ADXRS450_Gyro::ADXRS450_Gyro(SPI::Port port)
    : m_spi(port), m_port(port), m_simDevice("Gyro:ADXRS450", port) {
  if (m_simDevice) {
    m_simConnected =
        m_simDevice.CreateBoolean("connected", hal::SimDevice::kInput, true);
    m_simAngle =
        m_simDevice.CreateDouble("angle_x", hal::SimDevice::kInput, 0.0);
    m_simRate = m_simDevice.CreateDouble("rate_x", hal::SimDevice::kInput, 0.0);
  }

  m_spi.SetClockRate(3000000);
  m_spi.SetMode(frc::SPI::Mode::kMode0);
  m_spi.SetChipSelectActiveLow();

  if (!m_simDevice) {
    // Validate the part ID
    if ((ReadRegister(kPIDRegister) & 0xff00) != 0x5200) {
      FRC_ReportError(err::Error, "could not find ADXRS450 gyro");
      return;
    }

    m_spi.InitAccumulator(kSamplePeriod, 0x20000000u, 4, 0x0c00000eu,
                          0x04000000u, 10u, 16u, true, true);

    Calibrate();
  }

  HAL_Report(HALUsageReporting::kResourceType_ADXRS450, port + 1);

  wpi::SendableRegistry::AddLW(this, "ADXRS450_Gyro", port);
  m_connected = true;
}

bool ADXRS450_Gyro::IsConnected() const {
  if (m_simConnected) {
    return m_simConnected.Get();
  }
  return m_connected;
}

static bool CalcParity(uint32_t v) {
  bool parity = false;
  while (v != 0) {
    parity = !parity;
    v = v & (v - 1);
  }
  return parity;
}

static inline int BytesToIntBE(uint8_t* buf) {
  int result = static_cast<int>(buf[0]) << 24;
  result |= static_cast<int>(buf[1]) << 16;
  result |= static_cast<int>(buf[2]) << 8;
  result |= static_cast<int>(buf[3]);
  return result;
}

uint16_t ADXRS450_Gyro::ReadRegister(int reg) {
  uint32_t cmd = 0x80000000 | static_cast<int>(reg) << 17;
  if (!CalcParity(cmd)) {
    cmd |= 1u;
  }

  // big endian
  uint8_t buf[4] = {static_cast<uint8_t>((cmd >> 24) & 0xff),
                    static_cast<uint8_t>((cmd >> 16) & 0xff),
                    static_cast<uint8_t>((cmd >> 8) & 0xff),
                    static_cast<uint8_t>(cmd & 0xff)};

  m_spi.Write(buf, 4);
  m_spi.Read(false, buf, 4);
  if ((buf[0] & 0xe0) == 0) {
    return 0;  // error, return 0
  }
  return static_cast<uint16_t>((BytesToIntBE(buf) >> 5) & 0xffff);
}

double ADXRS450_Gyro::GetAngle() const {
  if (m_simAngle) {
    return m_simAngle.Get();
  }
  return m_spi.GetAccumulatorIntegratedValue() * kDegreePerSecondPerLSB;
}

double ADXRS450_Gyro::GetRate() const {
  if (m_simRate) {
    return m_simRate.Get();
  }
  return static_cast<double>(m_spi.GetAccumulatorLastValue()) *
         kDegreePerSecondPerLSB;
}

void ADXRS450_Gyro::Reset() {
  if (m_simAngle) {
    m_simAngle.Reset();
  }
  m_spi.ResetAccumulator();
}

void ADXRS450_Gyro::Calibrate() {
  Wait(100_ms);

  m_spi.SetAccumulatorIntegratedCenter(0);
  m_spi.ResetAccumulator();

  Wait(kCalibrationSampleTime);

  m_spi.SetAccumulatorIntegratedCenter(m_spi.GetAccumulatorIntegratedAverage());
  m_spi.ResetAccumulator();
}

Rotation2d ADXRS450_Gyro::GetRotation2d() const {
  return units::degree_t{-GetAngle()};
}

int ADXRS450_Gyro::GetPort() const {
  return m_port;
}

void ADXRS450_Gyro::InitSendable(wpi::SendableBuilder& builder) {
  builder.SetSmartDashboardType("Gyro");
  builder.AddDoubleProperty("Value", [=, this] { return GetAngle(); }, nullptr);
}
