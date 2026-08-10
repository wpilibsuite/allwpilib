// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/drivers/GoBildaPinpoint.hpp"

#include <algorithm>
#include <bit>
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

using Register = wpi::GoBildaPinpoint::Register;

std::vector<uint8_t> EncodeInt(int32_t value) {
  uint32_t bits = std::bit_cast<uint32_t>(value);
  return {static_cast<uint8_t>(bits), static_cast<uint8_t>(bits >> 8),
          static_cast<uint8_t>(bits >> 16), static_cast<uint8_t>(bits >> 24)};
}

std::vector<uint8_t> EncodeFloat(float value) {
  uint32_t bits = std::bit_cast<uint32_t>(value);
  return {static_cast<uint8_t>(bits), static_cast<uint8_t>(bits >> 8),
          static_cast<uint8_t>(bits >> 16), static_cast<uint8_t>(bits >> 24)};
}

template <typename... Vectors>
std::vector<uint8_t> Concat(const Vectors&... vectors) {
  std::vector<uint8_t> result;
  (result.insert(result.end(), vectors.begin(), vectors.end()), ...);
  return result;
}

std::vector<uint8_t> FixedBulkData(int32_t status, int32_t loopTime,
                                   int32_t xEncoder, int32_t yEncoder,
                                   float xPosition, float yPosition,
                                   float heading, float xVelocity,
                                   float yVelocity, float headingVelocity) {
  return Concat(EncodeInt(status), EncodeInt(loopTime), EncodeInt(xEncoder),
                EncodeInt(yEncoder), EncodeFloat(xPosition),
                EncodeFloat(yPosition), EncodeFloat(heading),
                EncodeFloat(xVelocity), EncodeFloat(yVelocity),
                EncodeFloat(headingVelocity));
}

uint8_t ComputeCrc8(const std::vector<uint8_t>& data) {
  uint8_t crc = 0x90;
  for (uint8_t value : data) {
    crc ^= value;
    for (int bit = 0; bit < 8; ++bit) {
      crc = (crc & 0x80) != 0 ? static_cast<uint8_t>((crc << 1) ^ 0x31)
                              : static_cast<uint8_t>(crc << 1);
    }
  }
  return crc;
}

std::vector<uint8_t> AppendCrc(std::vector<uint8_t> data) {
  data.push_back(ComputeCrc8(data));
  return data;
}

class PinpointTestFixture {
 public:
  PinpointTestFixture() {
    HALSIM_ResetI2CData(0);
    m_readUid = HALSIM_RegisterI2CReadCallback(0, ReadCallback, this);
    m_writeUid = HALSIM_RegisterI2CWriteCallback(0, WriteCallback, this);
  }

  ~PinpointTestFixture() {
    HALSIM_CancelI2CReadCallback(0, m_readUid);
    HALSIM_CancelI2CWriteCallback(0, m_writeUid);
    HALSIM_ResetI2CData(0);
  }

  void SetRegister(Register reg, std::vector<uint8_t> data) {
    m_registerData[static_cast<int>(reg)] = std::move(data);
  }

  std::unordered_map<int, std::vector<uint8_t>> m_registerData;
  std::vector<std::vector<uint8_t>> m_writes;
  int m_selectedRegister = 0;

 private:
  static void ReadCallback(const char*, void* param, unsigned char* buffer,
                           unsigned int count) {
    auto& self = *static_cast<PinpointTestFixture*>(param);
    std::fill_n(buffer, count, 0);
    auto it = self.m_registerData.find(self.m_selectedRegister);
    if (it != self.m_registerData.end()) {
      std::copy_n(it->second.begin(),
                  std::min<std::size_t>(count, it->second.size()), buffer);
    }
  }

  static void WriteCallback(const char*, void* param,
                            const unsigned char* buffer, unsigned int count) {
    auto& self = *static_cast<PinpointTestFixture*>(param);
    if (count == 0) {
      return;
    }
    self.m_selectedRegister = buffer[0];
    self.m_writes.emplace_back(buffer, buffer + count);
  }

  int32_t m_readUid;
  int32_t m_writeUid;
};

TEST_CASE_METHOD(PinpointTestFixture,
                 "GoBildaPinpoint decodes firmware v2 fixed bulk reads",
                 "[drivers][gobilda-pinpoint]") {
  SetRegister(Register::DEVICE_VERSION, EncodeInt(2));
  SetRegister(Register::BULK_READ,
              FixedBulkData(1, 800, 12345, -54321, 1234.5f, -678.25f, 7.25f,
                            2500.0f, -3000.0f, 4.5f));

  wpi::GoBildaPinpoint pinpoint{wpi::I2C::Port::PORT_0};
  pinpoint.Update();

  CHECK(pinpoint.GetPort() == wpi::I2C::Port::PORT_0);
  CHECK(pinpoint.GetDeviceAddress() == wpi::GoBildaPinpoint::DEFAULT_ADDRESS);
  CHECK(pinpoint.GetDeviceStatus() ==
        wpi::GoBildaPinpoint::DeviceStatus::READY);
  CHECK(pinpoint.GetLoopTimeMicroseconds() == 800);
  CHECK(pinpoint.GetFrequency().value() == Catch::Approx(1250.0));
  CHECK(pinpoint.GetXEncoder() == 12345);
  CHECK(pinpoint.GetYEncoder() == -54321);
  CHECK(pinpoint.GetXPosition().value() == Catch::Approx(1.2345));
  CHECK(pinpoint.GetYPosition().value() == Catch::Approx(-0.67825));
  CHECK(pinpoint.GetHeading().value() == Catch::Approx(7.25));
  CHECK(pinpoint.GetXVelocity().value() == Catch::Approx(2.5));
  CHECK(pinpoint.GetYVelocity().value() == Catch::Approx(-3.0));
  CHECK(pinpoint.GetHeadingVelocity().value() == Catch::Approx(4.5));
  CHECK(pinpoint.GetPose().X().value() == Catch::Approx(1.2345));
  REQUIRE(m_writes.size() >= 2);
  CHECK(m_writes[0] ==
        std::vector<uint8_t>{static_cast<uint8_t>(Register::DEVICE_VERSION)});
  CHECK(m_writes[1] ==
        std::vector<uint8_t>{static_cast<uint8_t>(Register::BULK_READ)});
}

TEST_CASE_METHOD(PinpointTestFixture,
                 "GoBildaPinpoint configures flexible bulk reads",
                 "[drivers][gobilda-pinpoint]") {
  SetRegister(Register::DEVICE_VERSION, EncodeInt(3));
  wpi::GoBildaPinpoint pinpoint{wpi::I2C::Port::PORT_0};

  pinpoint.SetBulkReadScope(
      {Register::X_POSITION, Register::H_ORIENTATION, Register::X_POSITION});

  REQUIRE(m_writes.size() >= 2);
  CHECK(m_writes.back() ==
        std::vector<uint8_t>{static_cast<uint8_t>(Register::SET_BULK_READ),
                             static_cast<uint8_t>(Register::X_POSITION),
                             static_cast<uint8_t>(Register::H_ORIENTATION)});

  SetRegister(Register::BULK_READ,
              Concat(EncodeFloat(8500.0f), EncodeFloat(12.5f)));
  pinpoint.Update();
  CHECK(pinpoint.GetXPosition().value() == Catch::Approx(8.5));
  CHECK(pinpoint.GetHeading().value() == Catch::Approx(12.5));
}

TEST_CASE_METHOD(PinpointTestFixture,
                 "GoBildaPinpoint validates CRC and preserves measurements",
                 "[drivers][gobilda-pinpoint]") {
  SetRegister(Register::DEVICE_VERSION, EncodeInt(3));
  SetRegister(Register::BULK_READ,
              AppendCrc(FixedBulkData(1, 1000, 1, 2, 1500.0f, -2500.0f, 0.75f,
                                      500.0f, -750.0f, 0.25f)));

  wpi::GoBildaPinpoint pinpoint{wpi::I2C::Port::PORT_0};
  pinpoint.SetErrorDetectionType(wpi::GoBildaPinpoint::ErrorDetectionType::CRC);
  pinpoint.Update();
  CHECK(pinpoint.GetXPosition().value() == Catch::Approx(1.5));

  auto corrupt = AppendCrc(
      FixedBulkData(1, 1000, 1, 2, 4200.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f));
  corrupt.back() ^= 1;
  SetRegister(Register::BULK_READ, std::move(corrupt));
  pinpoint.Update();

  CHECK(pinpoint.GetXPosition().value() == Catch::Approx(1.5));
  CHECK(pinpoint.GetDeviceStatus() ==
        wpi::GoBildaPinpoint::DeviceStatus::FAULT_BAD_READ);
  CHECK(pinpoint.GetLastFailedRegister() == Register::BULK_READ);
  CHECK(pinpoint.GetLastFailureReason() ==
        wpi::GoBildaPinpoint::FailureReason::CRC_MISMATCH);
  CHECK(pinpoint.GetFailureCount() == 1);
  CHECK(pinpoint.GetFailureCount(Register::BULK_READ) == 1);
}

TEST_CASE_METHOD(PinpointTestFixture,
                 "GoBildaPinpoint local validation rejects implausible jumps",
                 "[drivers][gobilda-pinpoint]") {
  SetRegister(Register::DEVICE_VERSION, EncodeInt(2));
  SetRegister(Register::BULK_READ, FixedBulkData(1, 1000, 0, 0, 1000.0f, 0.0f,
                                                 0.0f, 0.0f, 0.0f, 0.0f));
  wpi::GoBildaPinpoint pinpoint{wpi::I2C::Port::PORT_0};
  pinpoint.Update();

  SetRegister(Register::BULK_READ, FixedBulkData(1, 1000, 0, 0, 7000.0f, 0.0f,
                                                 0.0f, 0.0f, 0.0f, 0.0f));
  pinpoint.Update();

  CHECK(pinpoint.GetXPosition().value() == Catch::Approx(1.0));
  CHECK(pinpoint.GetDeviceStatus() ==
        wpi::GoBildaPinpoint::DeviceStatus::FAULT_BAD_READ);
  CHECK(pinpoint.GetLastFailedRegister() == Register::X_POSITION);
  CHECK(pinpoint.GetLastFailureReason() ==
        wpi::GoBildaPinpoint::FailureReason::CHANGE_TOO_LARGE);
}

TEST_CASE_METHOD(PinpointTestFixture,
                 "GoBildaPinpoint rejects invalid configuration",
                 "[drivers][gobilda-pinpoint]") {
  CHECK_THROWS_AS(wpi::GoBildaPinpoint(wpi::I2C::Port::PORT_0, -1),
                  std::invalid_argument);
  CHECK_THROWS_AS(wpi::GoBildaPinpoint(wpi::I2C::Port::PORT_0, 0x80),
                  std::invalid_argument);

  wpi::GoBildaPinpoint pinpoint{wpi::I2C::Port::PORT_0};
  CHECK_THROWS_AS(pinpoint.SetEncoderResolution(0.0), std::invalid_argument);
  CHECK_THROWS_AS(
      pinpoint.SetYawScalar(std::numeric_limits<double>::infinity()),
      std::invalid_argument);
}

}  // namespace
