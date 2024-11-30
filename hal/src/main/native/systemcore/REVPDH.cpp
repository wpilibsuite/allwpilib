// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "REVPDH.h"

#include <hal/CAN.h>
#include <hal/CANAPI.h>
#include <hal/CANAPITypes.h>
#include <hal/Errors.h>
#include <hal/handles/HandlesInternal.h>
#include <hal/handles/IndexedHandleResource.h>

#include <cstring>
#include <string>
#include <thread>

#include <fmt/format.h>

#include "HALInitializer.h"
#include "HALInternal.h"
#include "PortsInternal.h"
#include "rev/PDHFrames.h"

using namespace hal;

static constexpr HAL_CANManufacturer manufacturer =
    HAL_CANManufacturer::HAL_CAN_Man_kREV;

static constexpr HAL_CANDeviceType deviceType =
    HAL_CANDeviceType::HAL_CAN_Dev_kPowerDistribution;

static constexpr int32_t kDefaultControlPeriod = 50;

namespace {

struct REV_PDHObj {
  int32_t controlPeriod;
  HAL_CANHandle hcan;
  std::string previousAllocation;
  HAL_PowerDistributionVersion versionInfo;
  bool streamHandleAllocated{false};
  uint32_t streamSessionHandles[4];
};

}  // namespace

static constexpr uint32_t APIFromExtId(uint32_t extId) {
  return (extId >> 6) & 0x3FF;
}

static constexpr uint32_t PDH_SET_SWITCH_CHANNEL_FRAME_API =
    APIFromExtId(PDH_SET_SWITCH_CHANNEL_FRAME_ID);

static constexpr uint32_t PDH_STATUS_0_FRAME_API =
    APIFromExtId(PDH_STATUS_0_FRAME_ID);
static constexpr uint32_t PDH_STATUS_1_FRAME_API =
    APIFromExtId(PDH_STATUS_1_FRAME_ID);
static constexpr uint32_t PDH_STATUS_2_FRAME_API =
    APIFromExtId(PDH_STATUS_2_FRAME_ID);
static constexpr uint32_t PDH_STATUS_3_FRAME_API =
    APIFromExtId(PDH_STATUS_3_FRAME_ID);
static constexpr uint32_t PDH_STATUS_4_FRAME_API =
    APIFromExtId(PDH_STATUS_4_FRAME_ID);

static constexpr uint32_t PDH_CLEAR_FAULTS_FRAME_API =
    APIFromExtId(PDH_CLEAR_FAULTS_FRAME_ID);

static constexpr uint32_t PDH_VERSION_FRAME_API =
    APIFromExtId(PDH_VERSION_FRAME_ID);

static constexpr int32_t kPDHFrameStatus0Timeout = 20;
static constexpr int32_t kPDHFrameStatus1Timeout = 20;
static constexpr int32_t kPDHFrameStatus2Timeout = 20;
static constexpr int32_t kPDHFrameStatus3Timeout = 20;
static constexpr int32_t kPDHFrameStatus4Timeout = 20;

static IndexedHandleResource<HAL_REVPDHHandle, REV_PDHObj, kNumREVPDHModules,
                             HAL_HandleEnum::REVPDH>* REVPDHHandles;

namespace hal::init {
void InitializeREVPDH() {
  static IndexedHandleResource<HAL_REVPDHHandle, REV_PDHObj, kNumREVPDHModules,
                               HAL_HandleEnum::REVPDH>
      rH;
  REVPDHHandles = &rH;
}
}  // namespace hal::init

extern "C" {

static PDH_status_0_t HAL_ReadREVPDHStatus0(HAL_CANHandle hcan,
                                            int32_t* status) {
  uint8_t packedData[8] = {0};
  int32_t length = 0;
  uint64_t timestamp = 0;
  PDH_status_0_t result = {};

  HAL_ReadCANPacketTimeout(hcan, PDH_STATUS_0_FRAME_API, packedData, &length,
                           &timestamp, kPDHFrameStatus0Timeout * 2, status);

  if (*status != 0) {
    return result;
  }

  PDH_status_0_unpack(&result, packedData, PDH_STATUS_0_LENGTH);

  return result;
}

static PDH_status_1_t HAL_ReadREVPDHStatus1(HAL_CANHandle hcan,
                                            int32_t* status) {
  uint8_t packedData[8] = {0};
  int32_t length = 0;
  uint64_t timestamp = 0;
  PDH_status_1_t result = {};

  HAL_ReadCANPacketTimeout(hcan, PDH_STATUS_1_FRAME_API, packedData, &length,
                           &timestamp, kPDHFrameStatus1Timeout * 2, status);

  if (*status != 0) {
    return result;
  }

  PDH_status_1_unpack(&result, packedData, PDH_STATUS_1_LENGTH);

  return result;
}

static PDH_status_2_t HAL_ReadREVPDHStatus2(HAL_CANHandle hcan,
                                            int32_t* status) {
  uint8_t packedData[8] = {0};
  int32_t length = 0;
  uint64_t timestamp = 0;
  PDH_status_2_t result = {};

  HAL_ReadCANPacketTimeout(hcan, PDH_STATUS_2_FRAME_API, packedData, &length,
                           &timestamp, kPDHFrameStatus2Timeout * 2, status);

  if (*status != 0) {
    return result;
  }

  PDH_status_2_unpack(&result, packedData, PDH_STATUS_2_LENGTH);

  return result;
}

static PDH_status_3_t HAL_ReadREVPDHStatus3(HAL_CANHandle hcan,
                                            int32_t* status) {
  uint8_t packedData[8] = {0};
  int32_t length = 0;
  uint64_t timestamp = 0;
  PDH_status_3_t result = {};

  HAL_ReadCANPacketTimeout(hcan, PDH_STATUS_3_FRAME_API, packedData, &length,
                           &timestamp, kPDHFrameStatus3Timeout * 2, status);

  if (*status != 0) {
    return result;
  }

  PDH_status_3_unpack(&result, packedData, PDH_STATUS_3_LENGTH);

  return result;
}

static PDH_status_4_t HAL_ReadREVPDHStatus4(HAL_CANHandle hcan,
                                            int32_t* status) {
  uint8_t packedData[8] = {0};
  int32_t length = 0;
  uint64_t timestamp = 0;
  PDH_status_4_t result = {};

  HAL_ReadCANPacketTimeout(hcan, PDH_STATUS_4_FRAME_API, packedData, &length,
                           &timestamp, kPDHFrameStatus4Timeout * 2, status);

  if (*status != 0) {
    return result;
  }

  PDH_status_4_unpack(&result, packedData, PDH_STATUS_4_LENGTH);

  return result;
}

/**
 * Helper function for the individual getter functions for status 4
 */
PDH_status_4_t HAL_GetREVPDHStatus4(HAL_REVPDHHandle handle, int32_t* status) {
  PDH_status_4_t statusFrame = {};
  auto hpdh = REVPDHHandles->Get(handle);
  if (hpdh == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return statusFrame;
  }

  statusFrame = HAL_ReadREVPDHStatus4(hpdh->hcan, status);
  return statusFrame;
}

HAL_REVPDHHandle HAL_InitializeREVPDH(int32_t module,
                                      const char* allocationLocation,
                                      int32_t* status) {
  hal::init::CheckInit();
  if (!HAL_CheckREVPDHModuleNumber(module)) {
    *status = RESOURCE_OUT_OF_RANGE;
    hal::SetLastErrorIndexOutOfRange(status, "Invalid Index for REV PDH", 1,
                                     kNumREVPDHModules, module);
    return HAL_kInvalidHandle;
  }

  HAL_REVPDHHandle handle;
  // Module starts at 1
  auto hpdh = REVPDHHandles->Allocate(module - 1, &handle, status);
  if (*status != 0) {
    if (hpdh) {
      hal::SetLastErrorPreviouslyAllocated(status, "REV PDH", module,
                                           hpdh->previousAllocation);
    } else {
      hal::SetLastErrorIndexOutOfRange(status, "Invalid Index for REV PDH", 1,
                                       kNumREVPDHModules, module);
    }
    return HAL_kInvalidHandle;  // failed to allocate. Pass error back.
  }

  HAL_CANHandle hcan =
      HAL_InitializeCAN(manufacturer, module, deviceType, status);

  if (*status != 0) {
    REVPDHHandles->Free(handle);
    return HAL_kInvalidHandle;
  }

  hpdh->previousAllocation = allocationLocation ? allocationLocation : "";
  hpdh->hcan = hcan;
  hpdh->controlPeriod = kDefaultControlPeriod;
  std::memset(&hpdh->versionInfo, 0, sizeof(hpdh->versionInfo));

  return handle;
}

void HAL_FreeREVPDH(HAL_REVPDHHandle handle) {
  auto hpdh = REVPDHHandles->Get(handle);
  if (hpdh == nullptr) {
    return;
  }

  HAL_CleanCAN(hpdh->hcan);

  REVPDHHandles->Free(handle);
}

int32_t HAL_GetREVPDHModuleNumber(HAL_REVPDHHandle handle, int32_t* status) {
  return hal::getHandleIndex(handle);
}

HAL_Bool HAL_CheckREVPDHModuleNumber(int32_t module) {
  return ((module >= 1) && (module <= kNumREVPDHModules)) ? 1 : 0;
}

HAL_Bool HAL_CheckREVPDHChannelNumber(int32_t channel) {
  return ((channel >= 0) && (channel < kNumREVPDHChannels)) ? 1 : 0;
}

double HAL_GetREVPDHChannelCurrent(HAL_REVPDHHandle handle, int32_t channel,
                                   int32_t* status) {
  auto hpdh = REVPDHHandles->Get(handle);
  if (hpdh == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return 0;
  }

  if (!HAL_CheckREVPDHChannelNumber(channel)) {
    *status = RESOURCE_OUT_OF_RANGE;
    return 0;
  }

  // Determine what periodic status the channel is in
  if (channel < 6) {
    // Periodic status 0
    PDH_status_0_t statusFrame = HAL_ReadREVPDHStatus0(hpdh->hcan, status);
    switch (channel) {
      case 0:
        return PDH_status_0_channel_0_current_decode(
            statusFrame.channel_0_current);
      case 1:
        return PDH_status_0_channel_1_current_decode(
            statusFrame.channel_1_current);
      case 2:
        return PDH_status_0_channel_2_current_decode(
            statusFrame.channel_2_current);
      case 3:
        return PDH_status_0_channel_3_current_decode(
            statusFrame.channel_3_current);
      case 4:
        return PDH_status_0_channel_4_current_decode(
            statusFrame.channel_4_current);
      case 5:
        return PDH_status_0_channel_5_current_decode(
            statusFrame.channel_5_current);
    }
  } else if (channel < 12) {
    // Periodic status 1
    PDH_status_1_t statusFrame = HAL_ReadREVPDHStatus1(hpdh->hcan, status);
    switch (channel) {
      case 6:
        return PDH_status_1_channel_6_current_decode(
            statusFrame.channel_6_current);
      case 7:
        return PDH_status_1_channel_7_current_decode(
            statusFrame.channel_7_current);
      case 8:
        return PDH_status_1_channel_8_current_decode(
            statusFrame.channel_8_current);
      case 9:
        return PDH_status_1_channel_9_current_decode(
            statusFrame.channel_9_current);
      case 10:
        return PDH_status_1_channel_10_current_decode(
            statusFrame.channel_10_current);
      case 11:
        return PDH_status_1_channel_11_current_decode(
            statusFrame.channel_11_current);
    }
  } else if (channel < 18) {
    // Periodic status 2
    PDH_status_2_t statusFrame = HAL_ReadREVPDHStatus2(hpdh->hcan, status);
    switch (channel) {
      case 12:
        return PDH_status_2_channel_12_current_decode(
            statusFrame.channel_12_current);
      case 13:
        return PDH_status_2_channel_13_current_decode(
            statusFrame.channel_13_current);
      case 14:
        return PDH_status_2_channel_14_current_decode(
            statusFrame.channel_14_current);
      case 15:
        return PDH_status_2_channel_15_current_decode(
            statusFrame.channel_15_current);
      case 16:
        return PDH_status_2_channel_16_current_decode(
            statusFrame.channel_16_current);
      case 17:
        return PDH_status_2_channel_17_current_decode(
            statusFrame.channel_17_current);
    }
  } else if (channel < 24) {
    // Periodic status 3
    PDH_status_3_t statusFrame = HAL_ReadREVPDHStatus3(hpdh->hcan, status);
    switch (channel) {
      case 18:
        return PDH_status_3_channel_18_current_decode(
            statusFrame.channel_18_current);
      case 19:
        return PDH_status_3_channel_19_current_decode(
            statusFrame.channel_19_current);
      case 20:
        return PDH_status_3_channel_20_current_decode(
            statusFrame.channel_20_current);
      case 21:
        return PDH_status_3_channel_21_current_decode(
            statusFrame.channel_21_current);
      case 22:
        return PDH_status_3_channel_22_current_decode(
            statusFrame.channel_22_current);
      case 23:
        return PDH_status_3_channel_23_current_decode(
            statusFrame.channel_23_current);
    }
  }
  return 0;
}

void HAL_GetREVPDHAllChannelCurrents(HAL_REVPDHHandle handle, double* currents,
                                     int32_t* status) {
  auto hpdh = REVPDHHandles->Get(handle);
  if (hpdh == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return;
  }

  PDH_status_0_t statusFrame0 = HAL_ReadREVPDHStatus0(hpdh->hcan, status);
  PDH_status_1_t statusFrame1 = HAL_ReadREVPDHStatus1(hpdh->hcan, status);
  PDH_status_2_t statusFrame2 = HAL_ReadREVPDHStatus2(hpdh->hcan, status);
  PDH_status_3_t statusFrame3 = HAL_ReadREVPDHStatus3(hpdh->hcan, status);

  currents[0] =
      PDH_status_0_channel_0_current_decode(statusFrame0.channel_0_current);
  currents[1] =
      PDH_status_0_channel_1_current_decode(statusFrame0.channel_1_current);
  currents[2] =
      PDH_status_0_channel_2_current_decode(statusFrame0.channel_2_current);
  currents[3] =
      PDH_status_0_channel_3_current_decode(statusFrame0.channel_3_current);
  currents[4] =
      PDH_status_0_channel_4_current_decode(statusFrame0.channel_4_current);
  currents[5] =
      PDH_status_0_channel_5_current_decode(statusFrame0.channel_5_current);
  currents[6] =
      PDH_status_1_channel_6_current_decode(statusFrame1.channel_6_current);
  currents[7] =
      PDH_status_1_channel_7_current_decode(statusFrame1.channel_7_current);
  currents[8] =
      PDH_status_1_channel_8_current_decode(statusFrame1.channel_8_current);
  currents[9] =
      PDH_status_1_channel_9_current_decode(statusFrame1.channel_9_current);
  currents[10] =
      PDH_status_1_channel_10_current_decode(statusFrame1.channel_10_current);
  currents[11] =
      PDH_status_1_channel_11_current_decode(statusFrame1.channel_11_current);
  currents[12] =
      PDH_status_2_channel_12_current_decode(statusFrame2.channel_12_current);
  currents[13] =
      PDH_status_2_channel_13_current_decode(statusFrame2.channel_13_current);
  currents[14] =
      PDH_status_2_channel_14_current_decode(statusFrame2.channel_14_current);
  currents[15] =
      PDH_status_2_channel_15_current_decode(statusFrame2.channel_15_current);
  currents[16] =
      PDH_status_2_channel_16_current_decode(statusFrame2.channel_16_current);
  currents[17] =
      PDH_status_2_channel_17_current_decode(statusFrame2.channel_17_current);
  currents[18] =
      PDH_status_3_channel_18_current_decode(statusFrame3.channel_18_current);
  currents[19] =
      PDH_status_3_channel_19_current_decode(statusFrame3.channel_19_current);
  currents[20] =
      PDH_status_3_channel_20_current_decode(statusFrame3.channel_20_current);
  currents[21] =
      PDH_status_3_channel_21_current_decode(statusFrame3.channel_21_current);
  currents[22] =
      PDH_status_3_channel_22_current_decode(statusFrame3.channel_22_current);
  currents[23] =
      PDH_status_3_channel_23_current_decode(statusFrame3.channel_23_current);
}

uint16_t HAL_GetREVPDHTotalCurrent(HAL_REVPDHHandle handle, int32_t* status) {
  PDH_status_4_t statusFrame = HAL_GetREVPDHStatus4(handle, status);

  if (*status != 0) {
    return 0;
  }

  return PDH_status_4_total_current_decode(statusFrame.total_current);
}

void HAL_SetREVPDHSwitchableChannel(HAL_REVPDHHandle handle, HAL_Bool enabled,
                                    int32_t* status) {
  auto hpdh = REVPDHHandles->Get(handle);
  if (hpdh == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return;
  }

  uint8_t packedData[8] = {0};
  PDH_set_switch_channel_t frame;
  frame.output_set_value = enabled;
  PDH_set_switch_channel_pack(packedData, &frame,
                              PDH_SET_SWITCH_CHANNEL_LENGTH);

  HAL_WriteCANPacket(hpdh->hcan, packedData, PDH_SET_SWITCH_CHANNEL_LENGTH,
                     PDH_SET_SWITCH_CHANNEL_FRAME_API, status);
}

HAL_Bool HAL_GetREVPDHSwitchableChannelState(HAL_REVPDHHandle handle,
                                             int32_t* status) {
  PDH_status_4_t statusFrame = HAL_GetREVPDHStatus4(handle, status);

  if (*status != 0) {
    return 0.0;
  }

  return PDH_status_4_switch_channel_state_decode(
      statusFrame.switch_channel_state);
}

double HAL_GetREVPDHVoltage(HAL_REVPDHHandle handle, int32_t* status) {
  PDH_status_4_t statusFrame = HAL_GetREVPDHStatus4(handle, status);

  if (*status != 0) {
    return 0.0;
  }

  return PDH_status_4_v_bus_decode(statusFrame.v_bus);
}

void HAL_GetREVPDHVersion(HAL_REVPDHHandle handle,
                          HAL_PowerDistributionVersion* version,
                          int32_t* status) {
  std::memset(version, 0, sizeof(*version));
  uint8_t packedData[8] = {0};
  int32_t length = 0;
  uint64_t timestamp = 0;
  PDH_version_t result = {};
  auto hpdh = REVPDHHandles->Get(handle);
  if (hpdh == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return;
  }

  if (hpdh->versionInfo.firmwareMajor > 0) {
    version->firmwareMajor = hpdh->versionInfo.firmwareMajor;
    version->firmwareMinor = hpdh->versionInfo.firmwareMinor;
    version->firmwareFix = hpdh->versionInfo.firmwareFix;
    version->hardwareMajor = hpdh->versionInfo.hardwareMajor;
    version->hardwareMinor = hpdh->versionInfo.hardwareMinor;
    version->uniqueId = hpdh->versionInfo.uniqueId;

    *status = 0;
    return;
  }

  HAL_WriteCANRTRFrame(hpdh->hcan, PDH_VERSION_LENGTH, PDH_VERSION_FRAME_API,
                       status);

  if (*status != 0) {
    return;
  }

  uint32_t timeoutMs = 100;
  for (uint32_t i = 0; i <= timeoutMs; i++) {
    HAL_ReadCANPacketNew(hpdh->hcan, PDH_VERSION_FRAME_API, packedData, &length,
                         &timestamp, status);
    if (*status == 0) {
      break;
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
  }

  if (*status != 0) {
    return;
  }

  PDH_version_unpack(&result, packedData, PDH_VERSION_LENGTH);

  version->firmwareMajor = result.firmware_year;
  version->firmwareMinor = result.firmware_minor;
  version->firmwareFix = result.firmware_fix;
  version->hardwareMinor = result.hardware_minor;
  version->hardwareMajor = result.hardware_major;
  version->uniqueId = result.unique_id;

  hpdh->versionInfo = *version;
}

void HAL_GetREVPDHFaults(HAL_REVPDHHandle handle,
                         HAL_PowerDistributionFaults* faults, int32_t* status) {
  std::memset(faults, 0, sizeof(*faults));
  auto hpdh = REVPDHHandles->Get(handle);
  if (hpdh == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return;
  }

  PDH_status_0_t status0 = HAL_ReadREVPDHStatus0(hpdh->hcan, status);
  PDH_status_1_t status1 = HAL_ReadREVPDHStatus1(hpdh->hcan, status);
  PDH_status_2_t status2 = HAL_ReadREVPDHStatus2(hpdh->hcan, status);
  PDH_status_3_t status3 = HAL_ReadREVPDHStatus3(hpdh->hcan, status);
  PDH_status_4_t status4 = HAL_ReadREVPDHStatus4(hpdh->hcan, status);

  faults->channel0BreakerFault = status0.channel_0_breaker_fault;
  faults->channel1BreakerFault = status0.channel_1_breaker_fault;
  faults->channel2BreakerFault = status0.channel_2_breaker_fault;
  faults->channel3BreakerFault = status0.channel_3_breaker_fault;
  faults->channel4BreakerFault = status1.channel_4_breaker_fault;
  faults->channel5BreakerFault = status1.channel_5_breaker_fault;
  faults->channel6BreakerFault = status1.channel_6_breaker_fault;
  faults->channel7BreakerFault = status1.channel_7_breaker_fault;
  faults->channel8BreakerFault = status2.channel_8_breaker_fault;
  faults->channel9BreakerFault = status2.channel_9_breaker_fault;
  faults->channel10BreakerFault = status2.channel_10_breaker_fault;
  faults->channel11BreakerFault = status2.channel_11_breaker_fault;
  faults->channel12BreakerFault = status3.channel_12_breaker_fault;
  faults->channel13BreakerFault = status3.channel_13_breaker_fault;
  faults->channel14BreakerFault = status3.channel_14_breaker_fault;
  faults->channel15BreakerFault = status3.channel_15_breaker_fault;
  faults->channel16BreakerFault = status3.channel_16_breaker_fault;
  faults->channel17BreakerFault = status3.channel_17_breaker_fault;
  faults->channel18BreakerFault = status3.channel_18_breaker_fault;
  faults->channel19BreakerFault = status3.channel_19_breaker_fault;
  faults->channel20BreakerFault = status3.channel_20_breaker_fault;
  faults->channel21BreakerFault = status3.channel_21_breaker_fault;
  faults->channel22BreakerFault = status3.channel_22_breaker_fault;
  faults->channel23BreakerFault = status3.channel_23_breaker_fault;
  faults->brownout = status4.brownout_fault;
  faults->canWarning = status4.can_warning_fault;
  faults->hardwareFault = status4.hardware_fault;
}

void HAL_GetREVPDHStickyFaults(HAL_REVPDHHandle handle,
                               HAL_PowerDistributionStickyFaults* stickyFaults,
                               int32_t* status) {
  std::memset(stickyFaults, 0, sizeof(*stickyFaults));
  auto hpdh = REVPDHHandles->Get(handle);
  if (hpdh == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return;
  }

  PDH_status_4_t status4 = HAL_ReadREVPDHStatus4(hpdh->hcan, status);

  stickyFaults->channel0BreakerFault = status4.sticky_ch0_breaker_fault;
  stickyFaults->channel1BreakerFault = status4.sticky_ch1_breaker_fault;
  stickyFaults->channel2BreakerFault = status4.sticky_ch2_breaker_fault;
  stickyFaults->channel3BreakerFault = status4.sticky_ch3_breaker_fault;
  stickyFaults->channel4BreakerFault = status4.sticky_ch4_breaker_fault;
  stickyFaults->channel5BreakerFault = status4.sticky_ch5_breaker_fault;
  stickyFaults->channel6BreakerFault = status4.sticky_ch6_breaker_fault;
  stickyFaults->channel7BreakerFault = status4.sticky_ch7_breaker_fault;
  stickyFaults->channel8BreakerFault = status4.sticky_ch8_breaker_fault;
  stickyFaults->channel9BreakerFault = status4.sticky_ch9_breaker_fault;
  stickyFaults->channel10BreakerFault = status4.sticky_ch10_breaker_fault;
  stickyFaults->channel11BreakerFault = status4.sticky_ch11_breaker_fault;
  stickyFaults->channel12BreakerFault = status4.sticky_ch12_breaker_fault;
  stickyFaults->channel13BreakerFault = status4.sticky_ch13_breaker_fault;
  stickyFaults->channel14BreakerFault = status4.sticky_ch14_breaker_fault;
  stickyFaults->channel15BreakerFault = status4.sticky_ch15_breaker_fault;
  stickyFaults->channel16BreakerFault = status4.sticky_ch16_breaker_fault;
  stickyFaults->channel17BreakerFault = status4.sticky_ch17_breaker_fault;
  stickyFaults->channel18BreakerFault = status4.sticky_ch18_breaker_fault;
  stickyFaults->channel19BreakerFault = status4.sticky_ch19_breaker_fault;
  stickyFaults->channel20BreakerFault = status4.sticky_ch20_breaker_fault;
  stickyFaults->channel21BreakerFault = status4.sticky_ch21_breaker_fault;
  stickyFaults->channel22BreakerFault = status4.sticky_ch22_breaker_fault;
  stickyFaults->channel23BreakerFault = status4.sticky_ch23_breaker_fault;
  stickyFaults->brownout = status4.sticky_brownout_fault;
  stickyFaults->canWarning = status4.sticky_can_warning_fault;
  stickyFaults->canBusOff = status4.sticky_can_bus_off_fault;
  stickyFaults->hardwareFault = status4.sticky_hardware_fault;
  stickyFaults->firmwareFault = status4.sticky_firmware_fault;
  stickyFaults->hasReset = status4.sticky_has_reset_fault;
}

void HAL_ClearREVPDHStickyFaults(HAL_REVPDHHandle handle, int32_t* status) {
  auto hpdh = REVPDHHandles->Get(handle);
  if (hpdh == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return;
  }

  uint8_t packedData[8] = {0};
  HAL_WriteCANPacket(hpdh->hcan, packedData, PDH_CLEAR_FAULTS_LENGTH,
                     PDH_CLEAR_FAULTS_FRAME_API, status);
}

uint32_t HAL_StartCANStream(HAL_CANHandle handle, int32_t apiId, int32_t depth,
                            int32_t* status);

void HAL_StartREVPDHStream(HAL_REVPDHHandle handle, int32_t* status) {
  auto hpdh = REVPDHHandles->Get(handle);
  if (hpdh == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return;
  }

  if (hpdh->streamHandleAllocated) {
    *status = RESOURCE_IS_ALLOCATED;
    return;
  }

  hpdh->streamSessionHandles[0] =
      HAL_StartCANStream(hpdh->hcan, PDH_STATUS_0_FRAME_API, 50, status);
  if (*status != 0) {
    return;
  }
  hpdh->streamSessionHandles[1] =
      HAL_StartCANStream(hpdh->hcan, PDH_STATUS_1_FRAME_API, 50, status);
  if (*status != 0) {
    HAL_CAN_CloseStreamSession(hpdh->streamSessionHandles[0]);
    return;
  }
  hpdh->streamSessionHandles[2] =
      HAL_StartCANStream(hpdh->hcan, PDH_STATUS_2_FRAME_API, 50, status);
  if (*status != 0) {
    HAL_CAN_CloseStreamSession(hpdh->streamSessionHandles[0]);
    HAL_CAN_CloseStreamSession(hpdh->streamSessionHandles[1]);
    return;
  }
  hpdh->streamSessionHandles[3] =
      HAL_StartCANStream(hpdh->hcan, PDH_STATUS_3_FRAME_API, 50, status);
  if (*status != 0) {
    HAL_CAN_CloseStreamSession(hpdh->streamSessionHandles[0]);
    HAL_CAN_CloseStreamSession(hpdh->streamSessionHandles[1]);
    HAL_CAN_CloseStreamSession(hpdh->streamSessionHandles[3]);
    return;
  }
  hpdh->streamHandleAllocated = true;
}

HAL_PowerDistributionChannelData* HAL_GetREVPDHStreamData(
    HAL_REVPDHHandle handle, int32_t* count, int32_t* status) {
  auto hpdh = REVPDHHandles->Get(handle);
  if (hpdh == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return nullptr;
  }

  if (!hpdh->streamHandleAllocated) {
    *status = RESOURCE_OUT_OF_RANGE;
    return nullptr;
  }

  *count = 0;
  // 4 streams, 6 channels per stream, 50 depth per stream
  HAL_PowerDistributionChannelData* retData =
      new HAL_PowerDistributionChannelData[4 * 6 * 50];

  HAL_CANStreamMessage messages[50];
  uint32_t messagesRead = 0;
  HAL_CAN_ReadStreamSession(hpdh->streamSessionHandles[0], messages, 50,
                            &messagesRead, status);
  if (*status < 0) {
    goto Exit;
  }

  for (uint32_t i = 0; i < messagesRead; i++) {
    PDH_status_0_t statusFrame0;
    PDH_status_0_unpack(&statusFrame0, messages[i].data, PDH_STATUS_0_LENGTH);
    uint32_t timestamp = messages[i].timeStamp;

    retData[*count].current =
        PDH_status_0_channel_0_current_decode(statusFrame0.channel_0_current);
    retData[*count].channel = 1;
    retData[*count].timestamp = timestamp;
    (*count)++;
    retData[*count].current =
        PDH_status_0_channel_1_current_decode(statusFrame0.channel_1_current);
    retData[*count].channel = 2;
    retData[*count].timestamp = timestamp;
    (*count)++;
    retData[*count].current =
        PDH_status_0_channel_2_current_decode(statusFrame0.channel_2_current);
    retData[*count].channel = 3;
    retData[*count].timestamp = timestamp;
    (*count)++;
    retData[*count].current =
        PDH_status_0_channel_3_current_decode(statusFrame0.channel_3_current);
    retData[*count].channel = 4;
    retData[*count].timestamp = timestamp;
    (*count)++;
    retData[*count].current =
        PDH_status_0_channel_4_current_decode(statusFrame0.channel_4_current);
    retData[*count].channel = 5;
    retData[*count].timestamp = timestamp;
    (*count)++;
    retData[*count].current =
        PDH_status_0_channel_5_current_decode(statusFrame0.channel_5_current);
    retData[*count].channel = 6;
    retData[*count].timestamp = timestamp;
    (*count)++;
  }

  messagesRead = 0;
  HAL_CAN_ReadStreamSession(hpdh->streamSessionHandles[1], messages, 50,
                            &messagesRead, status);
  if (*status < 0) {
    goto Exit;
  }

  for (uint32_t i = 0; i < messagesRead; i++) {
    PDH_status_1_t statusFrame1;
    PDH_status_1_unpack(&statusFrame1, messages[i].data, PDH_STATUS_1_LENGTH);
    uint32_t timestamp = messages[i].timeStamp;

    retData[*count].current =
        PDH_status_1_channel_6_current_decode(statusFrame1.channel_6_current);
    retData[*count].channel = 7;
    retData[*count].timestamp = timestamp;
    (*count)++;
    retData[*count].current =
        PDH_status_1_channel_7_current_decode(statusFrame1.channel_7_current);
    retData[*count].channel = 8;
    retData[*count].timestamp = timestamp;
    (*count)++;
    retData[*count].current =
        PDH_status_1_channel_8_current_decode(statusFrame1.channel_8_current);
    retData[*count].channel = 9;
    retData[*count].timestamp = timestamp;
    (*count)++;
    retData[*count].current =
        PDH_status_1_channel_9_current_decode(statusFrame1.channel_9_current);
    retData[*count].channel = 10;
    retData[*count].timestamp = timestamp;
    (*count)++;
    retData[*count].current =
        PDH_status_1_channel_10_current_decode(statusFrame1.channel_10_current);
    retData[*count].channel = 11;
    retData[*count].timestamp = timestamp;
    (*count)++;
    retData[*count].current =
        PDH_status_1_channel_11_current_decode(statusFrame1.channel_11_current);
    retData[*count].channel = 12;
    retData[*count].timestamp = timestamp;
    (*count)++;
  }

  messagesRead = 0;
  HAL_CAN_ReadStreamSession(hpdh->streamSessionHandles[2], messages, 50,
                            &messagesRead, status);
  if (*status < 0) {
    goto Exit;
  }

  for (uint32_t i = 0; i < messagesRead; i++) {
    PDH_status_2_t statusFrame2;
    PDH_status_2_unpack(&statusFrame2, messages[i].data, PDH_STATUS_2_LENGTH);
    uint32_t timestamp = messages[i].timeStamp;

    retData[*count].current =
        PDH_status_2_channel_12_current_decode(statusFrame2.channel_12_current);
    retData[*count].channel = 13;
    retData[*count].timestamp = timestamp;
    (*count)++;
    retData[*count].current =
        PDH_status_2_channel_13_current_decode(statusFrame2.channel_13_current);
    retData[*count].channel = 14;
    retData[*count].timestamp = timestamp;
    (*count)++;
    retData[*count].current =
        PDH_status_2_channel_14_current_decode(statusFrame2.channel_14_current);
    retData[*count].channel = 15;
    retData[*count].timestamp = timestamp;
    (*count)++;
    retData[*count].current =
        PDH_status_2_channel_15_current_decode(statusFrame2.channel_15_current);
    retData[*count].channel = 16;
    retData[*count].timestamp = timestamp;
    (*count)++;
    retData[*count].current =
        PDH_status_2_channel_16_current_decode(statusFrame2.channel_16_current);
    retData[*count].channel = 17;
    retData[*count].timestamp = timestamp;
    (*count)++;
    retData[*count].current =
        PDH_status_2_channel_17_current_decode(statusFrame2.channel_17_current);
    retData[*count].channel = 18;
    retData[*count].timestamp = timestamp;
    (*count)++;
  }

  messagesRead = 0;
  HAL_CAN_ReadStreamSession(hpdh->streamSessionHandles[3], messages, 50,
                            &messagesRead, status);
  if (*status < 0) {
    goto Exit;
  }

  for (uint32_t i = 0; i < messagesRead; i++) {
    PDH_status_3_t statusFrame3;
    PDH_status_3_unpack(&statusFrame3, messages[i].data, PDH_STATUS_3_LENGTH);
    uint32_t timestamp = messages[i].timeStamp;

    retData[*count].current =
        PDH_status_3_channel_18_current_decode(statusFrame3.channel_18_current);
    retData[*count].channel = 19;
    retData[*count].timestamp = timestamp;
    (*count)++;
    retData[*count].current =
        PDH_status_3_channel_19_current_decode(statusFrame3.channel_19_current);
    retData[*count].channel = 20;
    retData[*count].timestamp = timestamp;
    (*count)++;
    retData[*count].current =
        PDH_status_3_channel_20_current_decode(statusFrame3.channel_20_current);
    retData[*count].channel = 21;
    retData[*count].timestamp = timestamp;
    (*count)++;
    retData[*count].current =
        PDH_status_3_channel_21_current_decode(statusFrame3.channel_21_current);
    retData[*count].channel = 22;
    retData[*count].timestamp = timestamp;
    (*count)++;
    retData[*count].current =
        PDH_status_3_channel_22_current_decode(statusFrame3.channel_22_current);
    retData[*count].channel = 23;
    retData[*count].timestamp = timestamp;
    (*count)++;
    retData[*count].current =
        PDH_status_3_channel_23_current_decode(statusFrame3.channel_23_current);
    retData[*count].channel = 24;
    retData[*count].timestamp = timestamp;
    (*count)++;
  }

Exit:
  if (*status < 0) {
    delete[] retData;
    retData = nullptr;
  }
  return retData;
}

void HAL_StopREVPDHStream(HAL_REVPDHHandle handle, int32_t* status) {
  auto hpdh = REVPDHHandles->Get(handle);
  if (hpdh == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return;
  }

  if (!hpdh->streamHandleAllocated) {
    *status = RESOURCE_OUT_OF_RANGE;
    return;
  }

  HAL_CAN_CloseStreamSession(hpdh->streamSessionHandles[0]);
  HAL_CAN_CloseStreamSession(hpdh->streamSessionHandles[1]);
  HAL_CAN_CloseStreamSession(hpdh->streamSessionHandles[2]);
  HAL_CAN_CloseStreamSession(hpdh->streamSessionHandles[3]);

  hpdh->streamHandleAllocated = false;
}

}  // extern "C"
