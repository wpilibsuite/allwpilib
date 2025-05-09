// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "hal/CANAPI.h"

#include <ctime>
#include <memory>

#include <wpi/DenseMap.h>
#include <wpi/mutex.h>
#include <wpi/timestamp.h>

#include "HALInitializer.h"
#include "PortsInternal.h"
#include "hal/CAN.h"
#include "hal/Errors.h"
#include "hal/handles/UnlimitedHandleResource.h"

using namespace hal;

namespace {
struct CANStorage {
  HAL_CANManufacturer manufacturer;
  HAL_CANDeviceType deviceType;
  int32_t busId;
  uint8_t deviceId;
  wpi::mutex periodicSendsMutex;
  wpi::SmallDenseMap<int32_t, int32_t> periodicSends;
  wpi::mutex receivesMutex;
  wpi::SmallDenseMap<int32_t, HAL_CANReceiveMessage> receives;
};
}  // namespace

static UnlimitedHandleResource<HAL_CANHandle, CANStorage, HAL_HandleEnum::CAN>*
    canHandles;

namespace hal::init {
void InitializeCANAPI() {
  static UnlimitedHandleResource<HAL_CANHandle, CANStorage, HAL_HandleEnum::CAN>
      cH;
  canHandles = &cH;
}
}  // namespace hal::init

static int32_t CreateCANId(CANStorage* storage, int32_t apiId) {
  int32_t createdId = 0;
  createdId |= (static_cast<int32_t>(storage->deviceType) & 0x1F) << 24;
  createdId |= (static_cast<int32_t>(storage->manufacturer) & 0xFF) << 16;
  createdId |= (apiId & 0x3FF) << 6;
  createdId |= (storage->deviceId & 0x3F);
  return createdId;
}

extern "C" {

HAL_CANHandle HAL_InitializeCAN(int32_t busId, HAL_CANManufacturer manufacturer,
                                int32_t deviceId, HAL_CANDeviceType deviceType,
                                int32_t* status) {
  hal::init::CheckInit();

  if (busId < 0 || busId > hal::NUM_CAN_BUSES) {
    *status = PARAMETER_OUT_OF_RANGE;
    return HAL_InvalidHandle;
  }

  auto can = std::make_shared<CANStorage>();

  auto handle = canHandles->Allocate(can);

  if (handle == HAL_InvalidHandle) {
    *status = NO_AVAILABLE_RESOURCES;
    return HAL_InvalidHandle;
  }

  can->busId = busId;
  can->deviceId = deviceId;
  can->deviceType = deviceType;
  can->manufacturer = manufacturer;

  return handle;
}

void HAL_CleanCAN(HAL_CANHandle handle) {
  auto data = canHandles->Free(handle);
  if (data == nullptr) {
    return;
  }

  std::scoped_lock lock(data->periodicSendsMutex);

  for (auto&& i : data->periodicSends) {
    int32_t s = 0;
    auto id = CreateCANId(data.get(), i.first);
    HAL_CANMessage message;
    std::memset(&message, 0, sizeof(message));
    HAL_CAN_SendMessage(data->busId, id, &message,
                        HAL_CAN_SEND_PERIOD_STOP_REPEATING, &s);
    i.second = -1;
  }
}

void HAL_WriteCANPacket(HAL_CANHandle handle, int32_t apiId,
                        const struct HAL_CANMessage* message, int32_t* status) {
  auto can = canHandles->Get(handle);
  if (!can) {
    *status = HAL_HANDLE_ERROR;
    return;
  }
  auto id = CreateCANId(can.get(), apiId);

  std::scoped_lock lock(can->periodicSendsMutex);
  HAL_CAN_SendMessage(can->busId, id, message, HAL_CAN_SEND_PERIOD_NO_REPEAT,
                      status);
  can->periodicSends[apiId] = -1;
}

void HAL_WriteCANPacketRepeating(HAL_CANHandle handle, int32_t apiId,
                                 const struct HAL_CANMessage* message,
                                 int32_t repeatMs, int32_t* status) {
  auto can = canHandles->Get(handle);
  if (!can) {
    *status = HAL_HANDLE_ERROR;
    return;
  }
  auto id = CreateCANId(can.get(), apiId);

  std::scoped_lock lock(can->periodicSendsMutex);
  HAL_CAN_SendMessage(can->busId, id, message, repeatMs, status);
  can->periodicSends[apiId] = repeatMs;
}

void HAL_WriteCANRTRFrame(HAL_CANHandle handle, int32_t apiId,
                          const struct HAL_CANMessage* message,
                          int32_t* status) {
  auto can = canHandles->Get(handle);
  if (!can) {
    *status = HAL_HANDLE_ERROR;
    return;
  }
  auto id = CreateCANId(can.get(), apiId);
  id |= HAL_CAN_IS_FRAME_REMOTE;

  std::scoped_lock lock(can->periodicSendsMutex);
  HAL_CAN_SendMessage(can->busId, id, message, HAL_CAN_SEND_PERIOD_NO_REPEAT,
                      status);
  can->periodicSends[apiId] = -1;
}

void HAL_StopCANPacketRepeating(HAL_CANHandle handle, int32_t apiId,
                                int32_t* status) {
  auto can = canHandles->Get(handle);
  if (!can) {
    *status = HAL_HANDLE_ERROR;
    return;
  }
  auto id = CreateCANId(can.get(), apiId);

  HAL_CANMessage message;
  std::memset(&message, 0, sizeof(message));

  std::scoped_lock lock(can->periodicSendsMutex);
  HAL_CAN_SendMessage(can->busId, id, &message,
                      HAL_CAN_SEND_PERIOD_STOP_REPEATING, status);
  can->periodicSends[apiId] = -1;
}

void HAL_ReadCANPacketNew(HAL_CANHandle handle, int32_t apiId,
                          struct HAL_CANReceiveMessage* message,
                          int32_t* status) {
  auto can = canHandles->Get(handle);
  if (!can) {
    *status = HAL_HANDLE_ERROR;
    return;
  }

  uint32_t messageId = CreateCANId(can.get(), apiId);

  HAL_CAN_ReceiveMessage(can->busId, messageId, message, status);

  if (*status == 0) {
    std::scoped_lock lock(can->receivesMutex);
    can->receives[messageId] = *message;
  }
}

void HAL_ReadCANPacketLatest(HAL_CANHandle handle, int32_t apiId,
                             struct HAL_CANReceiveMessage* message,
                             int32_t* status) {
  auto can = canHandles->Get(handle);
  if (!can) {
    *status = HAL_HANDLE_ERROR;
    return;
  }

  uint32_t messageId = CreateCANId(can.get(), apiId);

  HAL_CAN_ReceiveMessage(can->busId, messageId, message, status);

  std::scoped_lock lock(can->receivesMutex);
  if (*status == 0) {
    // fresh update
    can->receives[messageId] = *message;
  } else {
    auto i = can->receives.find(messageId);
    if (i != can->receives.end()) {
      // Read the data from the stored message into the output
      *message = i->second;
      *status = 0;
    }
  }
}

void HAL_ReadCANPacketTimeout(HAL_CANHandle handle, int32_t apiId,
                              struct HAL_CANReceiveMessage* message,
                              int32_t timeoutMs, int32_t* status) {
  auto can = canHandles->Get(handle);
  if (!can) {
    *status = HAL_HANDLE_ERROR;
    return;
  }

  uint32_t messageId = CreateCANId(can.get(), apiId);

  HAL_CAN_ReceiveMessage(can->busId, messageId, message, status);

  std::scoped_lock lock(can->receivesMutex);
  if (*status == 0) {
    // fresh update
    can->receives[messageId] = *message;
  } else {
    auto i = can->receives.find(messageId);
    if (i != can->receives.end()) {
      // Found, check if new enough
      uint64_t now = wpi::Now();
      if (now - i->second.timeStamp >
          (static_cast<uint64_t>(timeoutMs) * 1000)) {
        // Timeout, return bad status
        *status = HAL_CAN_TIMEOUT;
        return;
      }
      // Read the data from the stored message into the output
      *message = i->second;
      *status = 0;
    }
  }
}

uint32_t HAL_StartCANStream(HAL_CANHandle handle, int32_t apiId, int32_t depth,
                            int32_t* status) {
  auto can = canHandles->Get(handle);
  if (!can) {
    *status = HAL_HANDLE_ERROR;
    return 0;
  }

  uint32_t messageId = CreateCANId(can.get(), apiId);

  uint32_t session = HAL_CAN_OpenStreamSession(can->busId, messageId,
                                               0x1FFFFFFF, depth, status);
  return session;
}
}  // extern "C"
