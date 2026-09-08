// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/drivers/range/RevColorSensorV2.hpp"

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <limits>
#include <stdexcept>
#include <unordered_map>
#include <utility>
#include <vector>

#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>

#include "wpi/hal/simulation/I2CData.h"

namespace {

using DeviceStatus = wpi::RevColorSensorV2::DeviceStatus;
using FailureReason = wpi::RevColorSensorV2::FailureReason;
using Gain = wpi::RevColorSensorV2::Gain;
using LedDrive = wpi::RevColorSensorV2::LedDrive;
using Register = wpi::RevColorSensorV2::Register;

/// Command bit plus the auto-increment command type. The sensor advances
/// through consecutive registers during a multi-byte read only when the command
/// type is auto-increment.
constexpr int COMMAND_AUTO_INCREMENT = 0x80 | (0x01 << 5);

constexpr uint8_t TMD37821_DEVICE_ID = 0x60;

constexpr int STATUS_COLOR_VALID = 0x01;
constexpr int STATUS_PROXIMITY_VALID = 0x02;
constexpr int STATUS_ALL_VALID = STATUS_COLOR_VALID | STATUS_PROXIMITY_VALID;

/// Default integration time of 24 ms, expressed as ten 2.4 ms cycles.
constexpr int DEFAULT_ATIME = 246;

constexpr int DEFAULT_MAXIMUM_RAW_COLOR_VALUE = 10240;

void PutUnsignedShort(std::vector<uint8_t>& data, std::size_t offset,
                      int value) {
  data[offset] = static_cast<uint8_t>(value);
  data[offset + 1] = static_cast<uint8_t>(value >> 8);
}

std::vector<uint8_t> StatusBlock(int status, int clear, int red, int green,
                                 int blue, int proximity) {
  std::vector<uint8_t> data(11, 0);
  data[0] = static_cast<uint8_t>(status);
  PutUnsignedShort(data, 1, clear);
  PutUnsignedShort(data, 3, red);
  PutUnsignedShort(data, 5, green);
  PutUnsignedShort(data, 7, blue);
  PutUnsignedShort(data, 9, proximity);
  return data;
}

class ColorSensorTestFixture {
 public:
  ColorSensorTestFixture() {
    HALSIM_ResetI2CData(0);
    m_readUid = HALSIM_RegisterI2CReadCallback(0, ReadCallback, this);
    m_writeUid = HALSIM_RegisterI2CWriteCallback(0, WriteCallback, this);
    SetRegister(Register::DEVICE_ID, {TMD37821_DEVICE_ID});
  }

  ~ColorSensorTestFixture() {
    HALSIM_CancelI2CReadCallback(0, m_readUid);
    HALSIM_CancelI2CWriteCallback(0, m_writeUid);
    HALSIM_ResetI2CData(0);
  }

  void SetRegister(Register reg, std::vector<uint8_t> data) {
    m_registerData[static_cast<int>(reg) | COMMAND_AUTO_INCREMENT] =
        std::move(data);
  }

  /// Returns the most recent two-byte write to the given register.
  std::vector<uint8_t> LastWriteTo(Register reg) const {
    auto address =
        static_cast<uint8_t>(static_cast<int>(reg) | COMMAND_AUTO_INCREMENT);
    for (auto it = m_writes.rbegin(); it != m_writes.rend(); ++it) {
      if (it->size() > 1 && (*it)[0] == address) {
        return *it;
      }
    }
    FAIL("No write to the requested register");
    return {};
  }

  std::unordered_map<int, std::vector<uint8_t>> m_registerData;
  std::vector<std::vector<uint8_t>> m_writes;
  std::vector<int> m_readRegisters;
  std::vector<unsigned int> m_readCounts;
  int m_selectedRegister = 0;

 private:
  static void ReadCallback(const char*, void* param, unsigned char* buffer,
                           unsigned int count) {
    auto& self = *static_cast<ColorSensorTestFixture*>(param);
    self.m_readRegisters.push_back(self.m_selectedRegister);
    self.m_readCounts.push_back(count);
    std::fill_n(buffer, count, 0);
    auto it = self.m_registerData.find(self.m_selectedRegister);
    if (it != self.m_registerData.end()) {
      std::copy_n(it->second.begin(),
                  std::min<std::size_t>(count, it->second.size()), buffer);
    }
  }

  static void WriteCallback(const char*, void* param,
                            const unsigned char* buffer, unsigned int count) {
    auto& self = *static_cast<ColorSensorTestFixture*>(param);
    if (count == 0) {
      return;
    }
    self.m_selectedRegister = buffer[0];
    if (count > 1) {
      self.m_writes.emplace_back(buffer, buffer + count);
    }
  }

  int32_t m_readUid;
  int32_t m_writeUid;
};

std::vector<uint8_t> RegisterWrite(Register reg, int value) {
  return {static_cast<uint8_t>(static_cast<int>(reg) | COMMAND_AUTO_INCREMENT),
          static_cast<uint8_t>(value)};
}

TEST_CASE_METHOD(ColorSensorTestFixture,
                 "RevColorSensorV2 configures the sensor on construction",
                 "[drivers][rev-color-sensor-v2]") {
  SetRegister(Register::CONTROL, {0x00});

  wpi::RevColorSensorV2 sensor{wpi::I2C::Port::PORT_0};

  CHECK(sensor.GetPort() == wpi::I2C::Port::PORT_0);
  CHECK(sensor.GetDeviceAddress() == wpi::RevColorSensorV2::DEFAULT_ADDRESS);
  CHECK(sensor.GetDeviceStatus() == DeviceStatus::READY);
  CHECK(sensor.GetDeviceId() == TMD37821_DEVICE_ID);
  CHECK(sensor.GetFailureCount() == 0);
  CHECK_FALSE(sensor.GetLastFailureReason().has_value());

  // The integrator is turned off, the gain and timing are written, and the
  // color and proximity channels are enabled after the power on warm-up.
  REQUIRE(m_writes.size() == 6);
  CHECK(m_writes[0] == RegisterWrite(Register::ENABLE, 0x00));
  CHECK(m_writes[1] == RegisterWrite(Register::ATIME, DEFAULT_ATIME));
  CHECK(m_writes[2] == RegisterWrite(Register::CONTROL, 0x61));
  CHECK(m_writes[3] == RegisterWrite(Register::PPULSE, 8));
  CHECK(m_writes[4] == RegisterWrite(Register::ENABLE, 0x01));
  CHECK(m_writes[5] == RegisterWrite(Register::ENABLE, 0x07));
}

TEST_CASE_METHOD(ColorSensorTestFixture,
                 "RevColorSensorV2 overwrites retained control fields",
                 "[drivers][rev-color-sensor-v2]") {
  // A retained proximity diode bit and proximity gain, as the sensor holds them
  // across a program restart. Disabling the sensor does not reset this
  // register, and preserving these bits would select a reserved diode and
  // proximity gain instead of the calibrated ones.
  SetRegister(Register::CONTROL, {0x1C});

  wpi::RevColorSensorV2 sensor{wpi::I2C::Port::PORT_0};

  // Bits 7:6 LED drive (50%), 5:4 IR diode, 3:2 proximity gain 1x, 1:0 color
  // gain (4x).
  CHECK(LastWriteTo(Register::CONTROL) ==
        RegisterWrite(Register::CONTROL, 0x61));

  sensor.SetGain(Gain::GAIN_60);
  sensor.SetLedDrive(LedDrive::PERCENT_12_5);

  // The diode and proximity gain fields stay at their calibrated values.
  CHECK(LastWriteTo(Register::CONTROL) ==
        RegisterWrite(Register::CONTROL, 0xE3));
  CHECK(sensor.GetGain() == Gain::GAIN_60);
  CHECK(sensor.GetLedDrive() == LedDrive::PERCENT_12_5);
}

TEST_CASE_METHOD(ColorSensorTestFixture,
                 "RevColorSensorV2 rejects an unexpected device ID",
                 "[drivers][rev-color-sensor-v2]") {
  SetRegister(Register::DEVICE_ID, {0x44});

  wpi::RevColorSensorV2 sensor{wpi::I2C::Port::PORT_0};

  CHECK(sensor.GetDeviceStatus() == DeviceStatus::FAULT_UNEXPECTED_DEVICE_ID);
  REQUIRE(sensor.GetLastFailureReason().has_value());
  CHECK(*sensor.GetLastFailureReason() == FailureReason::UNEXPECTED_DEVICE_ID);
  CHECK(sensor.GetDeviceId() == 0x44);
  CHECK(m_writes.empty());
}

TEST_CASE_METHOD(ColorSensorTestFixture,
                 "RevColorSensorV2 retries configuration on update",
                 "[drivers][rev-color-sensor-v2]") {
  SetRegister(Register::DEVICE_ID, {0x00});

  wpi::RevColorSensorV2 sensor{wpi::I2C::Port::PORT_0};
  CHECK(sensor.GetDeviceStatus() == DeviceStatus::FAULT_UNEXPECTED_DEVICE_ID);

  SetRegister(Register::DEVICE_ID, {TMD37821_DEVICE_ID});
  SetRegister(Register::STATUS, StatusBlock(STATUS_ALL_VALID, 1, 2, 3, 4, 5));
  sensor.Update();

  CHECK(sensor.GetDeviceStatus() == DeviceStatus::READY);
  CHECK(sensor.GetRawClear() == 1);
  CHECK(sensor.GetRawProximity() == 5);
}

TEST_CASE_METHOD(ColorSensorTestFixture,
                 "RevColorSensorV2 decodes color and proximity",
                 "[drivers][rev-color-sensor-v2]") {
  SetRegister(Register::STATUS,
              StatusBlock(STATUS_ALL_VALID, 10240, 5120, 2560, 1024, 512));

  wpi::RevColorSensorV2 sensor{wpi::I2C::Port::PORT_0};
  sensor.Update();

  CHECK(sensor.GetDeviceStatus() == DeviceStatus::READY);
  CHECK(sensor.GetMaximumRawColorValue() == DEFAULT_MAXIMUM_RAW_COLOR_VALUE);

  CHECK(sensor.GetRawClear() == 10240);
  CHECK(sensor.GetRawRed() == 5120);
  CHECK(sensor.GetRawGreen() == 2560);
  CHECK(sensor.GetRawBlue() == 1024);
  CHECK(sensor.GetRawProximity() == 512);

  CHECK(sensor.GetClear() == Catch::Approx(1.0));
  CHECK(sensor.GetRed() == Catch::Approx(0.5));
  CHECK(sensor.GetGreen() == Catch::Approx(0.25));
  CHECK(sensor.GetBlue() == Catch::Approx(0.1));

  // Color rounds to 12 bits of precision.
  CHECK(sensor.GetColor().red == Catch::Approx(0.5).margin(1e-3));
  CHECK(sensor.GetColor().green == Catch::Approx(0.25).margin(1e-3));
  CHECK(sensor.GetColor().blue == Catch::Approx(0.1).margin(1e-3));

  CHECK(sensor.GetProximity() == Catch::Approx(512.0 / 1023.0));
}

TEST_CASE_METHOD(
    ColorSensorTestFixture,
    "RevColorSensorV2 addresses the bulk read with the auto-increment type",
    "[drivers][rev-color-sensor-v2]") {
  SetRegister(Register::STATUS, StatusBlock(STATUS_ALL_VALID, 1, 2, 3, 4, 5));

  wpi::RevColorSensorV2 sensor{wpi::I2C::Port::PORT_0};
  m_readRegisters.clear();
  m_readCounts.clear();
  sensor.Update();

  // Without the auto-increment command type the sensor would return the STATUS
  // register once per byte instead of advancing through the color and
  // proximity registers.
  REQUIRE(m_readRegisters.size() == 1);
  CHECK(m_readRegisters[0] == 0xB3);
  CHECK(m_readRegisters[0] ==
        (static_cast<int>(Register::STATUS) | 0x80 | 0x20));
  CHECK(m_readCounts[0] == 11);
}

TEST_CASE_METHOD(ColorSensorTestFixture,
                 "RevColorSensorV2 clamps normalized channels",
                 "[drivers][rev-color-sensor-v2]") {
  SetRegister(Register::STATUS,
              StatusBlock(STATUS_ALL_VALID, 0, 4096, 1024, 0, 2000));

  wpi::RevColorSensorV2 sensor{wpi::I2C::Port::PORT_0};
  sensor.SetSoftwareGain(4.0);
  sensor.Update();

  CHECK(sensor.GetSoftwareGain() == Catch::Approx(4.0));
  CHECK(sensor.GetRed() == Catch::Approx(1.0));
  CHECK(sensor.GetGreen() == Catch::Approx(0.4));
  CHECK(sensor.GetRawRed() == 4096);

  // The proximity channel saturates at its raw maximum.
  CHECK(sensor.GetProximity() == Catch::Approx(1.0));
}

TEST_CASE_METHOD(ColorSensorTestFixture,
                 "RevColorSensorV2 converts proximity to distance",
                 "[drivers][rev-color-sensor-v2]") {
  // A raw reading of a + b * (cm + c)^-2 must convert back to cm.
  int rawOptical = static_cast<int>(
      std::lround(186.347 + 30403.5 * std::pow(5.0 + 0.576649, -2.0)));
  SetRegister(Register::STATUS,
              StatusBlock(STATUS_ALL_VALID, 0, 0, 0, 0, rawOptical));

  wpi::RevColorSensorV2 sensor{wpi::I2C::Port::PORT_0};
  sensor.Update();

  CHECK(sensor.GetDistance().value() == Catch::Approx(0.05).margin(1e-3));
}

TEST_CASE_METHOD(ColorSensorTestFixture,
                 "RevColorSensorV2 reports an out of range distance as NaN",
                 "[drivers][rev-color-sensor-v2]") {
  SetRegister(Register::STATUS, StatusBlock(STATUS_ALL_VALID, 0, 0, 0, 0, 100));

  wpi::RevColorSensorV2 sensor{wpi::I2C::Port::PORT_0};
  sensor.Update();

  CHECK(std::isnan(sensor.GetDistance().value()));
}

TEST_CASE_METHOD(ColorSensorTestFixture,
                 "RevColorSensorV2 applies a custom distance calibration",
                 "[drivers][rev-color-sensor-v2]") {
  SetRegister(Register::STATUS, StatusBlock(STATUS_ALL_VALID, 0, 0, 0, 0, 300));

  wpi::RevColorSensorV2 sensor{wpi::I2C::Port::PORT_0};
  sensor.Update();
  auto defaultDistance = sensor.GetDistance();

  sensor.SetDistanceCalibration(186.347, 2 * 30403.5, 0.576649);

  // Doubling the b parameter means the same reading is produced by a more
  // distant target.
  CHECK(sensor.GetDistance() > defaultDistance);
}

TEST_CASE_METHOD(ColorSensorTestFixture,
                 "RevColorSensorV2 preserves measurements when data is invalid",
                 "[drivers][rev-color-sensor-v2]") {
  SetRegister(Register::STATUS,
              StatusBlock(STATUS_ALL_VALID, 100, 200, 300, 400, 500));

  wpi::RevColorSensorV2 sensor{wpi::I2C::Port::PORT_0};
  sensor.Update();

  SetRegister(Register::STATUS, StatusBlock(0x00, 1, 2, 3, 4, 5));
  sensor.Update();

  CHECK(sensor.GetDeviceStatus() == DeviceStatus::FAULT_BAD_READ);
  REQUIRE(sensor.GetLastFailureReason().has_value());
  CHECK(*sensor.GetLastFailureReason() ==
        FailureReason::PROXIMITY_DATA_NOT_VALID);
  CHECK(sensor.GetFailureCount() == 2);
  CHECK(sensor.GetRawClear() == 100);
  CHECK(sensor.GetRawProximity() == 500);

  SetRegister(Register::STATUS, StatusBlock(STATUS_ALL_VALID, 7, 8, 9, 10, 11));
  sensor.Update();

  CHECK(sensor.GetDeviceStatus() == DeviceStatus::READY);
  CHECK(sensor.GetRawClear() == 7);
  CHECK(sensor.GetRawProximity() == 11);
}

TEST_CASE_METHOD(
    ColorSensorTestFixture,
    "RevColorSensorV2 updates proximity when only color is invalid",
    "[drivers][rev-color-sensor-v2]") {
  SetRegister(Register::STATUS,
              StatusBlock(STATUS_ALL_VALID, 100, 200, 300, 400, 500));

  wpi::RevColorSensorV2 sensor{wpi::I2C::Port::PORT_0};
  sensor.Update();

  SetRegister(Register::STATUS,
              StatusBlock(STATUS_PROXIMITY_VALID, 1, 2, 3, 4, 900));
  sensor.Update();

  REQUIRE(sensor.GetLastFailureReason().has_value());
  CHECK(*sensor.GetLastFailureReason() == FailureReason::COLOR_DATA_NOT_VALID);
  CHECK(sensor.GetFailureCount() == 1);
  CHECK(sensor.GetRawClear() == 100);
  CHECK(sensor.GetRawProximity() == 900);
}

TEST_CASE_METHOD(ColorSensorTestFixture,
                 "RevColorSensorV2 rounds integration time up to whole cycles",
                 "[drivers][rev-color-sensor-v2]") {
  wpi::RevColorSensorV2 sensor{wpi::I2C::Port::PORT_0};

  sensor.SetIntegrationTime(wpi::units::millisecond_t{100});
  CHECK(sensor.GetIntegrationTime().value() == Catch::Approx(100.8));
  CHECK(sensor.GetMaximumRawColorValue() == 42 * 1024);
  CHECK(LastWriteTo(Register::ATIME) ==
        RegisterWrite(Register::ATIME, 256 - 42));

  // The color channels saturate at 16 bits well before the longest
  // integration time.
  sensor.SetIntegrationTime(wpi::units::second_t{1});
  CHECK(sensor.GetIntegrationTime().value() == Catch::Approx(614.4));
  CHECK(sensor.GetMaximumRawColorValue() == 65535);
  CHECK(LastWriteTo(Register::ATIME) == RegisterWrite(Register::ATIME, 0));

  sensor.SetIntegrationTime(wpi::units::millisecond_t{1});
  CHECK(sensor.GetIntegrationTime().value() == Catch::Approx(2.4));
  CHECK(sensor.GetMaximumRawColorValue() == 1024);
  CHECK(LastWriteTo(Register::ATIME) == RegisterWrite(Register::ATIME, 255));
}

TEST_CASE_METHOD(ColorSensorTestFixture,
                 "RevColorSensorV2 rejects invalid configuration values",
                 "[drivers][rev-color-sensor-v2]") {
  wpi::RevColorSensorV2 sensor{wpi::I2C::Port::PORT_0};

  CHECK_THROWS_AS(sensor.SetIntegrationTime(wpi::units::millisecond_t{0}),
                  std::invalid_argument);
  CHECK_THROWS_AS(sensor.SetIntegrationTime(wpi::units::millisecond_t{-1}),
                  std::invalid_argument);
  CHECK_THROWS_AS(sensor.SetProximityPulseCount(0), std::invalid_argument);
  CHECK_THROWS_AS(sensor.SetProximityPulseCount(256), std::invalid_argument);
  CHECK_THROWS_AS(sensor.SetSoftwareGain(0.0), std::invalid_argument);
  CHECK_THROWS_AS(sensor.SetGain(static_cast<Gain>(9)), std::invalid_argument);
  CHECK_THROWS_AS(sensor.SetLedDrive(static_cast<LedDrive>(9)),
                  std::invalid_argument);
  CHECK_THROWS_AS(sensor.SetDistanceCalibration(
                      std::numeric_limits<double>::infinity(), 1.0, 1.0),
                  std::invalid_argument);

  // The sensor keeps its defaults after every rejected value.
  CHECK(sensor.GetGain() == Gain::GAIN_4);
  CHECK(sensor.GetLedDrive() == LedDrive::PERCENT_50);
  CHECK(sensor.GetIntegrationTime().value() == Catch::Approx(24.0));
  CHECK(sensor.GetProximityPulseCount() == 8);
  CHECK(sensor.GetSoftwareGain() == Catch::Approx(1.0));
}

TEST_CASE_METHOD(ColorSensorTestFixture,
                 "RevColorSensorV2 rejects invalid I2C addresses",
                 "[drivers][rev-color-sensor-v2]") {
  CHECK_THROWS_AS(wpi::RevColorSensorV2(wpi::I2C::Port::PORT_0, -1),
                  std::invalid_argument);
  CHECK_THROWS_AS(wpi::RevColorSensorV2(wpi::I2C::Port::PORT_0, 0x80),
                  std::invalid_argument);
}

TEST_CASE_METHOD(
    ColorSensorTestFixture,
    "RevColorSensorV2 stores configuration written before initialization",
    "[drivers][rev-color-sensor-v2]") {
  SetRegister(Register::DEVICE_ID, {0x00});

  wpi::RevColorSensorV2 sensor{wpi::I2C::Port::PORT_0};
  sensor.SetGain(Gain::GAIN_16);
  sensor.SetProximityPulseCount(32);
  CHECK(m_writes.empty());

  SetRegister(Register::DEVICE_ID, {TMD37821_DEVICE_ID});
  SetRegister(Register::STATUS, StatusBlock(STATUS_ALL_VALID, 0, 0, 0, 0, 0));
  sensor.Update();

  CHECK(sensor.GetDeviceStatus() == DeviceStatus::READY);
  CHECK(LastWriteTo(Register::CONTROL) ==
        RegisterWrite(Register::CONTROL, 0x62));
  CHECK(LastWriteTo(Register::PPULSE) == RegisterWrite(Register::PPULSE, 32));
}

}  // namespace
