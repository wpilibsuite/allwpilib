/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "hal/CANAPI.h"

#include <atomic>
#include <ctime>

#include <wpi/DenseMap.h>

#include "HALInitializer.h"
#include "hal/CAN.h"
#include "hal/Errors.h"
#include "hal/HAL.h"
#include "hal/handles/UnlimitedHandleResource.h"

using namespace hal;

namespace {
struct Receives {
  uint32_t lastTimeStamp;
  uint8_t data[8];
  uint8_t length;
};

struct CANStorage {
  HAL_CANManufacturer manufacturer;
  HAL_CANDeviceType deviceType;
  uint8_t deviceId;
  wpi::mutex mapMutex;
  wpi::SmallDenseMap<int32_t, int32_t> periodicSends;
  wpi::SmallDenseMap<int32_t, Receives> receives;
};
}  // namespace

static UnlimitedHandleResource<HAL_CANHandle, CANStorage, HAL_HandleEnum::CAN>*
    canHandles;

static uint32_t GetPacketBaseTime() {
  timespec t;
  clock_gettime(CLOCK_MONOTONIC, &t);

  // Convert t to milliseconds
  uint64_t ms = t.tv_sec * 1000ull + t.tv_nsec / 1000000ull;
  return ms & 0xFFFFFFFF;
}

namespace hal {
namespace init {
void InitializeCANAPI() {
  static UnlimitedHandleResource<HAL_CANHandle, CANStorage, HAL_HandleEnum::CAN>
      cH;
  canHandles = &cH;
}
}  // namespace init
}  // namespace hal

static int32_t CreateCANId(CANStorage* storage, int32_t apiId) {
  int32_t createdId = 0;
  createdId |= (static_cast<int32_t>(storage->deviceType) & 0x1F) << 24;
  createdId |= (static_cast<int32_t>(storage->manufacturer) & 0xFF) << 16;
  createdId |= (apiId & 0x3FF) << 6;
  createdId |= (storage->deviceId & 0x3F);
  return createdId;
}

HAL_CANHandle HAL_InitializeCAN(HAL_CANManufacturer manufacturer,
                                int32_t deviceId, HAL_CANDeviceType deviceType,
                                int32_t* status) {
  hal::init::CheckInit();
  auto can = std::make_shared<CANStorage>();

  auto handle = canHandles->Allocate(can);

  if (handle == HAL_kInvalidHandle) {
    *status = NO_AVAILABLE_RESOURCES;
    return HAL_kInvalidHandle;
  }

  can->deviceId = deviceId;
  can->deviceType = deviceType;
  can->manufacturer = manufacturer;

  return handle;
}

void HAL_CleanCAN(HAL_CANHandle handle) {
  auto data = canHandles->Free(handle);

  std::lock_guard<wpi::mutex> lock(data->mapMutex);

  for (auto&& i : data->periodicSends) {
    int32_t s = 0;
    HAL_CAN_SendMessage(i.first, nullptr, 0, HAL_CAN_SEND_PERIOD_STOP_REPEATING,
                        &s);
    i.second = -1;
  }
}

void HAL_WriteCANPacket(HAL_CANHandle handle, const uint8_t* data,
                        int32_t length, int32_t apiId, int32_t* status) {
  auto can = canHandles->Get(handle);
  if (!can) {
    *status = HAL_HANDLE_ERROR;
    return;
  }
  auto id = CreateCANId(can.get(), apiId);

  HAL_CAN_SendMessage(id, data, length, HAL_CAN_SEND_PERIOD_NO_REPEAT, status);

  if (*status != 0) {
    return;
  }
  std::lock_guard<wpi::mutex> lock(can->mapMutex);
  can->periodicSends[apiId] = -1;
}

void HAL_WriteCANPacketRepeating(HAL_CANHandle handle, const uint8_t* data,
                                 int32_t length, int32_t apiId,
                                 int32_t repeatMs, int32_t* status) {
  auto can = canHandles->Get(handle);
  if (!can) {
    *status = HAL_HANDLE_ERROR;
    return;
  }
  auto id = CreateCANId(can.get(), apiId);

  HAL_CAN_SendMessage(id, data, length, repeatMs, status);

  if (*status != 0) {
    return;
  }
  std::lock_guard<wpi::mutex> lock(can->mapMutex);
  can->periodicSends[apiId] = repeatMs;
}

void HAL_StopCANPacketRepeating(HAL_CANHandle handle, int32_t apiId,
                                int32_t* status) {
  auto can = canHandles->Get(handle);
  if (!can) {
    *status = HAL_HANDLE_ERROR;
    return;
  }
  auto id = CreateCANId(can.get(), apiId);

  HAL_CAN_SendMessage(id, nullptr, 0, HAL_CAN_SEND_PERIOD_STOP_REPEATING,
                      status);

  if (*status != 0) {
    return;
  }
  std::lock_guard<wpi::mutex> lock(can->mapMutex);
  can->periodicSends[apiId] = -1;
}

void HAL_ReadCANPacketNew(HAL_CANHandle handle, int32_t apiId, uint8_t* data,
                          int32_t* length, uint64_t* receivedTimestamp,
                          int32_t* status) {
  auto can = canHandles->Get(handle);
  if (!can) {
    *status = HAL_HANDLE_ERROR;
    return;
  }

  uint32_t messageId = CreateCANId(can.get(), apiId);
  uint8_t dataSize = 0;
  uint32_t ts = 0;
  HAL_CAN_ReceiveMessage(&messageId, 0x1FFFFFFF, data, &dataSize, &ts, status);

  if (*status == 0) {
    std::lock_guard<wpi::mutex> lock(can->mapMutex);
    auto& msg = can->receives[messageId];
    msg.length = dataSize;
    msg.lastTimeStamp = ts;
    // The NetComm call placed in data, copy into the msg
    std::memcpy(msg.data, data, dataSize);
  }
  *length = dataSize;
  *receivedTimestamp = ts;
}

void HAL_ReadCANPacketLatest(HAL_CANHandle handle, int32_t apiId, uint8_t* data,
                             int32_t* length, uint64_t* receivedTimestamp,
                             int32_t* status) {
  auto can = canHandles->Get(handle);
  if (!can) {
    *status = HAL_HANDLE_ERROR;
    return;
  }

  uint32_t messageId = CreateCANId(can.get(), apiId);
  uint8_t dataSize = 0;
  uint32_t ts = 0;
  HAL_CAN_ReceiveMessage(&messageId, 0x1FFFFFFF, data, &dataSize, &ts, status);

  std::lock_guard<wpi::mutex> lock(can->mapMutex);
  if (*status == 0) {
    // fresh update
    auto& msg = can->receives[messageId];
    msg.length = dataSize;
    *length = dataSize;
    msg.lastTimeStamp = ts;
    *receivedTimestamp = ts;
    // The NetComm call placed in data, copy into the msg
    std::memcpy(msg.data, data, dataSize);
  } else {
    auto i = can->receives.find(messageId);
    if (i != can->receives.end()) {
      // Read the data from the stored message into the output
      std::memcpy(data, i->second.data, i->second.length);
      *length = i->second.length;
      *receivedTimestamp = i->second.lastTimeStamp;
      *status = 0;
    }
  }
}

void HAL_ReadCANPacketTimeout(HAL_CANHandle handle, int32_t apiId,
                              uint8_t* data, int32_t* length,
                              uint64_t* receivedTimestamp, int32_t timeoutMs,
                              int32_t* status) {
  auto can = canHandles->Get(handle);
  if (!can) {
    *status = HAL_HANDLE_ERROR;
    return;
  }

  uint32_t messageId = CreateCANId(can.get(), apiId);
  uint8_t dataSize = 0;
  uint32_t ts = 0;
  HAL_CAN_ReceiveMessage(&messageId, 0x1FFFFFFF, data, &dataSize, &ts, status);

  std::lock_guard<wpi::mutex> lock(can->mapMutex);
  if (*status == 0) {
    // fresh update
    auto& msg = can->receives[messageId];
    msg.length = dataSize;
    *length = dataSize;
    msg.lastTimeStamp = ts;
    *receivedTimestamp = ts;
    // The NetComm call placed in data, copy into the msg
    std::memcpy(msg.data, data, dataSize);
  } else {
    auto i = can->receives.find(messageId);
    if (i != can->receives.end()) {
      // Found, check if new enough
      uint32_t now = GetPacketBaseTime();
      if (now - i->second.lastTimeStamp > static_cast<uint32_t>(timeoutMs)) {
        // Timeout, return bad status
        *status = HAL_CAN_TIMEOUT;
        return;
      }
      // Read the data from the stored message into the output
      std::memcpy(data, i->second.data, i->second.length);
      *length = i->second.length;
      *receivedTimestamp = i->second.lastTimeStamp;
      *status = 0;
    }
  }
}

void HAL_ReadCANPeriodicPacket(HAL_CANHandle handle, int32_t apiId,
                               uint8_t* data, int32_t* length,
                               uint64_t* receivedTimestamp, int32_t timeoutMs,
                               int32_t periodMs, int32_t* status) {
  auto can = canHandles->Get(handle);
  if (!can) {
    *status = HAL_HANDLE_ERROR;
    return;
  }

  uint32_t messageId = CreateCANId(can.get(), apiId);

  {
    std::lock_guard<wpi::mutex> lock(can->mapMutex);
    auto i = can->receives.find(messageId);
    if (i != can->receives.end()) {
      // Found, check if new enough
      uint32_t now = GetPacketBaseTime();
      if (now - i->second.lastTimeStamp < static_cast<uint32_t>(periodMs)) {
        // Read the data from the stored message into the output
        std::memcpy(data, i->second.data, i->second.length);
        *length = i->second.length;
        *receivedTimestamp = i->second.lastTimeStamp;
        *status = 0;
        return;
      }
    }
  }

  uint8_t dataSize = 0;
  uint32_t ts = 0;
  HAL_CAN_ReceiveMessage(&messageId, 0x1FFFFFFF, data, &dataSize, &ts, status);

  std::lock_guard<wpi::mutex> lock(can->mapMutex);
  if (*status == 0) {
    // fresh update
    auto& msg = can->receives[messageId];
    msg.length = dataSize;
    *length = dataSize;
    msg.lastTimeStamp = ts;
    *receivedTimestamp = ts;
    // The NetComm call placed in data, copy into the msg
    std::memcpy(msg.data, data, dataSize);
  } else {
    auto i = can->receives.find(messageId);
    if (i != can->receives.end()) {
      // Found, check if new enough
      uint32_t now = GetPacketBaseTime();
      if (now - i->second.lastTimeStamp > static_cast<uint32_t>(timeoutMs)) {
        // Timeout, return bad status
        *status = HAL_CAN_TIMEOUT;
        return;
      }
      // Read the data from the stored message into the output
      std::memcpy(data, i->second.data, i->second.length);
      *length = i->second.length;
      *receivedTimestamp = i->second.lastTimeStamp;
      *status = 0;
    }
  }
}
