// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/hal/CAN.h"

#include <cstring>
#include <utility>

#include <catch2/catch_test_macros.hpp>

#include "wpi/hal/CANAPI.h"
#include "wpi/hal/simulation/CanData.h"

namespace wpi::hal {
static constexpr int32_t kCANBusS0 = 0;

struct CANTestStore {
  CANTestStore(int32_t busId, int32_t deviceId, int32_t* status) {
    this->deviceId = deviceId;
    handle = HAL_InitializeCAN(
        busId, HAL_CANManufacturer::HAL_CAN_MAN_TEAM_USE, deviceId,
        HAL_CANDeviceType::HAL_CAN_DEV_MISCELLANEOUS, status);
  }

  ~CANTestStore() {
    if (handle != HAL_INVALID_HANDLE) {
      HAL_CleanCAN(handle);
    }
  }

  int32_t deviceId;
  HAL_CANHandle handle;
};

struct CANReceiveCallbackStore {
  explicit CANReceiveCallbackStore(int32_t handle) { this->handle = handle; }
  ~CANReceiveCallbackStore() { HALSIM_CancelCanReceiveMessageCallback(handle); }
  int32_t handle;
};

struct CANSendCallbackStore {
  explicit CANSendCallbackStore(int32_t handle) { this->handle = handle; }
  ~CANSendCallbackStore() { HALSIM_CancelCanSendMessageCallback(handle); }
  int32_t handle;
};

TEST_CASE("CANTest CanIdPacking", "[hal][can]") {
  int32_t status = 0;
  int32_t deviceId = 12;
  CANTestStore testStore(kCANBusS0, deviceId, &status);
  REQUIRE(0 == status);

  std::pair<int32_t, bool> storePair;
  storePair.second = false;

  auto cbHandle = HALSIM_RegisterCanSendMessageCallback(
      [](const char* name, void* param, int32_t busId, uint32_t messageId,
         const struct HAL_CANMessage* message, int32_t periodMs,
         int32_t* status) {
        std::pair<int32_t, bool>* paramI =
            reinterpret_cast<std::pair<int32_t, bool>*>(param);
        paramI->first = messageId;
        paramI->second = true;
      },
      &storePair);

  CANSendCallbackStore cbStore(cbHandle);
  HAL_CANMessage message;
  std::memset(&message, 0, sizeof(message));
  message.dataSize = 8;

  int32_t apiId = 42;

  HAL_WriteCANPacket(testStore.handle, apiId, &message, &status);

  REQUIRE(0 == status);

  REQUIRE(storePair.second);

  REQUIRE(0 != storePair.first);

  REQUIRE(deviceId == (storePair.first & 0x3F));
  REQUIRE(apiId == ((storePair.first & 0x0000FFC0) >> 6));
  REQUIRE(static_cast<int32_t>(HAL_CANManufacturer::HAL_CAN_MAN_TEAM_USE) ==
          (storePair.first & 0x00FF0000) >> 16);
  REQUIRE(static_cast<int32_t>(HAL_CANDeviceType::HAL_CAN_DEV_MISCELLANEOUS) ==
          (storePair.first & 0x1F000000) >> 24);
}

TEST_CASE("CANTest PeriodicSendCallback", "[hal][can]") {
  struct CallbackData {
    int callbackCount = 0;
    bool shouldSend = true;
    bool didSend = false;
    uint8_t sentValue = 0;
  } callbackData;

  auto cbHandle = HALSIM_RegisterCanSendMessageCallback(
      [](const char* name, void* param, int32_t busId, uint32_t messageId,
         const struct HAL_CANMessage* message, int32_t periodMs,
         int32_t* status) {
        auto data = static_cast<CallbackData*>(param);
        data->didSend = true;
        data->sentValue = message->data[0];
      },
      &callbackData);
  CANSendCallbackStore cbStore(cbHandle);

  HAL_CANMessage message;
  std::memset(&message, 0, sizeof(message));
  message.dataSize = 1;
  message.data[0] = 1;

  int32_t status = 0;
  HAL_CAN_SendMessageWithPeriodicCallback(
      kCANBusS0, 0x123, &message, 20,
      [](void* param, HAL_CANMessage* callbackMessage) -> HAL_Bool {
        auto data = static_cast<CallbackData*>(param);
        ++data->callbackCount;
        callbackMessage->data[0] = 42;
        return data->shouldSend;
      },
      &callbackData, &status);

  REQUIRE(status == 0);
  REQUIRE(callbackData.callbackCount == 1);
  REQUIRE(callbackData.didSend);
  REQUIRE(callbackData.sentValue == 42);

  callbackData.shouldSend = false;
  callbackData.didSend = false;
  HAL_CAN_SendMessageWithPeriodicCallback(
      kCANBusS0, 0x124, &message, 20,
      [](void* param, HAL_CANMessage* callbackMessage) -> HAL_Bool {
        auto data = static_cast<CallbackData*>(param);
        ++data->callbackCount;
        return data->shouldSend;
      },
      &callbackData, &status);

  REQUIRE(status == 0);
  REQUIRE(callbackData.callbackCount == 2);
  REQUIRE_FALSE(callbackData.didSend);
}

TEST_CASE("CANTest PeriodicSendCallbackCanRead", "[hal][can]") {
  static constexpr int32_t kReadApiId = 0x50;
  static constexpr int32_t kWriteApiId = 0x70;

  int32_t status = 0;
  CANTestStore testStore(kCANBusS0, 1, &status);
  REQUIRE(status == 0);

  auto receiveHandle = HALSIM_RegisterCanReceiveMessageCallback(
      [](const char* name, void* param, int32_t busId, uint32_t messageId,
         HAL_CANReceiveMessage* message, int32_t* status) {
        message->message.dataSize = 1;
        message->message.data[0] = 42;
        message->timeStamp = 1;
        *status = 0;
      },
      nullptr);
  CANReceiveCallbackStore receiveStore(receiveHandle);

  struct CallbackData {
    HAL_CANHandle handle;
    bool didRead = false;
    bool didSend = false;
    uint8_t sentValue = 0;
  } callbackData{testStore.handle};

  auto sendHandle = HALSIM_RegisterCanSendMessageCallback(
      [](const char* name, void* param, int32_t busId, uint32_t messageId,
         const HAL_CANMessage* message, int32_t periodMs, int32_t* status) {
        auto data = static_cast<CallbackData*>(param);
        data->didSend = true;
        data->sentValue = message->data[0];
      },
      &callbackData);
  CANSendCallbackStore sendStore(sendHandle);

  HAL_CANMessage message;
  std::memset(&message, 0, sizeof(message));
  message.dataSize = 1;

  HAL_WriteCANPacketRepeatingWithCallback(
      testStore.handle, kWriteApiId, &message, 20,
      [](void* param, HAL_CANMessage* callbackMessage) -> HAL_Bool {
        auto data = static_cast<CallbackData*>(param);
        HAL_CANReceiveMessage received;
        int32_t readStatus = 0;
        HAL_ReadCANPacketTimeout(data->handle, kReadApiId, &received, 100,
                                 &readStatus);
        if (readStatus != 0) {
          return false;
        }
        data->didRead = true;
        callbackMessage->data[0] = received.message.data[0];
        return true;
      },
      &callbackData, &status);

  REQUIRE(status == 0);
  REQUIRE(callbackData.didRead);
  REQUIRE(callbackData.didSend);
  REQUIRE(callbackData.sentValue == 42);
}
}  // namespace wpi::hal
