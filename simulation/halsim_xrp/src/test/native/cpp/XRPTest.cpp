// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/halsim/xrp/XRP.hpp"

#include <algorithm>
#include <array>
#include <bit>
#include <cstdint>
#include <future>
#include <limits>
#include <memory>
#include <string>
#include <vector>

#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>

#include "wpi/hal/Encoder.h"
#include "wpi/hal/HAL.h"
#include "wpi/hal/Ports.h"
#include "wpi/hal/Power.h"
#include "wpi/hal/SimDevice.h"
#include "wpi/hal/simulation/AnalogInData.h"
#include "wpi/hal/simulation/DIOData.h"
#include "wpi/hal/simulation/DriverStationData.h"
#include "wpi/hal/simulation/EncoderData.h"
#include "wpi/hal/simulation/RoboRioData.h"
#include "wpi/hal/simulation/SimDeviceData.h"
#include "wpi/halsim/xrp/HALSimXRP.hpp"
#include "wpi/net/EventLoopRunner.hpp"
#include "wpi/util/Endian.hpp"

using namespace wpilibxrp;

namespace {

struct HALSimulationTest {
  HALSimulationTest() {
    HALSIM_ResetSimDeviceData();
    HALSIM_ResetDriverStationData();
    HALSIM_ResetRoboRioData();
    for (int i = 0; i < HAL_GetNumDigitalChannels(); ++i) {
      HALSIM_ResetDIOData(i);
    }
    for (int i = 0; i < HAL_GetNumAnalogInputs(); ++i) {
      HALSIM_ResetAnalogInData(i);
    }
    for (int i = 0; i < HAL_GetNumEncoders(); ++i) {
      HALSIM_ResetEncoderData(i);
    }
  }

  ~HALSimulationTest() {
    HALSIM_ResetSimDeviceData();
    HALSIM_ResetRoboRioData();
  }
};

struct TestEncoder {
  TestEncoder(int channelA, int channelB) {
    int32_t status = 0;
    handle = HAL_InitializeEncoder(channelA, channelB, false,
                                   HAL_ENCODER_4X_ENCODING, &status);
    REQUIRE(status == 0);
    index = HALSIM_FindEncoderForChannel(channelA);
    REQUIRE(index >= 0);
  }

  ~TestEncoder() { HAL_FreeEncoder(handle); }

  HAL_EncoderHandle handle;
  int index;
};

HAL_SimValueHandle SetSimOutput(const char* deviceName, const char* valueName,
                                double value) {
  auto device = HALSIM_GetSimDeviceHandle(deviceName);
  if (!device) {
    device = HAL_CreateSimDevice(deviceName);
  }
  auto handle = HALSIM_GetSimValueHandle(device, valueName);
  if (!handle) {
    handle = HAL_CreateSimValueDouble(device, valueName, HAL_SIM_VALUE_OUTPUT,
                                      value);
  } else {
    HAL_SetSimValueDouble(handle, value);
  }
  return handle;
}

void ReceiveSequence(XRP& xrp, uint16_t seq) {
  std::array<uint8_t, 5> packet{static_cast<uint8_t>(seq >> 8),
                                static_cast<uint8_t>(seq), 0, 0, 0};
  xrp.HandleXRPUpdate(packet);
}

std::vector<uint8_t> MakeStatus(uint16_t seq, uint16_t mask,
                                std::initializer_list<uint8_t> payload) {
  std::vector<uint8_t> result(PACKET_HEADER_SIZE + payload.size());
  result[0] = static_cast<uint8_t>(seq >> 8);
  result[1] = static_cast<uint8_t>(seq);
  result[2] = 0;
  result[3] = static_cast<uint8_t>(mask >> 8);
  result[4] = static_cast<uint8_t>(mask);
  std::copy(payload.begin(), payload.end(),
            result.begin() + PACKET_HEADER_SIZE);
  return result;
}

std::vector<uint8_t> MakeControl(XRP& xrp, std::string_view name = {},
                                 bool identify = false) {
  wpi::util::SmallVector<wpi::net::uv::Buffer, 4> buffers;
  {
    wpi::net::raw_uv_ostream stream{buffers, 512};
    if (identify) {
      xrp.SetupIdentifyBuffer(stream);
    } else if (name.empty()) {
      xrp.SetupXRPSendBuffer(stream);
    } else {
      xrp.SetupRenameDeviceBuffer(stream, name);
    }
  }
  std::vector<uint8_t> packet;
  for (auto& buffer : buffers) {
    auto bytes = buffer.bytes();
    packet.insert(packet.end(), bytes.begin(), bytes.end());
    buffer.Deallocate();
  }
  return packet;
}

}  // namespace

TEST_CASE_METHOD(HALSimulationTest,
                 "XRP accepts status gaps across sequence rollover", "[xrp]") {
  XRP xrp;
  ReceiveSequence(xrp, 65532);
  ReceiveSequence(xrp, 1);
  CHECK(xrp.GetDataSnapshot().status.packet.sequence == 1);
  ReceiveSequence(xrp, 65535);
  CHECK(xrp.GetDataSnapshot().status.packet.sequence == 1);
  ReceiveSequence(xrp, 2);
  CHECK(xrp.GetDataSnapshot().status.packet.sequence == 2);
}

TEST_CASE_METHOD(HALSimulationTest,
                 "XRP ignores duplicate stale and ambiguous status sequences",
                 "[xrp]") {
  XRP xrp;
  ReceiveSequence(xrp, 10);
  auto first = xrp.GetDataSnapshot().status.packet.lastUpdate;
  for (uint16_t seq : {10, 9, 65535, 32778}) {
    ReceiveSequence(xrp, seq);
    CHECK(xrp.GetDataSnapshot().status.packet.sequence == 10);
    CHECK(xrp.GetDataSnapshot().status.packet.lastUpdate == first);
  }
  ReceiveSequence(xrp, 32777);
  CHECK(xrp.GetDataSnapshot().status.packet.sequence == 32777);
  xrp.ResetStatusPacketSequence();
  ReceiveSequence(xrp, 0);
  CHECK(xrp.GetDataSnapshot().status.packet.sequence == 0);
}

TEST_CASE_METHOD(HALSimulationTest,
                 "XRP malformed status does not consume a sequence", "[xrp]") {
  XRP xrp;
  ReceiveSequence(xrp, 1);
  std::array<uint8_t, 5> unknownField{0, 2, 0, 0x80, 0};
  std::array<uint8_t, 5> missingEncoder{0, 2, 0, 0, 1};
  std::array<uint8_t, 6> extraPayload{0, 2, 0, 0, 0, 0};
  xrp.HandleXRPUpdate(unknownField);
  xrp.HandleXRPUpdate(missingEncoder);
  xrp.HandleXRPUpdate(extraPayload);
  CHECK(xrp.GetDataSnapshot().status.packet.sequence == 1);
  ReceiveSequence(xrp, 2);
  CHECK(xrp.GetDataSnapshot().status.packet.sequence == 2);
}

TEST_CASE_METHOD(HALSimulationTest,
                 "XRP reads command acknowledgement status packets", "[xrp]") {
  XRP xrp;
  auto ack =
      MakeStatus(1, STATUS_COMMAND_ACK, {0, 7, 0x80, 0, COMMAND_ACK_SUCCESS});
  CHECK(xrp.HandleXRPUpdate(ack));
  auto snapshot = xrp.GetDataSnapshot();
  REQUIRE(snapshot.status.commandAck.present);
  CHECK(snapshot.status.commandAck.value.controlSeq == 7);
  CHECK(snapshot.status.commandAck.value.controlFieldMask ==
        CONTROL_DEVICE_NAME);
  CHECK(snapshot.status.commandAck.value.result == COMMAND_ACK_SUCCESS);

  auto firstUpdate = snapshot.status.commandAck.lastUpdate;
  CHECK_FALSE(xrp.HandleXRPUpdate(ack));
  snapshot = xrp.GetDataSnapshot();
  CHECK(snapshot.status.commandAck.lastUpdate == firstUpdate);

  auto rejected =
      MakeStatus(2, STATUS_COMMAND_ACK, {0, 8, 0x80, 0, COMMAND_ACK_REJECTED});
  CHECK(xrp.HandleXRPUpdate(rejected));
  snapshot = xrp.GetDataSnapshot();
  CHECK(snapshot.status.commandAck.value.controlSeq == 8);
  CHECK(snapshot.status.commandAck.value.result == COMMAND_ACK_REJECTED);
}

TEST_CASE_METHOD(HALSimulationTest,
                 "XRP encoder update preserves count and signed period",
                 "[xrp]") {
  XRP xrp;
  TestEncoder other{0, 1};
  TestEncoder encoder{4, 5};
  HALSIM_SetEncoderDistancePerPulse(encoder.index, 0.25);
  // Firmware encoding: count -42, period 500 us, reverse direction.
  std::array<uint8_t, 13> packet{0,    1,    0, 0, 1, 0xff, 0xff,
                                 0xff, 0xd6, 0, 0, 3, 0xe8};
  xrp.HandleXRPUpdate(packet);
  CHECK(HALSIM_GetEncoderCount(encoder.index) == -42);
  CHECK(HALSIM_GetEncoderRate(encoder.index) == Catch::Approx(-500.0));

  packet[1] = 2;
  packet[12] = 0xe9;
  xrp.HandleXRPUpdate(packet);
  CHECK(HALSIM_GetEncoderRate(encoder.index) == Catch::Approx(500.0));

  packet[1] = 3;
  for (int i = 9; i < 13; ++i) {
    packet[i] = 0xff;
  }
  xrp.HandleXRPUpdate(packet);
  CHECK(HALSIM_GetEncoderCount(encoder.index) == -42);
  CHECK(HALSIM_GetEncoderRate(encoder.index) == Catch::Approx(500.0));
}

TEST_CASE_METHOD(HALSimulationTest,
                 "XRP control and rename use the firmware wire format",
                 "[xrp]") {
  XRP xrp;
  HALSIM_SetDriverStationEnabled(true);
  const std::array motors{"XRPMotor:motorL", "XRPMotor:motorR",
                          "XRPMotor:motor3", "XRPMotor:motor4"};
  const std::array servos{"XRPServo:servo1", "XRPServo:servo2",
                          "XRPServo:servo3", "XRPServo:servo4"};
  for (int i = 0; i < 4; ++i) {
    SetSimOutput(motors[i], "throttle", i % 2 ? -1.0 : 1.0);
    SetSimOutput(servos[i], "position", i % 2 ? 1.0 : 0.0);
  }
  HALSIM_SetDIOInitialized(1, true);
  HALSIM_SetDIOIsInput(1, false);
  HALSIM_SetDIOValue(1, true);
  const std::vector<uint8_t> expected{0,    0, 1,   1,    0xff, 0, 0xff,
                                      0xff, 1, 0,   0xff, 0xff, 1, 0,
                                      180,  0, 180, 2,    2};
  CHECK(MakeControl(xrp) == expected);
  const std::vector<uint8_t> rename{0, 1, 1, 0x80, 0, 3, 'X', 'R', 'P'};
  CHECK(MakeControl(xrp, "XRP") == rename);
  auto next = MakeControl(xrp);
  CHECK(next[0] == 0);
  CHECK(next[1] == 2);
}

TEST_CASE_METHOD(HALSimulationTest,
                 "XRP encodes non-finite actuator values as zero", "[xrp]") {
  XRP xrp;
  for (double value : {std::numeric_limits<double>::quiet_NaN(),
                       std::numeric_limits<double>::infinity(),
                       -std::numeric_limits<double>::infinity()}) {
    SetSimOutput("XRPMotor:motorL", "throttle", value);
    SetSimOutput("XRPServo:servo1", "position", value);
    auto packet = MakeControl(xrp);
    REQUIRE(packet.size() == PACKET_HEADER_SIZE + 4 * 2 + 2);
    CHECK(packet[5] == 0);
    CHECK(packet[6] == 0);
    CHECK(packet[13] == 0);
  }

  SetSimOutput("XRPMotor:motorL", "throttle", 0.5);
  SetSimOutput("XRPServo:servo1", "position", 0.5);
  auto packet = MakeControl(xrp);
  CHECK(packet[5] == 0);
  CHECK(packet[6] == 127);
  CHECK(packet[13] == 90);
}

TEST_CASE_METHOD(HALSimulationTest,
                 "XRP identify uses an isolated sequenced command", "[xrp]") {
  XRP xrp;
  CHECK(MakeControl(xrp, {}, true) == std::vector<uint8_t>{0, 0, 0, 0x40, 0});
  HALSIM_SetDriverStationEnabled(true);
  SetSimOutput("XRPMotor:motorL", "throttle", 0.5);
  auto control = MakeControl(xrp);
  CHECK(MakeControl(xrp, {}, true) == std::vector<uint8_t>{0, 2, 1, 0x40, 0});
  auto next = MakeControl(xrp);
  REQUIRE(next.size() == control.size());
  CHECK(next[1] == 3);
  CHECK(std::equal(next.begin() + 2, next.end(), control.begin() + 2));
  CHECK(MakeControl(xrp, "XRP") ==
        std::vector<uint8_t>{0, 4, 1, 0x80, 0, 3, 'X', 'R', 'P'});
}

TEST_CASE_METHOD(HALSimulationTest,
                 "XRP identify acknowledgement accompanies sensor telemetry",
                 "[xrp]") {
  XRP xrp;
  auto packet = MakeStatus(
      10,
      STATUS_DIO | STATUS_INPUT_VOLTAGE | STATUS_TIMING | STATUS_COMMAND_ACK,
      {1, 1, 0x19, 0xfc, 0, 7, 0, 10, 0x12, 0x34, 0x40, 0,
       COMMAND_ACK_SUCCESS});
  auto truncated = packet;
  truncated.pop_back();
  CHECK_FALSE(xrp.HandleXRPUpdate(truncated));
  CHECK(xrp.HandleXRPUpdate(packet));
  auto snapshot = xrp.GetDataSnapshot();
  CHECK(snapshot.status.digitalInputs[0].present);
  CHECK(snapshot.status.digitalInputs[0].value);
  REQUIRE(snapshot.status.inputVoltage.present);
  CHECK(snapshot.status.inputVoltage.value == Catch::Approx(6.652));
  REQUIRE(snapshot.status.commandAck.present);
  CHECK(snapshot.status.commandAck.value.controlSeq == 0x1234);
  CHECK(snapshot.status.commandAck.value.controlFieldMask == CONTROL_IDENTIFY);
  CHECK(snapshot.status.commandAck.value.result == COMMAND_ACK_SUCCESS);
}

TEST_CASE_METHOD(HALSimulationTest,
                 "XRP control reads current HAL state and device lifetimes",
                 "[xrp]") {
  XRP xrp;
  auto defaults = MakeControl(xrp);
  CHECK(defaults[2] == 0);
  CHECK(defaults[3] == 0);
  CHECK(defaults[4] == 0x3f);
  CHECK(defaults[13] == 90);
  CHECK(defaults[14] == 90);

  auto motor = SetSimOutput("XRPMotor:motorL", "throttle", 0.5);
  SetSimOutput("XRPServo:servo3", "position", 0.25);
  HALSIM_SetDriverStationEnabled(true);
  HALSIM_SetDIOInitialized(1, true);
  HALSIM_SetDIOIsInput(1, false);
  HALSIM_SetDIOValue(1, true);
  auto first = MakeControl(xrp);
  CHECK(first[2] == 1);
  CHECK(first[3] == 1);
  CHECK(first[4] == 0x7f);
  CHECK(first[6] == 127);
  CHECK(first[15] == 45);
  CHECK(first[16] == 2);
  CHECK(first[17] == 2);

  HAL_SetSimValueDouble(motor, -0.5);
  HALSIM_SetDriverStationEnabled(false);
  HALSIM_SetDIOIsInput(1, true);
  HAL_FreeSimDevice(HALSIM_GetSimDeviceHandle("XRPServo:servo3"));
  auto second = MakeControl(xrp);
  CHECK(second[2] == 0);
  CHECK(second[3] == 0);
  CHECK(second[4] == 0x3f);
  CHECK(second[5] == 0xff);
  CHECK(second[6] == 0x81);

  HAL_FreeSimDevice(HALSIM_GetSimDeviceHandle("XRPMotor:motorL"));
  CHECK(MakeControl(xrp)[6] == 0);
  SetSimOutput("XRPMotor:motorL", "throttle", 1.0);
  CHECK(MakeControl(xrp)[6] == 255);

  HALSIM_SetDIOIsInput(1, false);
  HALSIM_SetDIOInitialized(1, false);
  CHECK(MakeControl(xrp)[3] == 0);
}

TEST_CASE_METHOD(HALSimulationTest,
                 "XRP status writes digital and analog HAL inputs", "[xrp]") {
  XRP xrp;
  HALSIM_SetDIOInitialized(0, true);
  HALSIM_SetDIOIsInput(0, true);
  HALSIM_SetDIOValue(0, false);
  HALSIM_SetDIOInitialized(1, true);
  HALSIM_SetDIOIsInput(1, false);
  HALSIM_SetDIOValue(1, true);
  HALSIM_SetDIOInitialized(2, true);
  HALSIM_SetDIOValue(2, false);

  auto packet = MakeStatus(
      1, STATUS_DIO | STATUS_ANALOG_0 | STATUS_ANALOG_1 | STATUS_ANALOG_2,
      {3, 1, 0, 0, 0x80, 0, 0xff, 0xff});
  REQUIRE(xrp.HandleXRPUpdate(packet));
  CHECK(HALSIM_GetDIOValue(0));
  CHECK(HALSIM_GetDIOValue(1));
  CHECK_FALSE(HALSIM_GetDIOValue(2));
  CHECK(HALSIM_GetAnalogInVoltage(0) == 0.0);
  CHECK(HALSIM_GetAnalogInVoltage(1) == Catch::Approx(2.500038));
  CHECK(HALSIM_GetAnalogInVoltage(2) == 5.0);

  HALSIM_SetDIOValue(0, false);
  HALSIM_SetAnalogInVoltage(2, 1.0);
  CHECK_FALSE(xrp.HandleXRPUpdate(packet));
  CHECK_FALSE(HALSIM_GetDIOValue(0));
  CHECK(HALSIM_GetAnalogInVoltage(2) == 1.0);
  packet[1] = 2;
  packet.pop_back();
  CHECK_FALSE(xrp.HandleXRPUpdate(packet));
  CHECK_FALSE(HALSIM_GetDIOValue(0));
  CHECK(HALSIM_GetAnalogInVoltage(2) == 1.0);
}

TEST_CASE_METHOD(HALSimulationTest,
                 "XRP input voltage updates HAL battery voltage and GUI data",
                 "[xrp]") {
  XRP xrp;
  CHECK_FALSE(xrp.GetDataSnapshot().status.inputVoltage.present);
  HALSIM_SetAnalogInVoltage(3, 2.0);
  // 7.25 V in millivolts, alongside a 5 V analog input.
  auto packet = MakeStatus(1, STATUS_ANALOG_2 | STATUS_INPUT_VOLTAGE,
                           {0xff, 0xff, 0x1c, 0x52});
  REQUIRE(xrp.HandleXRPUpdate(packet));
  auto voltage = xrp.GetDataSnapshot().status.inputVoltage;
  REQUIRE(voltage.present);
  CHECK(voltage.value == 7.25f);
  CHECK(voltage.lastUpdate >= xrp.GetDataSnapshot().status.packet.lastUpdate);
  CHECK(HALSIM_GetAnalogInVoltage(2) == 5.0);
  CHECK(HALSIM_GetAnalogInVoltage(3) == 2.0);
  int32_t status = 0;
  // RobotController.getBatteryVoltage() reads this HAL entry point.
  CHECK(HAL_GetVinVoltage(&status) == 7.25);
  CHECK(status == 0);

  uint16_t seq = 2;
  for (uint16_t millivolts : {0, 13300, 65535}) {
    packet = MakeStatus(seq++, STATUS_INPUT_VOLTAGE,
                        {static_cast<uint8_t>(millivolts >> 8),
                         static_cast<uint8_t>(millivolts)});
    REQUIRE(xrp.HandleXRPUpdate(packet));
    CHECK(HAL_GetVinVoltage(&status) == Catch::Approx(millivolts / 1000.0));
    CHECK(xrp.GetDataSnapshot().status.inputVoltage.value ==
          Catch::Approx(millivolts / 1000.0));
  }
}

TEST_CASE_METHOD(HALSimulationTest,
                 "XRP decodes a full sensor status with voltage timing and ACK",
                 "[xrp]") {
  XRP xrp;
  // Full wire layout: sensors through byte 80, VIN at 81, timing at 83,
  // and the five-byte command ACK at 87.
  std::vector<uint8_t> packet(92);
  packet[1] = 1;
  packet[3] = 0x1f;
  packet[4] = 0xff;
  packet[81] = 0x19;
  packet[82] = 0xfc;
  packet[84] = 7;
  packet[86] = 10;
  packet[87] = 0x12;
  packet[88] = 0x34;
  packet[89] = 0x40;
  packet[91] = COMMAND_ACK_SUCCESS;
  for (size_t size = 0; size < packet.size(); ++size) {
    CHECK_FALSE(xrp.HandleXRPUpdate(std::span{packet}.first(size)));
  }
  CHECK_FALSE(xrp.GetDataSnapshot().status.inputVoltage.present);
  CHECK(HALSIM_GetRoboRioVInVoltage() == 12.0);
  REQUIRE(xrp.HandleXRPUpdate(packet));
  auto snapshot = xrp.GetDataSnapshot();
  CHECK(snapshot.status.analogInputs[2].present);
  CHECK(snapshot.status.analogInputs[2].value == 0.0f);
  CHECK(snapshot.status.inputVoltage.value == Catch::Approx(6.652));
  CHECK(HALSIM_GetRoboRioVInVoltage() == Catch::Approx(6.652));
  CHECK(snapshot.status.commandAck.value.controlSeq == 0x1234);
  CHECK(snapshot.status.commandAck.value.controlFieldMask == CONTROL_IDENTIFY);
  CHECK(snapshot.status.commandAck.value.result == COMMAND_ACK_SUCCESS);
}

TEST_CASE_METHOD(HALSimulationTest,
                 "XRP rejects malformed and stale input voltage updates",
                 "[xrp]") {
  XRP xrp;
  REQUIRE(
      xrp.HandleXRPUpdate(MakeStatus(10, STATUS_INPUT_VOLTAGE, {0x1c, 0x52})));
  auto voltage = xrp.GetDataSnapshot().status.inputVoltage;
  for (uint16_t seq : {9, 10}) {
    CHECK_FALSE(
        xrp.HandleXRPUpdate(MakeStatus(seq, STATUS_INPUT_VOLTAGE, {0, 0})));
  }
  for (auto packet : {MakeStatus(11, STATUS_INPUT_VOLTAGE, {}),
                      MakeStatus(11, STATUS_INPUT_VOLTAGE, {0}),
                      MakeStatus(11, STATUS_INPUT_VOLTAGE, {0, 0, 0})}) {
    CHECK_FALSE(xrp.HandleXRPUpdate(packet));
  }
  CHECK(HALSIM_GetRoboRioVInVoltage() == 7.25);
  CHECK(xrp.GetDataSnapshot().status.inputVoltage.lastUpdate ==
        voltage.lastUpdate);
  // An ACK-only packet has no voltage sample.
  REQUIRE(xrp.HandleXRPUpdate(MakeStatus(
      11, STATUS_COMMAND_ACK, {0, 7, 0x80, 0, COMMAND_ACK_SUCCESS})));
  CHECK(HALSIM_GetRoboRioVInVoltage() == 7.25);
  CHECK(xrp.GetDataSnapshot().status.inputVoltage.lastUpdate ==
        voltage.lastUpdate);
  xrp.ResetStatusPacketSequence();
  REQUIRE(
      xrp.HandleXRPUpdate(MakeStatus(0, STATUS_INPUT_VOLTAGE, {0x19, 0xfc})));
  CHECK(HALSIM_GetRoboRioVInVoltage() == Catch::Approx(6.652));
}

TEST_CASE_METHOD(HALSimulationTest,
                 "XRP gyro status updates only the XRP SimDevice", "[xrp]") {
  XRP xrp;
  auto packet = MakeStatus(1, STATUS_GYRO, {});
  const std::array values{1.5f, -2.0f, 3.0f, 45.0f, -90.0f, 180.0f};
  for (float value : values) {
    std::array<uint8_t, 4> bytes;
    wpi::util::support::endian::write32be(bytes.data(),
                                          std::bit_cast<uint32_t>(value));
    packet.insert(packet.end(), bytes.begin(), bytes.end());
  }
  // Sensor packets may arrive before the robot program creates its devices.
  REQUIRE(xrp.HandleXRPUpdate(packet));
  auto gyro = HAL_CreateSimDevice("Gyro:XRPGyro");
  const std::array names{"rate_x",  "rate_y",  "rate_z",
                         "angle_x", "angle_y", "angle_z"};
  std::array<HAL_SimValueHandle, 6> handles;
  for (size_t i = 0; i < handles.size(); ++i) {
    handles[i] =
        HAL_CreateSimValueDouble(gyro, names[i], HAL_SIM_VALUE_INPUT, 0.0);
  }
  auto other = HAL_CreateSimDevice("Gyro:Other");
  auto otherAngle =
      HAL_CreateSimValueDouble(other, "angle_z", HAL_SIM_VALUE_INPUT, 7.0);
  packet[1] = 2;
  REQUIRE(xrp.HandleXRPUpdate(packet));
  for (size_t i = 0; i < handles.size(); ++i) {
    CHECK(HAL_GetSimValueDouble(handles[i]) == values[i]);
  }
  CHECK(HAL_GetSimValueDouble(otherAngle) == 7.0);

  HAL_FreeSimDevice(gyro);
  auto replacement = HAL_CreateSimDevice("Gyro:XRPGyro");
  auto angle = HAL_CreateSimValueDouble(replacement, "angle_z",
                                        HAL_SIM_VALUE_INPUT, 0.0);
  packet[1] = 3;
  REQUIRE(xrp.HandleXRPUpdate(packet));
  CHECK(HAL_GetSimValueDouble(angle) == 180.0);
}

TEST_CASE_METHOD(HALSimulationTest,
                 "XRP encoder resets and reallocations preserve HAL counts",
                 "[xrp]") {
  XRP xrp;
  auto packet = MakeStatus(1, STATUS_ENCODER_0, {0, 0, 0, 42, 0, 0, 3, 0xe9});
  {
    // Channel order can be reversed; HAL allocation index is independent of
    // the XRP encoder ID.
    TestEncoder other{0, 1};
    TestEncoder encoder{5, 4};
    REQUIRE(xrp.HandleXRPUpdate(packet));
    CHECK(HALSIM_GetEncoderCount(encoder.index) == 42);
    CHECK(HALSIM_GetEncoderCount(other.index) == 0);
    int32_t status = 0;
    HAL_ResetEncoder(encoder.handle, &status);
    REQUIRE(status == 0);
    CHECK(HALSIM_GetEncoderCount(encoder.index) == 0);
    packet[1] = 2;
    packet[8] = 50;
    REQUIRE(xrp.HandleXRPUpdate(packet));
    CHECK(HALSIM_GetEncoderCount(encoder.index) == 8);
    HAL_ResetEncoder(encoder.handle, &status);
    packet[1] = 3;
    packet[8] = 55;
    REQUIRE(xrp.HandleXRPUpdate(packet));
    CHECK(HALSIM_GetEncoderCount(encoder.index) == 5);
  }
  {
    TestEncoder unrelated{4, 6};
    packet[1] = 4;
    REQUIRE(xrp.HandleXRPUpdate(packet));
    CHECK(HALSIM_GetEncoderCount(unrelated.index) == 0);
  }
  {
    TestEncoder replacement{4, 5};
    packet[1] = 5;
    REQUIRE(xrp.HandleXRPUpdate(packet));
    CHECK(HALSIM_GetEncoderCount(replacement.index) == 55);
  }
}

TEST_CASE_METHOD(HALSimulationTest,
                 "XRP maps all four encoder channel pairs to HAL", "[xrp]") {
  XRP xrp;
  TestEncoder encoder3{11, 10};
  TestEncoder encoder2{8, 9};
  TestEncoder encoder1{7, 6};
  TestEncoder encoder0{4, 5};
  auto packet = MakeStatus(
      1,
      STATUS_ENCODER_0 | STATUS_ENCODER_1 | STATUS_ENCODER_2 | STATUS_ENCODER_3,
      {0,    0,    0,    10,   0xff, 0xff, 0xff, 0xff, 0,    0,    0,
       20,   0xff, 0xff, 0xff, 0xff, 0,    0,    0,    30,   0xff, 0xff,
       0xff, 0xff, 0,    0,    0,    40,   0xff, 0xff, 0xff, 0xff});
  REQUIRE(xrp.HandleXRPUpdate(packet));
  CHECK(HALSIM_GetEncoderCount(encoder0.index) == 10);
  CHECK(HALSIM_GetEncoderCount(encoder1.index) == 20);
  CHECK(HALSIM_GetEncoderCount(encoder2.index) == 30);
  CHECK(HALSIM_GetEncoderCount(encoder3.index) == 40);
}

TEST_CASE_METHOD(HALSimulationTest,
                 "XRP sends HAL outputs on simulation periodic after",
                 "[xrp]") {
  wpi::net::EventLoopRunner runner;
  std::shared_ptr<HALSimXRP> xrp;
  runner.ExecSync([&](wpi::net::uv::Loop& loop) {
    xrp = std::make_shared<HALSimXRP>(loop);
    // Exercise HAL callbacks without creating a Bluetooth connection.
    xrp->Start();
    xrp->Start();
  });
  CHECK_FALSE(xrp->GetDataSnapshot().control.packet.present);
  SetSimOutput("XRPMotor:motorL", "throttle", 0.5);
  HALSIM_SetDriverStationEnabled(true);
  HAL_SimPeriodicAfter();
  std::promise<void> flushed;
  auto future = flushed.get_future();
  xrp->GetExec().Send([&] { flushed.set_value(); });
  future.get();
  auto snapshot = xrp->GetDataSnapshot();
  REQUIRE(snapshot.control.packet.present);
  CHECK(snapshot.control.packet.sequence == 0);
  CHECK(snapshot.control.enabled);
  CHECK(snapshot.control.motors[0].value == 0.5f);

  std::weak_ptr<HALSimXRP> weakXrp = xrp;
  runner.ExecSync([&](wpi::net::uv::Loop&) { xrp.reset(); });
  CHECK(weakXrp.expired());
  // Destroying the client must remove its callback before the next cycle.
  HAL_SimPeriodicAfter();
}
