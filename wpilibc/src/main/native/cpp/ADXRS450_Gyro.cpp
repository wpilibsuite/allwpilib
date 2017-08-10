/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2015-2017. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "ADXRS450_Gyro.h"

#include "DriverStation.h"
#include "HAL/HAL.h"
#include "LiveWindow/LiveWindow.h"
#include "Timer.h"

using namespace frc;

static constexpr double kSamplePeriod = 0.001;
static constexpr double kCalibrationSampleTime = 5.0;
static constexpr double kDegreePerSecondPerLSB = 0.0125;

static constexpr int kRateRegister = 0x00;
static constexpr int kTemRegister = 0x02;
static constexpr int kLoCSTRegister = 0x04;
static constexpr int kHiCSTRegister = 0x06;
static constexpr int kQuadRegister = 0x08;
static constexpr int kFaultRegister = 0x0A;
static constexpr int kPIDRegister = 0x0C;
static constexpr int kSNHighRegister = 0x0E;
static constexpr int kSNLowRegister = 0x10;

/**
 * Initialize the gyro.
 *
 * Calibrate the gyro by running for a number of samples and computing the
 * center value. Then use the center value as the Accumulator center value for
 * subsequent measurements.
 *
 * It's important to make sure that the robot is not moving while the centering
 * calculations are in progress, this is typically done when the robot is first
 * turned on while it's sitting at rest before the competition starts.
 */
void ADXRS450_Gyro::Calibrate() {
  Wait(0.1);

  m_spi.SetAccumulatorCenter(0);
  m_spi.ResetAccumulator();

  Wait(kCalibrationSampleTime);

  m_spi.SetAccumulatorCenter(static_cast<int>(m_spi.GetAccumulatorAverage()));
  m_spi.ResetAccumulator();
}

/**
 * Gyro constructor on onboard CS0.
 */
ADXRS450_Gyro::ADXRS450_Gyro() : ADXRS450_Gyro(SPI::kOnboardCS0) {}

/**
 * Gyro constructor on the specified SPI port.
 *
 * @param port The SPI port the gyro is attached to.
 */
ADXRS450_Gyro::ADXRS450_Gyro(SPI::Port port) : m_spi(port) {
  m_spi.SetClockRate(3000000);
  m_spi.SetMSBFirst();
  m_spi.SetSampleDataOnRising();
  m_spi.SetClockActiveHigh();
  m_spi.SetChipSelectActiveLow();

  // Validate the part ID
  if ((ReadRegister(kPIDRegister) & 0xff00) != 0x5200) {
    DriverStation::ReportError("could not find ADXRS450 gyro");
    return;
  }

  m_spi.InitAccumulator(kSamplePeriod, 0x20000000u, 4, 0x0c00000eu, 0x04000000u,
                        10u, 16u, true, true);

  Calibrate();

  HAL_Report(HALUsageReporting::kResourceType_ADXRS450, port);
  LiveWindow::GetInstance()->AddSensor("ADXRS450_Gyro", port, this);
}

static bool CalcParity(int v) {
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
  int cmd = 0x80000000 | static_cast<int>(reg) << 17;
  if (!CalcParity(cmd)) cmd |= 1u;

  // big endian
  uint8_t buf[4] = {static_cast<uint8_t>((cmd >> 24) & 0xff),
                    static_cast<uint8_t>((cmd >> 16) & 0xff),
                    static_cast<uint8_t>((cmd >> 8) & 0xff),
                    static_cast<uint8_t>(cmd & 0xff)};

  m_spi.Write(buf, 4);
  m_spi.Read(false, buf, 4);
  if ((buf[0] & 0xe0) == 0) return 0;  // error, return 0
  return static_cast<uint16_t>((BytesToIntBE(buf) >> 5) & 0xffff);
}

/**
 * Reset the gyro.
 *
 * Resets the gyro to a heading of zero. This can be used if there is
 * significant drift in the gyro and it needs to be recalibrated after it has
 * been running.
 */
void ADXRS450_Gyro::Reset() { m_spi.ResetAccumulator(); }

/**
 * Return the actual angle in degrees that the robot is currently facing.
 *
 * The angle is based on the current accumulator value corrected by the
 * oversampling rate, the gyro type and the A/D calibration values.
 * The angle is continuous, that is it will continue from 360->361 degrees. This
 * allows algorithms that wouldn't want to see a discontinuity in the gyro
 * output as it sweeps from 360 to 0 on the second time around.
 *
 * @return the current heading of the robot in degrees. This heading is based on
 *         integration of the returned rate from the gyro.
 */
double ADXRS450_Gyro::GetAngle() const {
  return m_spi.GetAccumulatorValue() * kDegreePerSecondPerLSB * kSamplePeriod;
}

/**
 * Return the rate of rotation of the gyro
 *
 * The rate is based on the most recent reading of the gyro analog value
 *
 * @return the current rate in degrees per second
 */
double ADXRS450_Gyro::GetRate() const {
  return static_cast<double>(m_spi.GetAccumulatorLastValue()) *
         kDegreePerSecondPerLSB;
}
