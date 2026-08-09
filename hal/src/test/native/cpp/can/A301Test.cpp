// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/hal/A301.h"

#include <algorithm>
#include <array>
#include <bit>
#include <cstdint>
#include <cstring>
#include <vector>

#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>

#include "wpi/hal/CAN.h"
#include "wpi/hal/CANAPITypes.h"
#include "wpi/hal/Errors.h"
#include "wpi/hal/HAL.h"
#include "wpi/hal/handles/HandlesInternal.hpp"
#include "wpi/hal/simulation/CanData.h"
#include "wpi/util/timestamp.hpp"

namespace {

struct SentFrame {
  int32_t busId;
  uint32_t messageId;
  HAL_CANMessage message;
  int32_t periodMs;
};

struct CallbackHandle {
  int32_t handle;
  void (*cancel)(int32_t);

  ~CallbackHandle() { cancel(handle); }
};

struct A301Handle {
  HAL_A301Handle handle;

  ~A301Handle() { HAL_FreeA301(handle); }
};

void StoreU32LE(uint8_t* data, uint32_t value) {
  data[0] = value;
  data[1] = value >> 8;
  data[2] = value >> 16;
  data[3] = value >> 24;
}

void StoreFloatLE(uint8_t* data, float value) {
  StoreU32LE(data, std::bit_cast<uint32_t>(value));
}

float LoadFloatLE(const uint8_t* data) {
  uint32_t value = data[0] | (static_cast<uint32_t>(data[1]) << 8) |
                   (static_cast<uint32_t>(data[2]) << 16) |
                   (static_cast<uint32_t>(data[3]) << 24);
  return std::bit_cast<float>(value);
}

TEST_CASE("A301 setpoint frames", "[hal][a301]") {
  HAL_Initialize();
  std::vector<SentFrame> sentFrames;
  struct SendData {
    std::vector<SentFrame>* sentFrames;
    bool failStops = false;
  } sendData{&sentFrames};
  CallbackHandle sendCallback{
      HALSIM_RegisterCanSendMessageCallback(
          [](const char*, void* param, int32_t busId, uint32_t messageId,
             const HAL_CANMessage* message, int32_t periodMs, int32_t* status) {
            auto data = static_cast<SendData*>(param);
            data->sentFrames->push_back({busId, messageId, *message, periodMs});
            if (data->failStops &&
                periodMs == HAL_CAN_SEND_PERIOD_STOP_REPEATING) {
              *status = HAL_CAN_BUFFER_OVERRUN;
            }
          },
          &sendData),
      HALSIM_CancelCanSendMessageCallback};

  int32_t status = 0;
  A301Handle a301{HAL_InitializeA301(0, 3, "A301Test", &status)};
  REQUIRE(status == 0);
  REQUIRE(a301.handle != HAL_INVALID_HANDLE);
  sentFrames.clear();

  HAL_SetA301Setpoint(a301.handle, 0.25, HAL_A301_CONTROL_TYPE_DUTY_CYCLE, 0.0,
                      &status);
  REQUIRE(status == 0);
  REQUIRE(sentFrames.size() == 1);
  CHECK(sentFrames[0].busId == 0);
  CHECK(sentFrames[0].messageId == (0x02030080u | 3));
  CHECK(sentFrames[0].periodMs == 20);
  CHECK(sentFrames[0].message.dataSize == 8);
  CHECK(LoadFloatLE(sentFrames[0].message.data) == 0.25f);
  CHECK(LoadFloatLE(sentFrames[0].message.data + 4) == 0.0f);

  sentFrames.clear();
  HAL_SetA301Setpoint(a301.handle, 125.0, HAL_A301_CONTROL_TYPE_VELOCITY, 0.0,
                      &status);
  REQUIRE(status == 0);
  REQUIRE(sentFrames.size() == 2);
  CHECK(sentFrames[0].messageId == (0x02030080u | 3));
  CHECK(sentFrames[0].periodMs == HAL_CAN_SEND_PERIOD_STOP_REPEATING);
  CHECK(sentFrames[1].messageId == (0x02030000u | 3));
  CHECK(sentFrames[1].periodMs == 20);
  CHECK(LoadFloatLE(sentFrames[1].message.data) == 125.0f);

  sendData.failStops = true;
  sentFrames.clear();
  HAL_SetA301Setpoint(a301.handle, 0.25,
                      HAL_A301_CONTROL_TYPE_ABSOLUTE_POSITION, 0.0, &status);
  CHECK(status == HAL_CAN_BUFFER_OVERRUN);
  REQUIRE(sentFrames.size() == 1);
  CHECK(sentFrames[0].messageId == (0x02030000u | 3));
  CHECK(sentFrames[0].periodMs == HAL_CAN_SEND_PERIOD_STOP_REPEATING);

  sendData.failStops = false;
  sentFrames.clear();
  status = 0;
  HAL_SetA301Setpoint(a301.handle, 0.25,
                      HAL_A301_CONTROL_TYPE_ABSOLUTE_POSITION, 0.0, &status);
  REQUIRE(status == 0);
  REQUIRE(sentFrames.size() == 2);
  CHECK(sentFrames[0].messageId == (0x02030000u | 3));
  CHECK(sentFrames[0].periodMs == HAL_CAN_SEND_PERIOD_STOP_REPEATING);
  CHECK(sentFrames[1].messageId == (0x02030200u | 3));
  CHECK(sentFrames[1].periodMs == 20);

  sentFrames.clear();
  HAL_SetA301Inverted(a301.handle, true, &status);
  HAL_SetA301Setpoint(a301.handle, 6.0, HAL_A301_CONTROL_TYPE_VOLTAGE, 0.0,
                      &status);
  REQUIRE(status == 0);
  REQUIRE_FALSE(sentFrames.empty());
  CHECK(LoadFloatLE(sentFrames.back().message.data) == -0.5f);

  sentFrames.clear();
  HAL_SetA301Setpoint(a301.handle, 3.0, HAL_A301_CONTROL_TYPE_RELATIVE_POSITION,
                      0.0, &status);
  REQUIRE(status == 0);
  REQUIRE(sentFrames.size() == 2);
  CHECK(sentFrames.back().messageId == (0x02030100u | 3));
  CHECK(LoadFloatLE(sentFrames.back().message.data) == 3.0f);

  sentFrames.clear();
  HAL_SetA301Setpoint(a301.handle, 0.25,
                      HAL_A301_CONTROL_TYPE_ABSOLUTE_POSITION, 0.0, &status);
  REQUIRE(status == 0);
  REQUIRE(sentFrames.size() == 2);
  CHECK(sentFrames.back().messageId == (0x02030200u | 3));
  CHECK(LoadFloatLE(sentFrames.back().message.data) == 0.25f);

  sentFrames.clear();
  HAL_SetA301RelativeEncoderPosition(a301.handle, 3.0, &status);
  REQUIRE(status == 0);
  REQUIRE(sentFrames.size() == 1);
  CHECK(sentFrames[0].messageId == (0x02032800u | 3));
  CHECK(LoadFloatLE(sentFrames[0].message.data) == 3.0f);

  sentFrames.clear();
  HAL_SetA301AbsoluteEncoderPosition(a301.handle, 0.25, &status);
  REQUIRE(status == 0);
  REQUIRE(sentFrames.size() == 1);
  CHECK(sentFrames[0].messageId == (0x02032880u | 3));
  CHECK(LoadFloatLE(sentFrames[0].message.data) == 0.25f);
}

TEST_CASE("A301 periodic status decoding", "[hal][a301]") {
  HAL_Initialize();
  std::array<uint8_t, 8> status0{};
  int16_t appliedOutput = 16222;
  status0[0] = appliedOutput;
  status0[1] = appliedOutput >> 8;
  uint16_t voltage = 1638;
  uint16_t current = 273;
  status0[2] = voltage;
  status0[3] = (voltage >> 8) | ((current & 0x0f) << 4);
  status0[4] = current >> 4;
  status0[5] = 42;
  status0[6] = 1 | (HAL_A301_GEARBOX_RPM_500 << 1);

  std::array<uint8_t, 8> status1{0x81, 0xc1, 0x81, 0x42,
                                 0x41, 0x42, 0x01, 0x00};
  std::array<uint8_t, 8> status2{};
  StoreFloatLE(status2.data(), 123.5f);
  StoreFloatLE(status2.data() + 4, 4.25f);
  std::array<uint8_t, 8> status3{};
  StoreFloatLE(status3.data() + 4, -0.25f);

  struct ReceiveData {
    std::array<uint8_t, 8>* status0;
    std::array<uint8_t, 8>* status1;
    std::array<uint8_t, 8>* status2;
    std::array<uint8_t, 8>* status3;
  } receiveData{&status0, &status1, &status2, &status3};

  CallbackHandle receiveCallback{
      HALSIM_RegisterCanReceiveMessageCallback(
          [](const char*, void* param, int32_t, uint32_t messageId,
             HAL_CANReceiveMessage* message, int32_t* status) {
            auto data = static_cast<ReceiveData*>(param);
            std::array<uint8_t, 8>* source = nullptr;
            switch (messageId & ~0x3fu) {
              case 0x0203b800:
                source = data->status0;
                break;
              case 0x0203b840:
                source = data->status1;
                break;
              case 0x0203b880:
                source = data->status2;
                break;
              case 0x0203b8c0:
                source = data->status3;
                break;
              default:
                return;
            }
            message->timeStamp = wpi::util::Now();
            message->message.dataSize = source->size();
            std::memcpy(message->message.data, source->data(), source->size());
            *status = 0;
          },
          &receiveData),
      HALSIM_CancelCanReceiveMessageCallback};

  int32_t status = 0;
  A301Handle a301{HAL_InitializeA301(0, 4, "A301Test", &status)};
  REQUIRE(status == 0);

  HAL_A301PeriodicStatus0 frame0{};
  HAL_GetA301PeriodicStatus0(a301.handle, &frame0, &status);
  REQUIRE(status == 0);
  CHECK(frame0.appliedOutput == Catch::Approx(0.5).margin(0.001));
  CHECK(frame0.voltage == Catch::Approx(12.0).margin(0.01));
  CHECK(frame0.current == Catch::Approx(10.0).margin(0.02));
  CHECK(frame0.motorTemperature == 42);
  CHECK(frame0.primaryHeartbeatLock);
  CHECK(frame0.gearboxRPM == HAL_A301_GEARBOX_RPM_500);

  HAL_A301PeriodicStatus1 frame1{};
  HAL_GetA301PeriodicStatus1(a301.handle, &frame1, &status);
  REQUIRE(status == 0);
  CHECK(frame1.otherFault);
  CHECK(frame1.firmwareFault);
  CHECK(frame1.motorStartupFault);
  CHECK(frame1.overvoltageWarning);
  CHECK(frame1.motorLoopSpeedWarning);
  CHECK(frame1.brownoutWarning);
  CHECK(frame1.otherWarning);
  CHECK(frame1.motorTypeStickyFault);
  CHECK(frame1.overvoltageStickyWarning);
  CHECK(frame1.overcurrentStickyWarning);
  CHECK(frame1.hasResetStickyWarning);
  CHECK(frame1.isFollower);

  HAL_SetA301Inverted(a301.handle, true, &status);
  HAL_A301PeriodicStatus2 frame2{};
  HAL_GetA301PeriodicStatus2(a301.handle, &frame2, &status);
  REQUIRE(status == 0);
  CHECK(frame2.encoderVelocity == -123.5);
  CHECK(frame2.relativeEncoderPosition == 4.25);

  HAL_A301PeriodicStatus3 frame3{};
  HAL_GetA301PeriodicStatus3(a301.handle, &frame3, &status);
  REQUIRE(status == 0);
  CHECK(frame3.absoluteEncoderPosition == -0.25);
}

TEST_CASE("A301 configuration request and response frames", "[hal][a301]") {
  HAL_Initialize();
  std::vector<SentFrame> sentFrames;
  float rangeOffset = 0.125f;
  CallbackHandle sendCallback{
      HALSIM_RegisterCanSendMessageCallback(
          [](const char*, void* param, int32_t busId, uint32_t messageId,
             const HAL_CANMessage* message, int32_t periodMs, int32_t*) {
            static_cast<std::vector<SentFrame>*>(param)->push_back(
                {busId, messageId, *message, periodMs});
          },
          &sentFrames),
      HALSIM_CancelCanSendMessageCallback};
  CallbackHandle receiveCallback{
      HALSIM_RegisterCanReceiveMessageCallback(
          [](const char*, void* param, int32_t, uint32_t messageId,
             HAL_CANReceiveMessage* message, int32_t* status) {
            std::array<uint8_t, 8> data{};
            uint8_t dataSize = 0;
            uint64_t timestamp = wpi::util::Now();
            switch (messageId & ~0x3fu) {
              case 0x02032600:
                data = {27, 1, 0x12, 0x34, 11, 2, 0, 0};
                dataSize = 8;
                break;
              case 0x02035140:
                data[0] = HAL_A301_IDLE_MODE_BRAKE;
                dataSize = 1;
                break;
              case 0x020351c0:
                data[0] = 1;
                dataSize = 1;
                break;
              case 0x02035240:
                StoreFloatLE(data.data(), *static_cast<float*>(param));
                dataSize = 4;
                break;
              case 0x02034440:
                data[0] = 0;
                data[1] = HAL_A301_STATUS_2;
                StoreU32LE(data.data() + 2, 1000);
                dataSize = 6;
                break;
              case 0x02036040:
                data[0] = HAL_A301_STATUS_2;
                StoreU32LE(data.data() + 1, 125);
                dataSize = 5;
                break;
              case 0x0203b880:
                dataSize = 8;
                timestamp -= 600'000;
                break;
              default:
                return;
            }
            message->timeStamp = timestamp;
            message->message.dataSize = dataSize;
            std::memcpy(message->message.data, data.data(), dataSize);
            *status = 0;
          },
          &rangeOffset),
      HALSIM_CancelCanReceiveMessageCallback};

  int32_t status = 0;
  A301Handle a301{HAL_InitializeA301(0, 5, "A301Test", &status)};
  REQUIRE(status == 0);
  sentFrames.clear();

  HAL_A301FirmwareVersion version{};
  HAL_GetA301FirmwareVersion(a301.handle, &version, &status);
  REQUIRE(status == 0);
  CHECK(version.major == 27);
  CHECK(version.minor == 1);
  CHECK(version.patch == 0x1234);
  CHECK(version.prerelease == 11);
  CHECK(version.hardwareRevision == 2);
  CHECK(version.raw == 0x1b011234);
  REQUIRE(sentFrames.size() == 1);
  CHECK(sentFrames[0].messageId ==
        ((0x02032600u | 5) | HAL_CAN_IS_FRAME_REMOTE));
  CHECK(sentFrames[0].message.dataSize == 8);

  sentFrames.clear();
  CHECK(HAL_GetA301IdleMode(a301.handle, &status) == HAL_A301_IDLE_MODE_BRAKE);
  REQUIRE(status == 0);
  CHECK(HAL_GetA301AbsolutePositionContinuousInput(a301.handle, &status));
  REQUIRE(status == 0);
  CHECK(HAL_GetA301AbsoluteEncoderRangeOffset(a301.handle, &status) == 0.125);
  REQUIRE(status == 0);
  REQUIRE(sentFrames.size() == 3);
  CHECK(sentFrames[0].messageId == (0x02035100u | 5));
  CHECK(sentFrames[1].messageId == (0x02035180u | 5));
  CHECK(sentFrames[2].messageId == (0x02035200u | 5));

  HAL_SetA301Inverted(a301.handle, true, &status);
  REQUIRE(status == 0);
  sentFrames.clear();
  HAL_SetA301AbsoluteEncoderRangeOffset(a301.handle, 0.25, &status);
  REQUIRE(status == 0);
  REQUIRE(sentFrames.size() == 1);
  CHECK(sentFrames[0].messageId == (0x02032980u | 5));
  rangeOffset = LoadFloatLE(sentFrames[0].message.data);
  CHECK(rangeOffset == -0.25f);
  CHECK(HAL_GetA301AbsoluteEncoderRangeOffset(a301.handle, &status) == 0.25);
  REQUIRE(status == 0);

  sentFrames.clear();
  HAL_SetA301StatusFramePeriod(a301.handle, HAL_A301_STATUS_2, 250, &status);
  REQUIRE(status == 0);
  REQUIRE(sentFrames.size() == 1);
  CHECK(sentFrames[0].messageId == (0x02034400u | 5));
  CHECK(sentFrames[0].message.dataSize == 5);
  CHECK(sentFrames[0].message.data[0] == HAL_A301_STATUS_2);
  CHECK(sentFrames[0].message.data[1] == 250);

  HAL_A301PeriodicStatus2 frame{};
  HAL_GetA301PeriodicStatus2(a301.handle, &frame, &status);
  CHECK(status == 0);

  sentFrames.clear();
  CHECK(HAL_GetA301StatusFramePeriod(a301.handle, HAL_A301_STATUS_2, &status) ==
        125);
  REQUIRE(status == 0);
  REQUIRE(sentFrames.size() == 1);
  CHECK(sentFrames[0].messageId == (0x02036000u | 5));
  CHECK(sentFrames[0].message.dataSize == 1);
  CHECK(sentFrames[0].message.data[0] == HAL_A301_STATUS_2);
}

TEST_CASE("A301 device ID detection", "[hal][a301]") {
  HAL_Initialize();
  struct DetectionData {
    int32_t busId = -1;
    uint32_t messageId = 0;
    uint32_t messageIdMask = 0;
  } detection;
  CallbackHandle openCallback{
      HALSIM_RegisterCanOpenStreamCallback(
          [](const char*, void* param, HAL_CANStreamHandle* streamHandle,
             int32_t busId, uint32_t messageId, uint32_t messageIdMask,
             uint32_t, int32_t* status) {
            auto data = static_cast<DetectionData*>(param);
            data->busId = busId;
            data->messageId = messageId;
            data->messageIdMask = messageIdMask;
            *streamHandle = 301;
            *status = 0;
          },
          &detection),
      HALSIM_CancelCanOpenStreamCallback};
  CallbackHandle readCallback{
      HALSIM_RegisterCanReadStreamCallback(
          [](const char*, void*, HAL_CANStreamHandle streamHandle,
             HAL_CANStreamMessage* messages, uint32_t, uint32_t* messagesRead,
             int32_t* status) {
            CHECK(streamHandle == 301);
            messages[0].messageId = 0x0203b800u | 17;
            *messagesRead = 1;
            *status = HAL_ERR_CANSessionMux_SessionOverrun;
          },
          nullptr),
      HALSIM_CancelCanReadStreamCallback};

  int32_t status = 0;
  CHECK(HAL_DetectA301DeviceId(5, &status) == 17);
  REQUIRE(status == 0);
  CHECK(detection.busId == 5);
  CHECK(detection.messageId == 0x0203b800);
  CHECK(detection.messageIdMask == 0x1fffffc0);
}

TEST_CASE("A301 allocation is released by a global handle reset",
          "[hal][a301]") {
  HAL_Initialize();
  std::vector<SentFrame> sentFrames;
  CallbackHandle sendCallback{
      HALSIM_RegisterCanSendMessageCallback(
          [](const char*, void* param, int32_t busId, uint32_t messageId,
             const HAL_CANMessage* message, int32_t periodMs, int32_t*) {
            static_cast<std::vector<SentFrame>*>(param)->push_back(
                {busId, messageId, *message, periodMs});
          },
          &sentFrames),
      HALSIM_CancelCanSendMessageCallback};

  int32_t status = 0;
  HAL_A301Handle first = HAL_InitializeA301(0, 6, "A301Test", &status);
  REQUIRE(status == 0);
  REQUIRE(first != HAL_INVALID_HANDLE);

  sentFrames.clear();
  wpi::hal::HandleBase::ResetGlobalHandles();
  constexpr std::array<uint32_t, 5> expectedStopIds{
      0x02030080u | 6, 0x02030000u | 6, 0x02030100u | 6, 0x02030180u | 6,
      0x02030200u | 6};
  REQUIRE(sentFrames.size() == expectedStopIds.size());
  for (uint32_t expectedId : expectedStopIds) {
    CHECK(std::count_if(sentFrames.begin(), sentFrames.end(),
                        [expectedId](const SentFrame& frame) {
                          return frame.messageId == expectedId &&
                                 frame.periodMs ==
                                     HAL_CAN_SEND_PERIOD_STOP_REPEATING;
                        }) == 1);
  }

  A301Handle second{HAL_InitializeA301(0, 6, "A301Test", &status)};
  REQUIRE(status == 0);
  CHECK(second.handle != HAL_INVALID_HANDLE);
}

}  // namespace
