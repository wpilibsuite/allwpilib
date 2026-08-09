// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/hal/A301.h"

#include <algorithm>
#include <array>
#include <bit>
#include <chrono>
#include <cmath>
#include <cstring>
#include <memory>
#include <mutex>
#include <string>
#include <thread>
#include <unordered_map>

#include "HALInitializer.hpp"
#include "wpi/hal/CAN.h"
#include "wpi/hal/CANAPI.h"
#include "wpi/hal/ErrorHandling.hpp"
#include "wpi/hal/Errors.h"
#include "wpi/hal/handles/UnlimitedHandleResource.hpp"
#include "wpi/util/mutex.hpp"
#include "wpi/util/timestamp.hpp"

using namespace wpi::hal;

namespace {

constexpr uint32_t kVelocitySetpointId = 0x02030000;
constexpr uint32_t kDutyCycleSetpointId = 0x02030080;
constexpr uint32_t kRelativePositionSetpointId = 0x02030100;
constexpr uint32_t kCurrentSetpointId = 0x02030180;
constexpr uint32_t kAbsolutePositionSetpointId = 0x02030200;
constexpr uint32_t kClearFaultsId = 0x02031b80;
constexpr uint32_t kGetFirmwareVersionId = 0x02032600;
constexpr uint32_t kSetRelativeEncoderPositionId = 0x02032800;
constexpr uint32_t kSetAbsoluteEncoderPositionId = 0x02032880;
constexpr uint32_t kSetContinuousInputId = 0x02032900;
constexpr uint32_t kSetAbsoluteRangeOffsetId = 0x02032980;
constexpr uint32_t kSetIdleModeId = 0x02033d00;
constexpr uint32_t kSetStatusPeriodId = 0x02034400;
constexpr uint32_t kSetStatusPeriodResponseId = 0x02034440;
constexpr uint32_t kGetIdleModeId = 0x02035100;
constexpr uint32_t kGetIdleModeResponseId = 0x02035140;
constexpr uint32_t kGetContinuousInputId = 0x02035180;
constexpr uint32_t kGetContinuousInputResponseId = 0x020351c0;
constexpr uint32_t kGetAbsoluteRangeOffsetId = 0x02035200;
constexpr uint32_t kGetAbsoluteRangeOffsetResponseId = 0x02035240;
constexpr uint32_t kGetStatusPeriodId = 0x02036000;
constexpr uint32_t kGetStatusPeriodResponseId = 0x02036040;
constexpr uint32_t kStatus0Id = 0x0203b800;
constexpr uint32_t kStatus1Id = 0x0203b840;
constexpr uint32_t kStatus2Id = 0x0203b880;
constexpr uint32_t kStatus3Id = 0x0203b8c0;

constexpr int32_t ApiFromExtId(uint32_t id) {
  return (id >> 6) & 0x3ff;
}

constexpr int32_t kVelocitySetpointApi = ApiFromExtId(kVelocitySetpointId);
constexpr int32_t kDutyCycleSetpointApi = ApiFromExtId(kDutyCycleSetpointId);
constexpr int32_t kRelativePositionSetpointApi =
    ApiFromExtId(kRelativePositionSetpointId);
constexpr int32_t kCurrentSetpointApi = ApiFromExtId(kCurrentSetpointId);
constexpr int32_t kAbsolutePositionSetpointApi =
    ApiFromExtId(kAbsolutePositionSetpointId);
constexpr int32_t kClearFaultsApi = ApiFromExtId(kClearFaultsId);
constexpr int32_t kGetFirmwareVersionApi = ApiFromExtId(kGetFirmwareVersionId);
constexpr int32_t kSetRelativeEncoderPositionApi =
    ApiFromExtId(kSetRelativeEncoderPositionId);
constexpr int32_t kSetAbsoluteEncoderPositionApi =
    ApiFromExtId(kSetAbsoluteEncoderPositionId);
constexpr int32_t kSetContinuousInputApi = ApiFromExtId(kSetContinuousInputId);
constexpr int32_t kSetAbsoluteRangeOffsetApi =
    ApiFromExtId(kSetAbsoluteRangeOffsetId);
constexpr int32_t kSetIdleModeApi = ApiFromExtId(kSetIdleModeId);
constexpr int32_t kSetStatusPeriodApi = ApiFromExtId(kSetStatusPeriodId);
constexpr int32_t kSetStatusPeriodResponseApi =
    ApiFromExtId(kSetStatusPeriodResponseId);
constexpr int32_t kGetIdleModeApi = ApiFromExtId(kGetIdleModeId);
constexpr int32_t kGetIdleModeResponseApi =
    ApiFromExtId(kGetIdleModeResponseId);
constexpr int32_t kGetContinuousInputApi = ApiFromExtId(kGetContinuousInputId);
constexpr int32_t kGetContinuousInputResponseApi =
    ApiFromExtId(kGetContinuousInputResponseId);
constexpr int32_t kGetAbsoluteRangeOffsetApi =
    ApiFromExtId(kGetAbsoluteRangeOffsetId);
constexpr int32_t kGetAbsoluteRangeOffsetResponseApi =
    ApiFromExtId(kGetAbsoluteRangeOffsetResponseId);
constexpr int32_t kGetStatusPeriodApi = ApiFromExtId(kGetStatusPeriodId);
constexpr int32_t kGetStatusPeriodResponseApi =
    ApiFromExtId(kGetStatusPeriodResponseId);
constexpr int32_t kStatus0Api = ApiFromExtId(kStatus0Id);
constexpr int32_t kStatus1Api = ApiFromExtId(kStatus1Id);
constexpr int32_t kStatus2Api = ApiFromExtId(kStatus2Id);
constexpr int32_t kStatus3Api = ApiFromExtId(kStatus3Id);

constexpr int32_t kControlPeriodMs = 20;
constexpr int32_t kDefaultReadTimeoutMs = 20;
constexpr int32_t kConfigurationReadTimeoutMs = 500;
constexpr int32_t kMaxReadRetries = 5;
constexpr int32_t kMinimumPeriodicTimeoutMs = 500;
constexpr int32_t kMaxStatusPeriodMs = 1000;
constexpr int32_t kFirstMotioncoreBus = 5;

constexpr std::array<int32_t, 5> kControlApis{
    kDutyCycleSetpointApi, kVelocitySetpointApi, kRelativePositionSetpointApi,
    kCurrentSetpointApi, kAbsolutePositionSetpointApi};

struct A301Obj {
  ~A301Obj();

  int32_t busId;
  int32_t deviceId;
  HAL_CANHandle canHandle;
  std::array<int32_t, 4> statusPeriods{10, 250, 20, 20};
  bool inverted = false;
  int32_t activeSetpointApi = kDutyCycleSetpointApi;
  wpi::util::mutex stateMutex;
  wpi::util::mutex configurationMutex;
  std::string previousAllocation;
};

UnlimitedHandleResource<HAL_A301Handle, A301Obj, HAL_HandleEnum::A301>*
    gA301Handles;
wpi::util::mutex gAllocationsMutex;
std::unordered_map<uint32_t, std::string> gAllocations;

uint32_t MakeAllocationKey(int32_t busId, int32_t deviceId) {
  return (static_cast<uint32_t>(busId) << 8) | static_cast<uint32_t>(deviceId);
}

A301Obj::~A301Obj() {
  HAL_CleanCAN(canHandle);
  std::scoped_lock lock{gAllocationsMutex};
  gAllocations.erase(MakeAllocationKey(busId, deviceId));
}

std::shared_ptr<A301Obj> GetA301(HAL_A301Handle handle, int32_t* status) {
  auto a301 = gA301Handles->Get(handle);
  if (!a301) {
    *status = HAL_HANDLE_ERROR;
  }
  return a301;
}

void StoreU32LE(uint8_t* data, uint32_t value) {
  data[0] = value;
  data[1] = value >> 8;
  data[2] = value >> 16;
  data[3] = value >> 24;
}

uint16_t LoadU16LE(const uint8_t* data) {
  return static_cast<uint16_t>(data[0]) | (static_cast<uint16_t>(data[1]) << 8);
}

uint32_t LoadU32LE(const uint8_t* data) {
  return static_cast<uint32_t>(data[0]) |
         (static_cast<uint32_t>(data[1]) << 8) |
         (static_cast<uint32_t>(data[2]) << 16) |
         (static_cast<uint32_t>(data[3]) << 24);
}

void StoreFloatLE(uint8_t* data, double value) {
  StoreU32LE(data, std::bit_cast<uint32_t>(static_cast<float>(value)));
}

double LoadFloatLE(const uint8_t* data) {
  return std::bit_cast<float>(LoadU32LE(data));
}

HAL_CANMessage MakeMessage(const uint8_t* data, uint8_t size) {
  HAL_CANMessage message{};
  message.dataSize = size;
  if (size != 0) {
    std::memcpy(message.data, data, size);
  }
  return message;
}

void WritePacket(const A301Obj& a301, int32_t api, const uint8_t* data,
                 uint8_t size, int32_t* status) {
  auto message = MakeMessage(data, size);
  HAL_WriteCANPacket(a301.canHandle, api, &message, status);
}

void WritePacketRepeating(const A301Obj& a301, int32_t api, const uint8_t* data,
                          uint8_t size, int32_t* status) {
  auto message = MakeMessage(data, size);
  HAL_WriteCANPacketRepeating(a301.canHandle, api, &message, kControlPeriodMs,
                              status);
}

std::array<uint8_t, 64> WriteAndReadPacket(A301Obj& a301, int32_t writeApi,
                                           const uint8_t* writeData,
                                           uint8_t writeSize, int32_t readApi,
                                           uint8_t expectedReadSize,
                                           int32_t readTimeoutMs, bool rtr,
                                           int32_t* status) {
  std::array<uint8_t, 64> data{};
  std::scoped_lock lock{a301.configurationMutex};

  HAL_CANReceiveMessage oldMessage{};
  int32_t oldStatus = 0;
  HAL_ReadCANPacketNew(a301.canHandle, readApi, &oldMessage, &oldStatus);

  int32_t lastSendStatus = 0;
  for (int32_t retry = 0; retry <= kMaxReadRetries; ++retry) {
    if (rtr) {
      HAL_CANMessage message{};
      message.dataSize = expectedReadSize;
      HAL_WriteCANRTRFrame(a301.canHandle, writeApi, &message, &lastSendStatus);
    } else {
      WritePacket(a301, writeApi, writeData, writeSize, &lastSendStatus);
    }
    if (lastSendStatus != 0) {
      continue;
    }

    for (int32_t elapsed = 0; elapsed <= readTimeoutMs; ++elapsed) {
      HAL_CANReceiveMessage message{};
      int32_t readStatus = 0;
      HAL_ReadCANPacketNew(a301.canHandle, readApi, &message, &readStatus);
      if (readStatus == 0) {
        auto copySize =
            std::min<uint8_t>(message.message.dataSize, data.size());
        std::memcpy(data.data(), message.message.data, copySize);
        if (message.message.dataSize != expectedReadSize) {
          *status = HAL_ERR_CANSessionMux_InvalidBuffer;
          return data;
        }
        *status = 0;
        return data;
      }
      if (elapsed != readTimeoutMs) {
        std::this_thread::sleep_for(std::chrono::milliseconds{1});
      }
    }
  }

  *status = lastSendStatus != 0 ? lastSendStatus : HAL_CAN_TIMEOUT;
  return data;
}

std::array<uint8_t, 8> ReadPeriodicPacket(A301Obj& a301, int32_t api,
                                          int32_t periodMs, uint64_t* timestamp,
                                          int32_t* status) {
  std::array<uint8_t, 8> data{};
  HAL_CANReceiveMessage message{};
  HAL_ReadCANPacketLatest(a301.canHandle, api, &message, status);
  if (*status != 0) {
    return data;
  }

  *timestamp = message.timeStamp;
  auto copySize = std::min<uint8_t>(message.message.dataSize, data.size());
  std::memcpy(data.data(), message.message.data, copySize);
  if (message.message.dataSize != data.size()) {
    *status = HAL_ERR_CANSessionMux_InvalidBuffer;
    return data;
  }

  int32_t timeoutMs =
      std::max(static_cast<int32_t>(periodMs * 2.1), kMinimumPeriodicTimeoutMs);
  uint64_t now = wpi::util::Now();
  if (now >= message.timeStamp &&
      now - message.timeStamp > static_cast<uint64_t>(timeoutMs) * 1000) {
    *status = HAL_CAN_TIMEOUT;
  }
  return data;
}

int32_t GetStatusPeriod(A301Obj& a301, HAL_A301PeriodicFrame frame,
                        int32_t* status) {
  auto index = static_cast<int32_t>(frame);
  if (index < 0 || index >= static_cast<int32_t>(a301.statusPeriods.size())) {
    *status = HAL_PARAMETER_OUT_OF_RANGE;
    return 0;
  }
  std::scoped_lock lock{a301.stateMutex};
  return a301.statusPeriods[index];
}

HAL_Bool Bit(uint8_t value, int bit) {
  return (value >> bit) & 1;
}

}  // namespace

namespace wpi::hal::init {
void InitializeA301() {
  static UnlimitedHandleResource<HAL_A301Handle, A301Obj, HAL_HandleEnum::A301>
      handles;
  gA301Handles = &handles;
}
}  // namespace wpi::hal::init

extern "C" {

int32_t HAL_DetectA301DeviceId(int32_t busId, int32_t* status) {
  wpi::hal::init::CheckInit();
  *status = 0;

  constexpr uint32_t kDeviceIdMask = 0x3f;
  constexpr uint32_t kAllExceptDeviceIdMask = 0x1fffffc0;
  HAL_CANStreamHandle stream = HAL_CAN_OpenStreamSession(
      busId, kStatus0Id, kAllExceptDeviceIdMask, 1, status);
  if (*status != 0) {
    return -1;
  }

  std::this_thread::sleep_for(std::chrono::milliseconds{10});
  int32_t lastStatus = HAL_ERR_CANSessionMux_MessageNotFound;
  for (int retry = 0; retry < 4; ++retry) {
    HAL_CANStreamMessage message{};
    uint32_t messagesRead = 0;
    int32_t readStatus = 0;
    HAL_CAN_ReadStreamSession(stream, &message, 1, &messagesRead, &readStatus);
    if ((readStatus == 0 ||
         readStatus == HAL_ERR_CANSessionMux_SessionOverrun) &&
        messagesRead == 1) {
      HAL_CAN_CloseStreamSession(stream);
      *status = 0;
      return message.messageId & kDeviceIdMask;
    }
    lastStatus =
        readStatus != 0 ? readStatus : HAL_ERR_CANSessionMux_MessageNotFound;
    if (retry != 3) {
      std::this_thread::sleep_for(std::chrono::milliseconds{100});
    }
  }

  HAL_CAN_CloseStreamSession(stream);
  *status = lastStatus;
  return -1;
}

HAL_A301Handle HAL_InitializeA301(int32_t busId, int32_t deviceId,
                                  const char* allocationLocation,
                                  int32_t* status) {
  wpi::hal::init::CheckInit();
  *status = 0;

  if (busId >= kFirstMotioncoreBus) {
    int32_t detectStatus = 0;
    int32_t detectedId = HAL_DetectA301DeviceId(busId, &detectStatus);
    if (detectStatus == 0) {
      deviceId = detectedId;
    }
  }

  if (!HAL_CheckA301DeviceId(deviceId)) {
    *status = HAL_PARAMETER_OUT_OF_RANGE;
    return HAL_INVALID_HANDLE;
  }

  HAL_CANHandle canHandle = HAL_InitializeCAN(
      busId, HAL_CANManufacturer::HAL_CAN_MAN_FIRST, deviceId,
      HAL_CANDeviceType::HAL_CAN_DEV_MOTOR_CONTROLLER, status);
  if (*status != 0) {
    return HAL_INVALID_HANDLE;
  }

  uint32_t allocationKey = MakeAllocationKey(busId, deviceId);
  {
    std::scoped_lock lock{gAllocationsMutex};
    auto [it, inserted] = gAllocations.try_emplace(
        allocationKey, allocationLocation ? allocationLocation : "");
    if (!inserted) {
      HAL_CleanCAN(canHandle);
      *status = MakeErrorPreviouslyAllocated(HAL_RESOURCE_IS_ALLOCATED, "A301",
                                             deviceId, it->second);
      return HAL_INVALID_HANDLE;
    }
  }

  auto a301 = std::make_shared<A301Obj>();
  a301->busId = busId;
  a301->deviceId = deviceId;
  a301->canHandle = canHandle;
  a301->previousAllocation = allocationLocation ? allocationLocation : "";

  HAL_A301Handle handle = gA301Handles->Allocate(a301);
  if (handle == HAL_INVALID_HANDLE) {
    *status = HAL_NO_AVAILABLE_RESOURCES;
    return HAL_INVALID_HANDLE;
  }

  for (int32_t api : kControlApis) {
    int32_t stopStatus = 0;
    HAL_StopCANPacketRepeating(canHandle, api, &stopStatus);
  }

  std::array<uint8_t, 8> zeroSetpoint{};
  int32_t setpointStatus = 0;
  WritePacketRepeating(*a301, kDutyCycleSetpointApi, zeroSetpoint.data(),
                       zeroSetpoint.size(), &setpointStatus);
  return handle;
}

void HAL_FreeA301(HAL_A301Handle handle) {
  gA301Handles->Free(handle);
}

HAL_Bool HAL_CheckA301DeviceId(int32_t deviceId) {
  return deviceId >= HAL_A301_MIN_DEVICE_ID &&
         deviceId <= HAL_A301_MAX_DEVICE_ID;
}

int32_t HAL_GetA301BusId(HAL_A301Handle handle, int32_t* status) {
  auto a301 = GetA301(handle, status);
  return a301 ? a301->busId : -1;
}

int32_t HAL_GetA301DeviceId(HAL_A301Handle handle, int32_t* status) {
  auto a301 = GetA301(handle, status);
  return a301 ? a301->deviceId : -1;
}

void HAL_GetA301FirmwareVersion(HAL_A301Handle handle,
                                HAL_A301FirmwareVersion* version,
                                int32_t* status) {
  if (!version) {
    *status = HAL_NULL_PARAMETER;
    return;
  }
  *version = {};
  auto a301 = GetA301(handle, status);
  if (!a301) {
    return;
  }

  auto data = WriteAndReadPacket(*a301, kGetFirmwareVersionApi, nullptr, 0,
                                 kGetFirmwareVersionApi, 8,
                                 kDefaultReadTimeoutMs, true, status);
  version->major = data[0];
  version->minor = data[1];
  version->patch = (static_cast<uint16_t>(data[2]) << 8) | data[3];
  version->prerelease = data[4];
  version->hardwareRevision = data[5];
  version->raw = (static_cast<uint32_t>(version->major) << 24) |
                 (static_cast<uint32_t>(version->minor) << 16) | version->patch;
}

void HAL_GetA301PeriodicStatus0(HAL_A301Handle handle,
                                HAL_A301PeriodicStatus0* frame,
                                int32_t* status) {
  if (!frame) {
    *status = HAL_NULL_PARAMETER;
    return;
  }
  *frame = {};
  auto a301 = GetA301(handle, status);
  if (!a301) {
    return;
  }

  bool inverted;
  int32_t periodMs;
  {
    std::scoped_lock lock{a301->stateMutex};
    inverted = a301->inverted;
    periodMs = a301->statusPeriods[0];
  }
  auto data = ReadPeriodicPacket(*a301, kStatus0Api, periodMs,
                                 &frame->timestamp, status);
  int16_t appliedOutput = static_cast<int16_t>(LoadU16LE(data.data()));
  uint16_t voltage = static_cast<uint16_t>(data[2]) |
                     (static_cast<uint16_t>(data[3] & 0x0f) << 8);
  uint16_t current = static_cast<uint16_t>(data[3] >> 4) |
                     (static_cast<uint16_t>(data[4]) << 4);
  frame->appliedOutput =
      appliedOutput * 3.082369457075716e-05 * (inverted ? -1.0 : 1.0);
  frame->voltage = voltage * 0.0073260073260073;
  frame->current = current * 0.0366300366300366;
  frame->motorTemperature = data[5];
  frame->inverted = inverted;
  frame->primaryHeartbeatLock = data[6] & 0x01;
  frame->gearboxRPM = static_cast<HAL_A301GearboxRPM>((data[6] >> 1) & 0x0f);
}

void HAL_GetA301PeriodicStatus1(HAL_A301Handle handle,
                                HAL_A301PeriodicStatus1* frame,
                                int32_t* status) {
  if (!frame) {
    *status = HAL_NULL_PARAMETER;
    return;
  }
  *frame = {};
  auto a301 = GetA301(handle, status);
  if (!a301) {
    return;
  }

  int32_t periodMs = GetStatusPeriod(*a301, HAL_A301_STATUS_1, status);
  if (*status != 0) {
    return;
  }
  auto data = ReadPeriodicPacket(*a301, kStatus1Api, periodMs,
                                 &frame->timestamp, status);
  frame->otherFault = Bit(data[0], 0);
  frame->motorTypeFault = Bit(data[0], 1);
  frame->sensorFault = Bit(data[0], 2);
  frame->canFault = Bit(data[0], 3);
  frame->temperatureFault = Bit(data[0], 4);
  frame->drvFault = Bit(data[0], 5);
  frame->escEepromFault = Bit(data[0], 6);
  frame->firmwareFault = Bit(data[0], 7);
  frame->motorStartupFault = Bit(data[1], 0);
  frame->overvoltageWarning = Bit(data[1], 6);
  frame->motorLoopSpeedWarning = Bit(data[1], 7);
  frame->brownoutWarning = Bit(data[2], 0);
  frame->overcurrentWarning = Bit(data[2], 1);
  frame->escEepromWarning = Bit(data[2], 2);
  frame->extEepromWarning = Bit(data[2], 3);
  frame->sensorWarning = Bit(data[2], 4);
  frame->stallWarning = Bit(data[2], 5);
  frame->hasResetWarning = Bit(data[2], 6);
  frame->otherWarning = Bit(data[2], 7);
  frame->otherStickyFault = Bit(data[3], 0);
  frame->motorTypeStickyFault = Bit(data[3], 1);
  frame->sensorStickyFault = Bit(data[3], 2);
  frame->canStickyFault = Bit(data[3], 3);
  frame->temperatureStickyFault = Bit(data[3], 4);
  frame->drvStickyFault = Bit(data[3], 5);
  frame->escEepromStickyFault = Bit(data[3], 6);
  frame->firmwareStickyFault = Bit(data[3], 7);
  frame->motorStartupStickyFault = Bit(data[4], 0);
  frame->overvoltageStickyWarning = Bit(data[4], 6);
  frame->motorLoopSpeedStickyWarning = Bit(data[4], 7);
  frame->brownoutStickyWarning = Bit(data[5], 0);
  frame->overcurrentStickyWarning = Bit(data[5], 1);
  frame->escEepromStickyWarning = Bit(data[5], 2);
  frame->extEepromStickyWarning = Bit(data[5], 3);
  frame->sensorStickyWarning = Bit(data[5], 4);
  frame->stallStickyWarning = Bit(data[5], 5);
  frame->hasResetStickyWarning = Bit(data[5], 6);
  frame->otherStickyWarning = Bit(data[5], 7);
  frame->isFollower = Bit(data[6], 0);
}

void HAL_GetA301PeriodicStatus2(HAL_A301Handle handle,
                                HAL_A301PeriodicStatus2* frame,
                                int32_t* status) {
  if (!frame) {
    *status = HAL_NULL_PARAMETER;
    return;
  }
  *frame = {};
  auto a301 = GetA301(handle, status);
  if (!a301) {
    return;
  }

  bool inverted;
  int32_t periodMs;
  {
    std::scoped_lock lock{a301->stateMutex};
    inverted = a301->inverted;
    periodMs = a301->statusPeriods[2];
  }
  auto data = ReadPeriodicPacket(*a301, kStatus2Api, periodMs,
                                 &frame->timestamp, status);
  frame->encoderVelocity = LoadFloatLE(data.data()) * (inverted ? -1.0 : 1.0);
  frame->relativeEncoderPosition = LoadFloatLE(data.data() + 4);
}

void HAL_GetA301PeriodicStatus3(HAL_A301Handle handle,
                                HAL_A301PeriodicStatus3* frame,
                                int32_t* status) {
  if (!frame) {
    *status = HAL_NULL_PARAMETER;
    return;
  }
  *frame = {};
  auto a301 = GetA301(handle, status);
  if (!a301) {
    return;
  }

  int32_t periodMs = GetStatusPeriod(*a301, HAL_A301_STATUS_3, status);
  if (*status != 0) {
    return;
  }
  auto data = ReadPeriodicPacket(*a301, kStatus3Api, periodMs,
                                 &frame->timestamp, status);
  frame->absoluteEncoderPosition = LoadFloatLE(data.data() + 4);
}

void HAL_SetA301RelativeEncoderPosition(HAL_A301Handle handle, double position,
                                        int32_t* status) {
  auto a301 = GetA301(handle, status);
  if (!a301) {
    return;
  }
  std::array<uint8_t, 4> data{};
  StoreFloatLE(data.data(), position);
  WritePacket(*a301, kSetRelativeEncoderPositionApi, data.data(), data.size(),
              status);
}

void HAL_SetA301AbsoluteEncoderPosition(HAL_A301Handle handle, double position,
                                        int32_t* status) {
  auto a301 = GetA301(handle, status);
  if (!a301) {
    return;
  }
  std::array<uint8_t, 4> data{};
  StoreFloatLE(data.data(), position);
  WritePacket(*a301, kSetAbsoluteEncoderPositionApi, data.data(), data.size(),
              status);
}

void HAL_SetA301Setpoint(HAL_A301Handle handle, double value,
                         HAL_A301ControlType controlType, double positionSpeed,
                         int32_t* status) {
  auto a301 = GetA301(handle, status);
  if (!a301) {
    return;
  }

  auto controlIndex = static_cast<int32_t>(controlType);
  constexpr std::array<int32_t, 6> kControlTypeApis{
      kDutyCycleSetpointApi,        kVelocitySetpointApi,
      kDutyCycleSetpointApi,        kRelativePositionSetpointApi,
      kAbsolutePositionSetpointApi, kCurrentSetpointApi};
  if (controlIndex < 0 ||
      controlIndex >= static_cast<int32_t>(kControlTypeApis.size())) {
    *status = HAL_PARAMETER_OUT_OF_RANGE;
    return;
  }

  if (controlType == HAL_A301_CONTROL_TYPE_VOLTAGE) {
    value /= 12.0;
  }

  int32_t api = kControlTypeApis[controlIndex];
  std::array<uint8_t, 8> data{};
  {
    std::scoped_lock lock{a301->stateMutex};
    if (a301->inverted) {
      value *= -1.0;
    }
    if (a301->activeSetpointApi != api) {
      int32_t stopStatus = 0;
      HAL_StopCANPacketRepeating(a301->canHandle, a301->activeSetpointApi,
                                 &stopStatus);
      if (stopStatus != 0) {
        *status = stopStatus;
        return;
      }
    }
    a301->activeSetpointApi = api;
    StoreFloatLE(data.data(), value);
    StoreFloatLE(data.data() + 4, positionSpeed);
    WritePacketRepeating(*a301, api, data.data(), data.size(), status);
  }
}

void HAL_SetA301IdleMode(HAL_A301Handle handle, HAL_A301IdleMode idleMode,
                         int32_t* status) {
  auto a301 = GetA301(handle, status);
  if (!a301) {
    return;
  }
  if (idleMode != HAL_A301_IDLE_MODE_COAST &&
      idleMode != HAL_A301_IDLE_MODE_BRAKE) {
    *status = HAL_PARAMETER_OUT_OF_RANGE;
    return;
  }
  uint8_t data[] = {static_cast<uint8_t>(idleMode)};
  WritePacket(*a301, kSetIdleModeApi, data, sizeof(data), status);
}

HAL_A301IdleMode HAL_GetA301IdleMode(HAL_A301Handle handle, int32_t* status) {
  auto a301 = GetA301(handle, status);
  if (!a301) {
    return HAL_A301_IDLE_MODE_COAST;
  }
  auto data = WriteAndReadPacket(*a301, kGetIdleModeApi, nullptr, 0,
                                 kGetIdleModeResponseApi, 1,
                                 kConfigurationReadTimeoutMs, false, status);
  return static_cast<HAL_A301IdleMode>(data[0]);
}

void HAL_SetA301AbsolutePositionContinuousInput(HAL_A301Handle handle,
                                                HAL_Bool enabled,
                                                int32_t* status) {
  auto a301 = GetA301(handle, status);
  if (!a301) {
    return;
  }
  uint8_t data[] = {static_cast<uint8_t>(enabled != 0)};
  WritePacket(*a301, kSetContinuousInputApi, data, sizeof(data), status);
}

HAL_Bool HAL_GetA301AbsolutePositionContinuousInput(HAL_A301Handle handle,
                                                    int32_t* status) {
  auto a301 = GetA301(handle, status);
  if (!a301) {
    return false;
  }
  auto data = WriteAndReadPacket(*a301, kGetContinuousInputApi, nullptr, 0,
                                 kGetContinuousInputResponseApi, 1,
                                 kConfigurationReadTimeoutMs, false, status);
  return data[0] & 1;
}

void HAL_SetA301AbsoluteEncoderRangeOffset(HAL_A301Handle handle, double offset,
                                           int32_t* status) {
  auto a301 = GetA301(handle, status);
  if (!a301) {
    return;
  }
  if (!std::isfinite(offset) || offset < -0.5 || offset > 0.5) {
    *status = HAL_PARAMETER_OUT_OF_RANGE;
    return;
  }
  {
    std::scoped_lock lock{a301->stateMutex};
    if (a301->inverted) {
      offset *= -1.0;
    }
  }
  std::array<uint8_t, 4> data{};
  StoreFloatLE(data.data(), offset);
  WritePacket(*a301, kSetAbsoluteRangeOffsetApi, data.data(), data.size(),
              status);
}

double HAL_GetA301AbsoluteEncoderRangeOffset(HAL_A301Handle handle,
                                             int32_t* status) {
  auto a301 = GetA301(handle, status);
  if (!a301) {
    return 0.0;
  }
  auto data = WriteAndReadPacket(*a301, kGetAbsoluteRangeOffsetApi, nullptr, 0,
                                 kGetAbsoluteRangeOffsetResponseApi, 4,
                                 kConfigurationReadTimeoutMs, false, status);
  double offset = LoadFloatLE(data.data());
  {
    std::scoped_lock lock{a301->stateMutex};
    if (a301->inverted) {
      offset *= -1.0;
    }
  }
  return offset;
}

void HAL_SetA301Inverted(HAL_A301Handle handle, HAL_Bool inverted,
                         int32_t* status) {
  auto a301 = GetA301(handle, status);
  if (!a301) {
    return;
  }
  std::scoped_lock lock{a301->stateMutex};
  a301->inverted = inverted;
}

HAL_Bool HAL_GetA301Inverted(HAL_A301Handle handle, int32_t* status) {
  auto a301 = GetA301(handle, status);
  if (!a301) {
    return false;
  }
  std::scoped_lock lock{a301->stateMutex};
  return a301->inverted;
}

void HAL_ClearA301Faults(HAL_A301Handle handle, int32_t* status) {
  auto a301 = GetA301(handle, status);
  if (!a301) {
    return;
  }
  WritePacket(*a301, kClearFaultsApi, nullptr, 0, status);
}

void HAL_SetA301StatusFramePeriod(HAL_A301Handle handle,
                                  HAL_A301PeriodicFrame frame, int32_t periodMs,
                                  int32_t* status) {
  auto a301 = GetA301(handle, status);
  if (!a301) {
    return;
  }
  auto frameIndex = static_cast<int32_t>(frame);
  if (frameIndex < 0 ||
      frameIndex >= static_cast<int32_t>(a301->statusPeriods.size()) ||
      periodMs < 0) {
    *status = HAL_PARAMETER_OUT_OF_RANGE;
    return;
  }
  periodMs = std::min(periodMs, kMaxStatusPeriodMs);

  std::array<uint8_t, 5> request{};
  request[0] = frameIndex;
  StoreU32LE(request.data() + 1, periodMs);
  auto response =
      WriteAndReadPacket(*a301, kSetStatusPeriodApi, request.data(),
                         request.size(), kSetStatusPeriodResponseApi, 6,
                         kConfigurationReadTimeoutMs, false, status);
  if (*status != 0) {
    return;
  }
  if (response[0] != 0 || response[1] != frameIndex) {
    *status = MakeError(HAL_INCOMPATIBLE_STATE,
                        "A301 rejected the status frame period");
    return;
  }

  int32_t effectivePeriodMs = LoadU32LE(response.data() + 2);
  std::scoped_lock lock{a301->stateMutex};
  a301->statusPeriods[frameIndex] = effectivePeriodMs;
}

int32_t HAL_GetA301StatusFramePeriod(HAL_A301Handle handle,
                                     HAL_A301PeriodicFrame frame,
                                     int32_t* status) {
  auto a301 = GetA301(handle, status);
  if (!a301) {
    return 0;
  }
  auto frameIndex = static_cast<int32_t>(frame);
  if (frameIndex < 0 ||
      frameIndex >= static_cast<int32_t>(a301->statusPeriods.size())) {
    *status = HAL_PARAMETER_OUT_OF_RANGE;
    return 0;
  }

  uint8_t request[] = {static_cast<uint8_t>(frameIndex)};
  auto response =
      WriteAndReadPacket(*a301, kGetStatusPeriodApi, request, sizeof(request),
                         kGetStatusPeriodResponseApi, 5,
                         kConfigurationReadTimeoutMs, false, status);
  if (*status != 0) {
    return 0;
  }
  if (response[0] != frameIndex) {
    *status = MakeError(HAL_INCOMPATIBLE_STATE,
                        "A301 returned a mismatched status frame");
    return 0;
  }
  int32_t periodMs = LoadU32LE(response.data() + 1);
  {
    std::scoped_lock lock{a301->stateMutex};
    a301->statusPeriods[frameIndex] = periodMs;
  }
  return periodMs;
}

}  // extern "C"
