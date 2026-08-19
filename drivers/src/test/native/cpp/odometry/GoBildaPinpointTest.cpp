// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/drivers/odometry/GoBildaPinpoint.hpp"

#include <algorithm>
#include <array>
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
  std::vector<int> m_readRegisters;
  std::vector<unsigned int> m_readCounts;
  int m_selectedRegister = 0;

 private:
  static void ReadCallback(const char*, void* param, unsigned char* buffer,
                           unsigned int count) {
    auto& self = *static_cast<PinpointTestFixture*>(param);
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

TEST_CASE_METHOD(
    PinpointTestFixture,
    "GoBildaPinpoint bulk reads cannot overwrite the detected device version",
    "[drivers][gobilda-pinpoint]") {
  using ErrorDetectionType = wpi::GoBildaPinpoint::ErrorDetectionType;

  for (ErrorDetectionType errorDetectionType :
       std::array{ErrorDetectionType::NONE, ErrorDetectionType::LOCAL_TEST}) {
    SetRegister(Register::DEVICE_VERSION, EncodeInt(3));
    wpi::GoBildaPinpoint pinpoint{wpi::I2C::Port::PORT_0};
    pinpoint.SetErrorDetectionType(errorDetectionType);
    pinpoint.SetBulkReadScope({Register::DEVICE_VERSION, Register::X_POSITION});
    std::size_t readCount = m_readCounts.size();

    SetRegister(Register::BULK_READ,
                Concat(EncodeInt(2), EncodeFloat(1000.0f)));
    pinpoint.Update();

    CHECK(pinpoint.GetDeviceVersion() == 3);
    CHECK(pinpoint.GetXPosition().value() == Catch::Approx(1.0));

    SetRegister(Register::BULK_READ,
                Concat(EncodeInt(1), EncodeFloat(2000.0f)));
    pinpoint.Update();

    CHECK(pinpoint.GetDeviceVersion() == 3);
    CHECK(pinpoint.GetXPosition().value() == Catch::Approx(2.0));
    REQUIRE(m_readCounts.size() == readCount + 2);
    CHECK(m_readCounts[readCount] == 8);
    CHECK(m_readCounts[readCount + 1] == 8);
    CHECK(m_readRegisters[readCount] == static_cast<int>(Register::BULK_READ));
    CHECK(m_readRegisters[readCount + 1] ==
          static_cast<int>(Register::BULK_READ));
  }
}

TEST_CASE_METHOD(
    PinpointTestFixture,
    "GoBildaPinpoint getter cannot overwrite the detected protocol version",
    "[drivers][gobilda-pinpoint]") {
  using ErrorDetectionType = wpi::GoBildaPinpoint::ErrorDetectionType;

  for (ErrorDetectionType errorDetectionType :
       std::array{ErrorDetectionType::NONE, ErrorDetectionType::LOCAL_TEST}) {
    SetRegister(Register::DEVICE_VERSION, EncodeInt(3));
    SetRegister(Register::BULK_READ, FixedBulkData(1, 1000, 0, 0, 1000.0f, 0.0f,
                                                   0.0f, 0.0f, 0.0f, 0.0f));
    wpi::GoBildaPinpoint pinpoint{wpi::I2C::Port::PORT_0};
    pinpoint.SetErrorDetectionType(errorDetectionType);
    pinpoint.Update();

    std::size_t readCount = m_readCounts.size();
    SetRegister(Register::DEVICE_VERSION, EncodeInt(2));
    CHECK(pinpoint.GetDeviceVersion() == 3);
    CHECK(m_readCounts.size() == readCount);

    SetRegister(Register::BULK_READ, FixedBulkData(1, 1000, 0, 0, 2000.0f, 0.0f,
                                                   0.0f, 0.0f, 0.0f, 0.0f));
    pinpoint.Update();
    CHECK(pinpoint.GetXPosition().value() == Catch::Approx(2.0));
    CHECK(m_readCounts[readCount] == 40);

    readCount = m_readCounts.size();
    SetRegister(Register::DEVICE_VERSION, EncodeInt(-1));
    CHECK(pinpoint.GetDeviceVersion() == 3);
    CHECK(m_readCounts.size() == readCount);

    SetRegister(Register::BULK_READ, FixedBulkData(1, 1000, 0, 0, 3000.0f, 0.0f,
                                                   0.0f, 0.0f, 0.0f, 0.0f));
    pinpoint.Update();
    CHECK(pinpoint.GetXPosition().value() == Catch::Approx(3.0));
    CHECK(m_readCounts[readCount] == 40);
  }
}

TEST_CASE_METHOD(
    PinpointTestFixture,
    "GoBildaPinpoint synchronizes the default scope for a new v3 instance",
    "[drivers][gobilda-pinpoint]") {
  SetRegister(Register::DEVICE_VERSION, EncodeInt(3));
  SetRegister(Register::BULK_READ,
              FixedBulkData(1, 1000, 17, 29, -2400.0f, 3600.0f, -2.5f, -1250.0f,
                            875.0f, -1.25f));

  {
    wpi::GoBildaPinpoint previous{wpi::I2C::Port::PORT_0};
    previous.SetBulkReadScope({Register::X_POSITION, Register::H_ORIENTATION});
  }
  m_writes.clear();

  wpi::GoBildaPinpoint pinpoint{wpi::I2C::Port::PORT_0};
  pinpoint.Update();

  REQUIRE(m_writes.size() == 3);
  CHECK(m_writes[0] ==
        std::vector<uint8_t>{static_cast<uint8_t>(Register::DEVICE_VERSION)});
  CHECK(m_writes[1] ==
        std::vector<uint8_t>{static_cast<uint8_t>(Register::SET_BULK_READ),
                             static_cast<uint8_t>(Register::DEVICE_STATUS),
                             static_cast<uint8_t>(Register::LOOP_TIME),
                             static_cast<uint8_t>(Register::X_ENCODER_VALUE),
                             static_cast<uint8_t>(Register::Y_ENCODER_VALUE),
                             static_cast<uint8_t>(Register::X_POSITION),
                             static_cast<uint8_t>(Register::Y_POSITION),
                             static_cast<uint8_t>(Register::H_ORIENTATION),
                             static_cast<uint8_t>(Register::X_VELOCITY),
                             static_cast<uint8_t>(Register::Y_VELOCITY),
                             static_cast<uint8_t>(Register::H_VELOCITY)});
  CHECK(m_writes[2] ==
        std::vector<uint8_t>{static_cast<uint8_t>(Register::BULK_READ)});
  CHECK(pinpoint.GetXPosition().value() == Catch::Approx(-2.4));
  CHECK(pinpoint.GetYPosition().value() == Catch::Approx(3.6));
}

TEST_CASE_METHOD(PinpointTestFixture,
                 "GoBildaPinpoint reads a partial pose scope as one snapshot",
                 "[drivers][gobilda-pinpoint]") {
  SetRegister(Register::DEVICE_VERSION, EncodeInt(3));
  wpi::GoBildaPinpoint pinpoint{wpi::I2C::Port::PORT_0};
  pinpoint.SetBulkReadScope({Register::X_POSITION, Register::H_ORIENTATION});
  SetRegister(Register::BULK_READ,
              Concat(EncodeFloat(1000.0f), EncodeFloat(0.1f)));
  pinpoint.Update();

  SetRegister(
      Register::BULK_READ,
      Concat(EncodeFloat(2000.0f), EncodeFloat(3000.0f), EncodeFloat(0.2f)));
  std::size_t readCount = m_readCounts.size();
  std::size_t writeCount = m_writes.size();

  auto pose = pinpoint.GetPose();

  CHECK(pose.X().value() == Catch::Approx(2.0));
  CHECK(pose.Y().value() == Catch::Approx(3.0));
  CHECK(pose.Rotation().Radians().value() == Catch::Approx(0.2));
  REQUIRE(m_readCounts.size() == readCount + 1);
  CHECK(m_readRegisters[readCount] == static_cast<int>(Register::BULK_READ));
  CHECK(m_readCounts[readCount] == 12);
  REQUIRE(m_writes.size() == writeCount + 3);
  CHECK(m_writes[writeCount] ==
        std::vector<uint8_t>{static_cast<uint8_t>(Register::SET_BULK_READ),
                             static_cast<uint8_t>(Register::X_POSITION),
                             static_cast<uint8_t>(Register::Y_POSITION),
                             static_cast<uint8_t>(Register::H_ORIENTATION)});
  CHECK(m_writes[writeCount + 1] ==
        std::vector<uint8_t>{static_cast<uint8_t>(Register::BULK_READ)});
  CHECK(m_writes[writeCount + 2] ==
        std::vector<uint8_t>{static_cast<uint8_t>(Register::SET_BULK_READ),
                             static_cast<uint8_t>(Register::X_POSITION),
                             static_cast<uint8_t>(Register::H_ORIENTATION)});

  SetRegister(Register::BULK_READ,
              Concat(EncodeFloat(2500.0f),
                     EncodeFloat(std::numeric_limits<float>::quiet_NaN()),
                     EncodeFloat(0.3f)));
  pose = pinpoint.GetPose();

  CHECK(pose.X().value() == Catch::Approx(2.0));
  CHECK(pose.Y().value() == Catch::Approx(3.0));
  CHECK(pose.Rotation().Radians().value() == Catch::Approx(0.2));
  CHECK(pinpoint.GetLastFailedRegister() == Register::Y_POSITION);
  CHECK(pinpoint.GetLastFailureReason() ==
        wpi::GoBildaPinpoint::FailureReason::NONFINITE_VALUE);
}

TEST_CASE_METHOD(
    PinpointTestFixture,
    "GoBildaPinpoint reads an omitted pose scope as one CRC protected snapshot",
    "[drivers][gobilda-pinpoint]") {
  SetRegister(Register::DEVICE_VERSION, EncodeInt(3));
  wpi::GoBildaPinpoint pinpoint{wpi::I2C::Port::PORT_0};
  pinpoint.SetBulkReadScope({Register::DEVICE_STATUS});
  pinpoint.SetErrorDetectionType(wpi::GoBildaPinpoint::ErrorDetectionType::CRC);
  SetRegister(Register::BULK_READ, AppendCrc(EncodeInt(1)));
  pinpoint.Update();

  SetRegister(Register::BULK_READ,
              AppendCrc(Concat(EncodeFloat(4000.0f), EncodeFloat(-5000.0f),
                               EncodeFloat(1.25f))));
  std::size_t readCount = m_readCounts.size();

  auto pose = pinpoint.GetPose();

  CHECK(pose.X().value() == Catch::Approx(4.0));
  CHECK(pose.Y().value() == Catch::Approx(-5.0));
  CHECK(pose.Rotation().Radians().value() == Catch::Approx(1.25));
  REQUIRE(m_readCounts.size() == readCount + 1);
  CHECK(m_readRegisters[readCount] == static_cast<int>(Register::BULK_READ));
  CHECK(m_readCounts[readCount] == 13);
}

TEST_CASE_METHOD(
    PinpointTestFixture,
    "GoBildaPinpoint reads a partial quaternion scope as one snapshot",
    "[drivers][gobilda-pinpoint]") {
  SetRegister(Register::DEVICE_VERSION, EncodeInt(3));
  wpi::GoBildaPinpoint pinpoint{wpi::I2C::Port::PORT_0};
  pinpoint.SetBulkReadScope({Register::QUATERNION_W, Register::QUATERNION_Z});
  SetRegister(Register::BULK_READ,
              Concat(EncodeFloat(0.1f), EncodeFloat(0.4f)));
  pinpoint.Update();

  SetRegister(Register::BULK_READ,
              Concat(EncodeFloat(0.5f), EncodeFloat(-0.25f),
                     EncodeFloat(0.125f), EncodeFloat(0.75f)));
  std::size_t readCount = m_readCounts.size();

  auto quaternion = pinpoint.GetQuaternion();

  CHECK(quaternion.W() == Catch::Approx(0.5));
  CHECK(quaternion.X() == Catch::Approx(-0.25));
  CHECK(quaternion.Y() == Catch::Approx(0.125));
  CHECK(quaternion.Z() == Catch::Approx(0.75));
  REQUIRE(m_readCounts.size() == readCount + 1);
  CHECK(m_readRegisters[readCount] == static_cast<int>(Register::BULK_READ));
  CHECK(m_readCounts[readCount] == 16);

  SetRegister(Register::BULK_READ,
              Concat(EncodeFloat(0.6f),
                     EncodeFloat(std::numeric_limits<float>::quiet_NaN()),
                     EncodeFloat(0.2f), EncodeFloat(0.7f)));
  quaternion = pinpoint.GetQuaternion();

  CHECK(quaternion.W() == Catch::Approx(0.5));
  CHECK(quaternion.X() == Catch::Approx(-0.25));
  CHECK(quaternion.Y() == Catch::Approx(0.125));
  CHECK(quaternion.Z() == Catch::Approx(0.75));
  CHECK(pinpoint.GetLastFailedRegister() == Register::QUATERNION_X);
  CHECK(pinpoint.GetLastFailureReason() ==
        wpi::GoBildaPinpoint::FailureReason::NONFINITE_VALUE);
}

TEST_CASE_METHOD(PinpointTestFixture,
                 "GoBildaPinpoint uses a complete cached quaternion scope",
                 "[drivers][gobilda-pinpoint]") {
  SetRegister(Register::DEVICE_VERSION, EncodeInt(3));
  wpi::GoBildaPinpoint pinpoint{wpi::I2C::Port::PORT_0};
  pinpoint.SetBulkReadScope({Register::QUATERNION_W, Register::QUATERNION_X,
                             Register::QUATERNION_Y, Register::QUATERNION_Z});
  SetRegister(Register::BULK_READ,
              Concat(EncodeFloat(0.5f), EncodeFloat(-0.25f),
                     EncodeFloat(0.125f), EncodeFloat(0.75f)));
  pinpoint.Update();
  std::size_t readCount = m_readCounts.size();

  auto quaternion = pinpoint.GetQuaternion();

  CHECK(quaternion.W() == Catch::Approx(0.5));
  CHECK(quaternion.X() == Catch::Approx(-0.25));
  CHECK(quaternion.Y() == Catch::Approx(0.125));
  CHECK(quaternion.Z() == Catch::Approx(0.75));
  CHECK(m_readCounts.size() == readCount);

  SetRegister(Register::BULK_READ,
              Concat(EncodeFloat(0.6f),
                     EncodeFloat(std::numeric_limits<float>::quiet_NaN()),
                     EncodeFloat(0.2f), EncodeFloat(0.7f)));
  pinpoint.Update();
  quaternion = pinpoint.GetQuaternion();

  CHECK(quaternion.W() == Catch::Approx(0.5));
  CHECK(quaternion.X() == Catch::Approx(-0.25));
  CHECK(quaternion.Y() == Catch::Approx(0.125));
  CHECK(quaternion.Z() == Catch::Approx(0.75));
  CHECK(pinpoint.GetLastFailedRegister() == Register::QUATERNION_X);
  CHECK(pinpoint.GetLastFailureReason() ==
        wpi::GoBildaPinpoint::FailureReason::NONFINITE_VALUE);
}

TEST_CASE_METHOD(PinpointTestFixture,
                 "GoBildaPinpoint rejects zero and near-zero quaternion norms",
                 "[drivers][gobilda-pinpoint]") {
  SetRegister(Register::DEVICE_VERSION, EncodeInt(3));
  wpi::GoBildaPinpoint pinpoint{wpi::I2C::Port::PORT_0};
  pinpoint.SetBulkReadScope({Register::QUATERNION_W, Register::QUATERNION_X,
                             Register::QUATERNION_Y, Register::QUATERNION_Z});
  SetRegister(Register::BULK_READ,
              Concat(EncodeFloat(0.5f), EncodeFloat(-0.25f),
                     EncodeFloat(0.125f), EncodeFloat(0.75f)));
  pinpoint.Update();

  for (float component : std::array{0.0f, 1e-8f}) {
    SetRegister(Register::BULK_READ,
                Concat(EncodeFloat(component), EncodeFloat(component),
                       EncodeFloat(component), EncodeFloat(component)));
    pinpoint.Update();

    auto quaternion = pinpoint.GetQuaternion();
    CHECK(quaternion.W() == Catch::Approx(0.5));
    CHECK(quaternion.X() == Catch::Approx(-0.25));
    CHECK(quaternion.Y() == Catch::Approx(0.125));
    CHECK(quaternion.Z() == Catch::Approx(0.75));
    CHECK(pinpoint.GetLastFailedRegister() == Register::QUATERNION_W);
    CHECK(pinpoint.GetLastFailureReason() ==
          wpi::GoBildaPinpoint::FailureReason::INVALID_QUATERNION);
  }
  CHECK(pinpoint.GetFailureCount(Register::QUATERNION_W) == 2);
}

TEST_CASE_METHOD(
    PinpointTestFixture,
    "GoBildaPinpoint quaternion snapshot CRC failure preserves cached values",
    "[drivers][gobilda-pinpoint]") {
  SetRegister(Register::DEVICE_VERSION, EncodeInt(3));
  wpi::GoBildaPinpoint pinpoint{wpi::I2C::Port::PORT_0};
  pinpoint.SetBulkReadScope({Register::DEVICE_STATUS});
  pinpoint.SetErrorDetectionType(wpi::GoBildaPinpoint::ErrorDetectionType::CRC);
  SetRegister(Register::BULK_READ, AppendCrc(EncodeInt(1)));
  pinpoint.Update();

  auto data = AppendCrc(Concat(EncodeFloat(0.5f), EncodeFloat(-0.25f),
                               EncodeFloat(0.125f), EncodeFloat(0.75f)));
  data.back() ^= 1;
  SetRegister(Register::BULK_READ, std::move(data));
  std::size_t readCount = m_readCounts.size();

  auto quaternion = pinpoint.GetQuaternion();

  CHECK(quaternion.W() == Catch::Approx(0.0));
  CHECK(quaternion.X() == Catch::Approx(0.0));
  CHECK(quaternion.Y() == Catch::Approx(0.0));
  CHECK(quaternion.Z() == Catch::Approx(0.0));
  REQUIRE(m_readCounts.size() == readCount + 1);
  CHECK(m_readRegisters[readCount] == static_cast<int>(Register::BULK_READ));
  CHECK(m_readCounts[readCount] == 17);
  CHECK(pinpoint.GetDeviceStatus() ==
        wpi::GoBildaPinpoint::DeviceStatus::FAULT_BAD_READ);
  CHECK(pinpoint.GetLastFailedRegister() == Register::BULK_READ);
  CHECK(pinpoint.GetLastFailureReason() ==
        wpi::GoBildaPinpoint::FailureReason::CRC_MISMATCH);
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
  for (int version : std::array{2, 3}) {
    SetRegister(Register::DEVICE_VERSION, EncodeInt(version));
    SetRegister(Register::BULK_READ, FixedBulkData(1, 1000, 0, 0, 1000.0f, 0.0f,
                                                   0.0f, 0.0f, 0.0f, 0.0f));
    wpi::GoBildaPinpoint pinpoint{wpi::I2C::Port::PORT_0};
    pinpoint.Update();

    SetRegister(
        Register::BULK_READ,
        FixedBulkData(1, 1000, 0, 0, 7000.0f, 2000.0f, 1.0f, 0.0f, 0.0f, 0.0f));
    pinpoint.Update();

    CHECK(pinpoint.GetXPosition().value() == Catch::Approx(1.0));
    CHECK(pinpoint.GetYPosition().value() == Catch::Approx(0.0));
    CHECK(pinpoint.GetHeading().value() == Catch::Approx(0.0));
    CHECK(pinpoint.GetDeviceStatus() ==
          wpi::GoBildaPinpoint::DeviceStatus::FAULT_BAD_READ);
    CHECK(pinpoint.GetLastFailedRegister() == Register::X_POSITION);
    CHECK(pinpoint.GetLastFailureReason() ==
          wpi::GoBildaPinpoint::FailureReason::CHANGE_TOO_LARGE);
  }
}

TEST_CASE_METHOD(
    PinpointTestFixture,
    "GoBildaPinpoint reestablishes pose baseline after omitted bulk samples",
    "[drivers][gobilda-pinpoint]") {
  using wpi::units::meters<>;

  SetRegister(Register::DEVICE_VERSION, EncodeInt(3));
  wpi::GoBildaPinpoint pinpoint{wpi::I2C::Port::PORT_0};
  pinpoint.SetBulkReadScope({Register::DEVICE_STATUS});
  SetRegister(Register::BULK_READ, EncodeInt(1));
  pinpoint.Update();

  SetRegister(
      Register::BULK_READ,
      Concat(EncodeFloat(1000.0f), EncodeFloat(2000.0f), EncodeFloat(0.5f)));
  auto pose = pinpoint.GetPose();
  CHECK(pose.X() == meter_t{1.0});
  CHECK(pose.Y() == meter_t{2.0});

  SetRegister(Register::BULK_READ, EncodeInt(1));
  pinpoint.Update();

  SetRegister(
      Register::BULK_READ,
      Concat(EncodeFloat(7000.0f), EncodeFloat(8000.0f), EncodeFloat(1.0f)));
  pose = pinpoint.GetPose();
  CHECK(pose.X() == meter_t{7.0});
  CHECK(pose.Y() == meter_t{8.0});
  CHECK(pose.Rotation().Radians().value() == Catch::Approx(1.0));
  CHECK(pinpoint.GetFailureCount() == 0);

  SetRegister(
      Register::BULK_READ,
      Concat(EncodeFloat(13000.0f), EncodeFloat(8000.0f), EncodeFloat(1.0f)));
  pose = pinpoint.GetPose();
  CHECK(pose.X() == meter_t{7.0});
  CHECK(pose.Y() == meter_t{8.0});
  CHECK(pose.Rotation().Radians().value() == Catch::Approx(1.0));
  CHECK(pinpoint.GetLastFailureReason() ==
        wpi::GoBildaPinpoint::FailureReason::CHANGE_TOO_LARGE);
}

TEST_CASE_METHOD(PinpointTestFixture,
                 "GoBildaPinpoint pose writes reset local validation baselines",
                 "[drivers][gobilda-pinpoint]") {
  using wpi::units::meters<>;
  using wpi::units::radians<>;

  SetRegister(Register::DEVICE_VERSION, EncodeInt(2));
  SetRegister(Register::BULK_READ,
              FixedBulkData(1, 1000, 0, 0, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f));
  wpi::GoBildaPinpoint pinpoint{wpi::I2C::Port::PORT_0};
  pinpoint.Update();

  pinpoint.SetPose(wpi::math::Pose2d{meter_t{7.0}, meter_t{-7.0},
                                     wpi::math::Rotation2d{radian_t{1.0}}});
  SetRegister(
      Register::BULK_READ,
      FixedBulkData(1, 1000, 0, 0, 7000.0f, -7000.0f, 1.0f, 0.0f, 0.0f, 0.0f));
  pinpoint.Update();
  CHECK(pinpoint.GetXPosition().value() == Catch::Approx(7.0));
  CHECK(pinpoint.GetYPosition().value() == Catch::Approx(-7.0));

  pinpoint.SetXPosition(meter_t{-7.0});
  pinpoint.SetYPosition(meter_t{7.0});
  pinpoint.SetHeading(radian_t{130.0});
  SetRegister(Register::BULK_READ,
              FixedBulkData(1, 1000, 0, 0, -7000.0f, 7000.0f, 130.0f, 0.0f,
                            0.0f, 0.0f));
  pinpoint.Update();
  CHECK(pinpoint.GetXPosition().value() == Catch::Approx(-7.0));
  CHECK(pinpoint.GetYPosition().value() == Catch::Approx(7.0));
  CHECK(pinpoint.GetHeading().value() == Catch::Approx(130.0));

  pinpoint.ResetPositionAndIMU();
  SetRegister(Register::BULK_READ,
              FixedBulkData(1, 1000, 0, 0, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f));
  pinpoint.Update();
  CHECK(pinpoint.GetXPosition().value() == Catch::Approx(0.0));
  CHECK(pinpoint.GetYPosition().value() == Catch::Approx(0.0));
  CHECK(pinpoint.GetHeading().value() == Catch::Approx(0.0));
  CHECK(pinpoint.GetDeviceStatus() ==
        wpi::GoBildaPinpoint::DeviceStatus::READY);
}

TEST_CASE_METHOD(
    PinpointTestFixture,
    "GoBildaPinpoint flexible reads reject invalid loop times before floats",
    "[drivers][gobilda-pinpoint]") {
  SetRegister(Register::DEVICE_VERSION, EncodeInt(3));
  wpi::GoBildaPinpoint pinpoint{wpi::I2C::Port::PORT_0};
  pinpoint.SetBulkReadScope({Register::X_POSITION, Register::LOOP_TIME});
  SetRegister(Register::BULK_READ, Concat(EncodeFloat(1500.0f), EncodeInt(0)));

  pinpoint.Update();

  CHECK(pinpoint.GetXPosition().value() == Catch::Approx(0.0));
  CHECK(pinpoint.GetDeviceStatus() ==
        wpi::GoBildaPinpoint::DeviceStatus::FAULT_BAD_READ);
  CHECK(pinpoint.GetLastFailedRegister() == Register::LOOP_TIME);
  CHECK(pinpoint.GetLastFailureReason() ==
        wpi::GoBildaPinpoint::FailureReason::INVALID_LOOP_TIME);
}

TEST_CASE_METHOD(
    PinpointTestFixture,
    "GoBildaPinpoint local validation rejects every nonfinite float register",
    "[drivers][gobilda-pinpoint]") {
  constexpr std::array floatRegisters = {
      Register::X_POSITION,   Register::Y_POSITION,   Register::H_ORIENTATION,
      Register::X_VELOCITY,   Register::Y_VELOCITY,   Register::H_VELOCITY,
      Register::MM_PER_TICK,  Register::X_POD_OFFSET, Register::Y_POD_OFFSET,
      Register::YAW_SCALAR,   Register::QUATERNION_W, Register::QUATERNION_X,
      Register::QUATERNION_Y, Register::QUATERNION_Z, Register::PITCH,
      Register::ROLL};
  SetRegister(Register::DEVICE_VERSION, EncodeInt(3));
  wpi::GoBildaPinpoint pinpoint{wpi::I2C::Port::PORT_0};

  for (Register reg : floatRegisters) {
    pinpoint.SetBulkReadScope({reg});
    SetRegister(Register::BULK_READ,
                EncodeFloat(std::numeric_limits<float>::quiet_NaN()));

    pinpoint.Update();

    CHECK(pinpoint.GetDeviceStatus() ==
          wpi::GoBildaPinpoint::DeviceStatus::FAULT_BAD_READ);
    CHECK(pinpoint.GetLastFailedRegister() == reg);
    CHECK(pinpoint.GetLastFailureReason() ==
          wpi::GoBildaPinpoint::FailureReason::NONFINITE_VALUE);
    CHECK(pinpoint.GetFailureCount(reg) == 1);
  }
}

TEST_CASE_METHOD(
    PinpointTestFixture,
    "GoBildaPinpoint establishes fresh pose baselines on entering local tests",
    "[drivers][gobilda-pinpoint]") {
  using ErrorDetectionType = wpi::GoBildaPinpoint::ErrorDetectionType;

  for (ErrorDetectionType initialMode :
       std::array{ErrorDetectionType::NONE, ErrorDetectionType::CRC}) {
    SetRegister(Register::DEVICE_VERSION, EncodeInt(3));
    auto corruptedPose =
        FixedBulkData(1, 1000, 0, 0, std::numeric_limits<float>::infinity(),
                      8000.0f, 200.0f, 0.0f, 0.0f, 0.0f);
    if (initialMode == ErrorDetectionType::CRC) {
      corruptedPose = AppendCrc(std::move(corruptedPose));
    }
    SetRegister(Register::BULK_READ, std::move(corruptedPose));

    wpi::GoBildaPinpoint pinpoint{wpi::I2C::Port::PORT_0};
    pinpoint.SetErrorDetectionType(initialMode);
    pinpoint.Update();

    CHECK(std::isinf(pinpoint.GetXPosition().value()));
    CHECK(pinpoint.GetYPosition().value() == Catch::Approx(8.0));
    CHECK(pinpoint.GetHeading().value() == Catch::Approx(200.0));

    pinpoint.SetErrorDetectionType(ErrorDetectionType::LOCAL_TEST);
    SetRegister(
        Register::BULK_READ,
        FixedBulkData(1, 1000, 0, 0, 1000.0f, 2000.0f, 0.5f, 0.0f, 0.0f, 0.0f));
    pinpoint.Update();

    CHECK(pinpoint.GetXPosition().value() == Catch::Approx(1.0));
    CHECK(pinpoint.GetYPosition().value() == Catch::Approx(2.0));
    CHECK(pinpoint.GetHeading().value() == Catch::Approx(0.5));
    CHECK(pinpoint.GetDeviceStatus() ==
          wpi::GoBildaPinpoint::DeviceStatus::READY);
    CHECK(pinpoint.GetFailureCount() == 0);
  }
}

TEST_CASE_METHOD(
    PinpointTestFixture,
    "GoBildaPinpoint transient read failures preserve cached device status",
    "[drivers][gobilda-pinpoint]") {
  using DeviceStatus = wpi::GoBildaPinpoint::DeviceStatus;
  SetRegister(Register::DEVICE_VERSION, EncodeInt(3));
  wpi::GoBildaPinpoint pinpoint{wpi::I2C::Port::PORT_0};

  pinpoint.SetBulkReadScope({Register::DEVICE_STATUS, Register::H_ORIENTATION});
  SetRegister(Register::BULK_READ,
              Concat(EncodeInt(static_cast<int32_t>(DeviceStatus::CALIBRATING)),
                     EncodeFloat(0.25f)));
  pinpoint.Update();
  CHECK(pinpoint.GetDeviceStatus() == DeviceStatus::CALIBRATING);

  SetRegister(Register::H_ORIENTATION,
              EncodeFloat(std::numeric_limits<float>::quiet_NaN()));
  pinpoint.UpdateHeading();
  CHECK(pinpoint.GetDeviceStatus() == DeviceStatus::FAULT_BAD_READ);
  CHECK(pinpoint.GetDeviceStatusBits() ==
        (static_cast<int32_t>(DeviceStatus::CALIBRATING) |
         static_cast<int32_t>(DeviceStatus::FAULT_BAD_READ)));

  SetRegister(Register::H_ORIENTATION, EncodeFloat(0.5f));
  pinpoint.UpdateHeading();
  CHECK(pinpoint.GetDeviceStatus() == DeviceStatus::CALIBRATING);
  CHECK(pinpoint.GetDeviceStatusBits() ==
        static_cast<int32_t>(DeviceStatus::CALIBRATING));

  SetRegister(Register::BULK_READ,
              Concat(EncodeInt(static_cast<int32_t>(
                         DeviceStatus::FAULT_X_POD_NOT_DETECTED)),
                     EncodeFloat(0.75f)));
  pinpoint.Update();
  CHECK(pinpoint.GetDeviceStatus() == DeviceStatus::FAULT_X_POD_NOT_DETECTED);

  pinpoint.SetBulkReadScope({Register::H_ORIENTATION});
  SetRegister(Register::BULK_READ,
              EncodeFloat(std::numeric_limits<float>::quiet_NaN()));
  pinpoint.Update();
  CHECK(pinpoint.GetDeviceStatus() == DeviceStatus::FAULT_BAD_READ);
  CHECK(pinpoint.GetDeviceStatusBits() ==
        (static_cast<int32_t>(DeviceStatus::FAULT_X_POD_NOT_DETECTED) |
         static_cast<int32_t>(DeviceStatus::FAULT_BAD_READ)));

  SetRegister(Register::BULK_READ, EncodeFloat(1.0f));
  pinpoint.Update();
  CHECK(pinpoint.GetDeviceStatus() == DeviceStatus::FAULT_X_POD_NOT_DETECTED);
  CHECK(pinpoint.GetDeviceStatusBits() ==
        static_cast<int32_t>(DeviceStatus::FAULT_X_POD_NOT_DETECTED));
}

TEST_CASE_METHOD(PinpointTestFixture,
                 "GoBildaPinpoint rejects invalid configuration",
                 "[drivers][gobilda-pinpoint]") {
  CHECK_THROWS_AS(wpi::GoBildaPinpoint(wpi::I2C::Port::PORT_0, -1),
                  std::invalid_argument);
  CHECK_THROWS_AS(wpi::GoBildaPinpoint(wpi::I2C::Port::PORT_0, 0x80),
                  std::invalid_argument);

  wpi::GoBildaPinpoint pinpoint{wpi::I2C::Port::PORT_0};
  CHECK_THROWS_AS(pinpoint.SetOffsets(
                      wpi::units::meters<>{1.0},
                      wpi::units::meters<>{std::numeric_limits<double>::max()}),
                  std::invalid_argument);
  CHECK(m_writes.empty());
  CHECK_THROWS_AS(pinpoint.SetEncoderResolution(0.0), std::invalid_argument);
  CHECK_THROWS_AS(
      pinpoint.SetYawScalar(std::numeric_limits<double>::infinity()),
      std::invalid_argument);
}

TEST_CASE_METHOD(PinpointTestFixture,
                 "GoBildaPinpoint validates the entire pose before writing",
                 "[drivers][gobilda-pinpoint]") {
  wpi::GoBildaPinpoint pinpoint{wpi::I2C::Port::PORT_0};

  CHECK_THROWS_AS(pinpoint.SetPose(wpi::math::Pose2d{
                      wpi::units::meters<>{1.0},
                      wpi::units::meters<>{std::numeric_limits<double>::max()},
                      wpi::math::Rotation2d{}}),
                  std::invalid_argument);
  CHECK(m_writes.empty());

  CHECK_THROWS_AS(pinpoint.SetPose(wpi::math::Pose2d{
                      wpi::units::meters<>{1.0}, wpi::units::meters<>{2.0},
                      wpi::math::Rotation2d{wpi::units::radians<>{
                          std::numeric_limits<double>::quiet_NaN()}}}),
                  std::invalid_argument);
  CHECK(m_writes.empty());
}

TEST_CASE_METHOD(
    PinpointTestFixture,
    "GoBildaPinpoint converts encoder resolution to and from device units",
    "[drivers][gobilda-pinpoint]") {
  SetRegister(Register::MM_PER_TICK, EncodeFloat(12.345f));
  wpi::GoBildaPinpoint pinpoint{wpi::I2C::Port::PORT_0};

  pinpoint.SetEncoderResolution(wpi::GoBildaPinpoint::OdometryPod::SWINGARM);
  CHECK(m_writes.back() == Concat(std::vector<uint8_t>{static_cast<uint8_t>(
                                      Register::MM_PER_TICK)},
                                  EncodeFloat(13.26291192f)));

  pinpoint.SetEncoderResolution(54321.0);
  CHECK(m_writes.back() == Concat(std::vector<uint8_t>{static_cast<uint8_t>(
                                      Register::MM_PER_TICK)},
                                  EncodeFloat(54.321f)));
  CHECK(pinpoint.GetEncoderResolution() == Catch::Approx(12345.0));
}

}  // namespace
