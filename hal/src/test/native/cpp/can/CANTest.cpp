// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <utility>

#include <gtest/gtest.h>

#include "hal/CANAPI.h"
#include "hal/simulation/CanData.h"

namespace hal {
struct CANTestStore {
  CANTestStore(int32_t deviceId, int32_t* status) {
    this->deviceId = deviceId;
    handle = HAL_InitializeCAN(
        HAL_CANManufacturer::HAL_CAN_Man_kTeamUse, deviceId,
        HAL_CANDeviceType::HAL_CAN_Dev_kMiscellaneous, status);
  }

  ~CANTestStore() {
    if (handle != HAL_kInvalidHandle) {
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

TEST(CANTest, CanIdPacking) {
  int32_t status = 0;
  int32_t deviceId = 12;
  CANTestStore testStore(deviceId, &status);
  ASSERT_EQ(0, status);

  std::pair<int32_t, bool> storePair;
  storePair.second = false;

  auto cbHandle = HALSIM_RegisterCanSendMessageCallback(
      [](const char* name, void* param, uint32_t messageID, const uint8_t* data,
         uint8_t dataSize, int32_t periodMs, int32_t* status) {
        std::pair<int32_t, bool>* paramI =
            reinterpret_cast<std::pair<int32_t, bool>*>(param);
        paramI->first = messageID;
        paramI->second = true;
      },
      &storePair);

  CANSendCallbackStore cbStore(cbHandle);
  uint8_t data[8];

  int32_t apiId = 42;

  HAL_WriteCANPacket(testStore.handle, data, 8, 42, &status);

  ASSERT_EQ(0, status);

  ASSERT_TRUE(storePair.second);

  ASSERT_NE(0, storePair.first);

  ASSERT_EQ(deviceId, storePair.first & 0x3F);
  ASSERT_EQ(apiId, (storePair.first & 0x0000FFC0) >> 6);
  ASSERT_EQ(static_cast<int32_t>(HAL_CANManufacturer::HAL_CAN_Man_kTeamUse),
            (storePair.first & 0x00FF0000) >> 16);
  ASSERT_EQ(static_cast<int32_t>(HAL_CANDeviceType::HAL_CAN_Dev_kMiscellaneous),
            (storePair.first & 0x1F000000) >> 24);
}
}  // namespace hal
