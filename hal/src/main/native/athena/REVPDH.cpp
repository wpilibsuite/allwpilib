/*----------------------------------------------------------------------------*/
/* Copyright (c) 2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "REVPDH.h"

#include <cstring>
#include <fmt/format.h>

#include <hal/Errors.h>
#include <hal/handles/HandlesInternal.h>
#include <hal/handles/IndexedHandleResource.h>
#include "PortsInternal.h"
#include "HALInitializer.h"
#include "HALInternal.h"

#include <hal/CANAPI.h>
#include <hal/CANAPITypes.h>

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
};

}  // namespace

static constexpr uint32_t APIFromExtId(uint32_t extId) {
  return (extId >> 6) & 0x3FF;
}

static constexpr uint32_t PDH_SWITCH_CHANNEL_SET_FRAME_API =
    APIFromExtId(PDH_SWITCH_CHANNEL_SET_FRAME_ID);

static constexpr uint32_t PDH_STATUS0_FRAME_API =
    APIFromExtId(PDH_STATUS0_FRAME_ID);
static constexpr uint32_t PDH_STATUS1_FRAME_API =
    APIFromExtId(PDH_STATUS1_FRAME_ID);
static constexpr uint32_t PDH_STATUS2_FRAME_API =
    APIFromExtId(PDH_STATUS2_FRAME_ID);
static constexpr uint32_t PDH_STATUS3_FRAME_API =
    APIFromExtId(PDH_STATUS3_FRAME_ID);
static constexpr uint32_t PDH_STATUS4_FRAME_API =
    APIFromExtId(PDH_STATUS4_FRAME_ID);

static constexpr uint32_t PDH_CLEAR_FAULTS_FRAME_API =
    APIFromExtId(PDH_CLEAR_FAULTS_FRAME_ID);

static constexpr uint32_t PDH_IDENTIFY_FRAME_API =
    APIFromExtId(PDH_IDENTIFY_FRAME_ID);

static constexpr uint32_t PDH_VERSION_FRAME_API =
    APIFromExtId(PDH_VERSION_FRAME_ID);

static constexpr uint32_t PDH_CONFIGURE_HR_CHANNEL_FRAME_API =
    APIFromExtId(PDH_CONFIGURE_HR_CHANNEL_FRAME_ID);

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

static PDH_status0_t HAL_REV_ReadPDHStatus0(HAL_CANHandle hcan,
                                            int32_t* status) {
  uint8_t packedData[8] = {0};
  int32_t length = 0;
  uint64_t timestamp = 0;
  PDH_status0_t result = {};

  HAL_ReadCANPacketTimeout(hcan, PDH_STATUS0_FRAME_API, packedData, &length,
                           &timestamp, kPDHFrameStatus0Timeout * 2, status);

  if (*status != 0) {
    return result;
  }

  PDH_status0_unpack(&result, packedData, PDH_STATUS0_LENGTH);

  return result;
}

static PDH_status1_t HAL_REV_ReadPDHStatus1(HAL_CANHandle hcan,
                                            int32_t* status) {
  uint8_t packedData[8] = {0};
  int32_t length = 0;
  uint64_t timestamp = 0;
  PDH_status1_t result = {};

  HAL_ReadCANPacketTimeout(hcan, PDH_STATUS1_FRAME_API, packedData, &length,
                           &timestamp, kPDHFrameStatus1Timeout * 2, status);

  if (*status != 0) {
    return result;
  }

  PDH_status1_unpack(&result, packedData, PDH_STATUS1_LENGTH);

  return result;
}

static PDH_status2_t HAL_REV_ReadPDHStatus2(HAL_CANHandle hcan,
                                            int32_t* status) {
  uint8_t packedData[8] = {0};
  int32_t length = 0;
  uint64_t timestamp = 0;
  PDH_status2_t result = {};

  HAL_ReadCANPacketTimeout(hcan, PDH_STATUS2_FRAME_API, packedData, &length,
                           &timestamp, kPDHFrameStatus2Timeout * 2, status);

  if (*status != 0) {
    return result;
  }

  PDH_status2_unpack(&result, packedData, PDH_STATUS2_LENGTH);

  return result;
}

static PDH_status3_t HAL_REV_ReadPDHStatus3(HAL_CANHandle hcan,
                                            int32_t* status) {
  uint8_t packedData[8] = {0};
  int32_t length = 0;
  uint64_t timestamp = 0;
  PDH_status3_t result = {};

  HAL_ReadCANPacketTimeout(hcan, PDH_STATUS3_FRAME_API, packedData, &length,
                           &timestamp, kPDHFrameStatus3Timeout * 2, status);

  if (*status != 0) {
    return result;
  }

  PDH_status3_unpack(&result, packedData, PDH_STATUS3_LENGTH);

  return result;
}

static PDH_status4_t HAL_REV_ReadPDHStatus4(HAL_CANHandle hcan,
                                            int32_t* status) {
  uint8_t packedData[8] = {0};
  int32_t length = 0;
  uint64_t timestamp = 0;
  PDH_status4_t result = {};

  HAL_ReadCANPacketTimeout(hcan, PDH_STATUS4_FRAME_API, packedData, &length,
                           &timestamp, kPDHFrameStatus4Timeout * 2, status);

  if (*status != 0) {
    return result;
  }

  PDH_status4_unpack(&result, packedData, PDH_STATUS4_LENGTH);

  return result;
}

/**
 * Helper function for the individual getter functions for status 4
 */
PDH_status4_t HAL_REV_GetPDHStatus4(HAL_REVPDHHandle handle, int32_t* status) {
  PDH_status4_t statusFrame = {};
  auto hpdh = REVPDHHandles->Get(handle);
  if (hpdh == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return statusFrame;
  }

  statusFrame = HAL_REV_ReadPDHStatus4(hpdh->hcan, status);
  return statusFrame;
}

HAL_REVPDHHandle HAL_REV_InitializePDH(int32_t module,
                                       const char* allocationLocation,
                                       int32_t* status) {
  hal::init::CheckInit();
  if (!HAL_REV_CheckPDHModuleNumber(module)) {
    *status = RESOURCE_OUT_OF_RANGE;
    return HAL_kInvalidHandle;
  }

  HAL_REVPDHHandle handle;
  auto hpdh = REVPDHHandles->Allocate(module, &handle, status);
  if (*status != 0) {
    if (hpdh) {
      hal::SetLastErrorPreviouslyAllocated(status, "REV PDH", module,
                                           hpdh->previousAllocation);
    } else {
      hal::SetLastErrorIndexOutOfRange(status, "Invalid Index for REV PDH", 0,
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

  return handle;
}

void HAL_REV_FreePDH(HAL_REVPDHHandle handle) {
  auto hpdh = REVPDHHandles->Get(handle);
  if (hpdh == nullptr) {
    return;
  }

  HAL_CleanCAN(hpdh->hcan);

  REVPDHHandles->Free(handle);
}

int32_t HAL_REV_GetPDHModuleNumber(HAL_REVPDHHandle handle, int32_t* status) {
  return hal::getHandleIndex(handle);
}

HAL_Bool HAL_REV_CheckPDHModuleNumber(int32_t module) {
  return ((module >= 1) && (module < kNumREVPDHModules)) ? 1 : 0;
}

HAL_Bool HAL_REV_CheckPDHChannelNumber(int32_t channel) {
  return ((channel >= 0) && (channel < kNumREVPDHChannels)) ? 1 : 0;
}

double HAL_REV_GetPDHChannelCurrent(HAL_REVPDHHandle handle, int32_t channel,
                                    int32_t* status) {
  auto hpdh = REVPDHHandles->Get(handle);
  if (hpdh == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return 0;
  }

  if (!HAL_REV_CheckPDHChannelNumber(channel)) {
    *status = RESOURCE_OUT_OF_RANGE;
    return 0;
  }

  // Determine what periodic status the channel is in
  if (channel < 6) {
    // Periodic status 0
    PDH_status0_t statusFrame = HAL_REV_ReadPDHStatus0(hpdh->hcan, status);
    switch (channel) {
      case 0:
        return PDH_status0_channel_0_current_decode(
            statusFrame.channel_0_current);
      case 1:
        return PDH_status0_channel_1_current_decode(
            statusFrame.channel_1_current);
      case 2:
        return PDH_status0_channel_2_current_decode(
            statusFrame.channel_2_current);
      case 3:
        return PDH_status0_channel_3_current_decode(
            statusFrame.channel_3_current);
      case 4:
        return PDH_status0_channel_4_current_decode(
            statusFrame.channel_4_current);
      case 5:
        return PDH_status0_channel_5_current_decode(
            statusFrame.channel_5_current);
    }
  } else if (channel < 12) {
    // Periodic status 1
    PDH_status1_t statusFrame = HAL_REV_ReadPDHStatus1(hpdh->hcan, status);
    switch (channel) {
      case 6:
        return PDH_status1_channel_6_current_decode(
            statusFrame.channel_6_current);
      case 7:
        return PDH_status1_channel_7_current_decode(
            statusFrame.channel_7_current);
      case 8:
        return PDH_status1_channel_8_current_decode(
            statusFrame.channel_8_current);
      case 9:
        return PDH_status1_channel_9_current_decode(
            statusFrame.channel_9_current);
      case 10:
        return PDH_status1_channel_10_current_decode(
            statusFrame.channel_10_current);
      case 11:
        return PDH_status1_channel_11_current_decode(
            statusFrame.channel_11_current);
    }
  } else if (channel < 18) {
    // Periodic status 2
    PDH_status2_t statusFrame = HAL_REV_ReadPDHStatus2(hpdh->hcan, status);
    switch (channel) {
      case 12:
        return PDH_status2_channel_12_current_decode(
            statusFrame.channel_12_current);
      case 13:
        return PDH_status2_channel_13_current_decode(
            statusFrame.channel_13_current);
      case 14:
        return PDH_status2_channel_14_current_decode(
            statusFrame.channel_14_current);
      case 15:
        return PDH_status2_channel_15_current_decode(
            statusFrame.channel_15_current);
      case 16:
        return PDH_status2_channel_16_current_decode(
            statusFrame.channel_16_current);
      case 17:
        return PDH_status2_channel_17_current_decode(
            statusFrame.channel_17_current);
    }
  } else if (channel < 24) {
    // Periodic status 3
    PDH_status3_t statusFrame = HAL_REV_ReadPDHStatus3(hpdh->hcan, status);
    switch (channel) {
      case 18:
        return PDH_status3_channel_18_current_decode(
            statusFrame.channel_18_current);
      case 19:
        return PDH_status3_channel_19_current_decode(
            statusFrame.channel_19_current);
      case 20:
        return PDH_status3_channel_20_current_decode(
            statusFrame.channel_20_current);
      case 21:
        return PDH_status3_channel_21_current_decode(
            statusFrame.channel_21_current);
      case 22:
        return PDH_status3_channel_22_current_decode(
            statusFrame.channel_22_current);
      case 23:
        return PDH_status3_channel_23_current_decode(
            statusFrame.channel_23_current);
    }
  }
  return 0;
}

void HAL_REV_GetPDHAllChannelCurrents(HAL_REVPDHHandle handle, double* currents,
                                      int32_t* status) {
  auto hpdh = REVPDHHandles->Get(handle);
  if (hpdh == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return;
  }

  PDH_status0_t statusFrame0 = HAL_REV_ReadPDHStatus0(hpdh->hcan, status);
  PDH_status1_t statusFrame1 = HAL_REV_ReadPDHStatus1(hpdh->hcan, status);
  PDH_status2_t statusFrame2 = HAL_REV_ReadPDHStatus2(hpdh->hcan, status);
  PDH_status3_t statusFrame3 = HAL_REV_ReadPDHStatus3(hpdh->hcan, status);

  currents[0] =
      PDH_status0_channel_0_current_decode(statusFrame0.channel_0_current);
  currents[1] =
      PDH_status0_channel_1_current_decode(statusFrame0.channel_1_current);
  currents[2] =
      PDH_status0_channel_2_current_decode(statusFrame0.channel_2_current);
  currents[3] =
      PDH_status0_channel_3_current_decode(statusFrame0.channel_3_current);
  currents[4] =
      PDH_status0_channel_4_current_decode(statusFrame0.channel_4_current);
  currents[5] =
      PDH_status0_channel_5_current_decode(statusFrame0.channel_5_current);
  currents[6] =
      PDH_status1_channel_6_current_decode(statusFrame1.channel_6_current);
  currents[7] =
      PDH_status1_channel_7_current_decode(statusFrame1.channel_7_current);
  currents[8] =
      PDH_status1_channel_8_current_decode(statusFrame1.channel_8_current);
  currents[9] =
      PDH_status1_channel_9_current_decode(statusFrame1.channel_9_current);
  currents[10] =
      PDH_status1_channel_10_current_decode(statusFrame1.channel_10_current);
  currents[11] =
      PDH_status1_channel_11_current_decode(statusFrame1.channel_11_current);
  currents[12] =
      PDH_status2_channel_12_current_decode(statusFrame2.channel_12_current);
  currents[13] =
      PDH_status2_channel_13_current_decode(statusFrame2.channel_13_current);
  currents[14] =
      PDH_status2_channel_14_current_decode(statusFrame2.channel_14_current);
  currents[15] =
      PDH_status2_channel_15_current_decode(statusFrame2.channel_15_current);
  currents[16] =
      PDH_status2_channel_16_current_decode(statusFrame2.channel_16_current);
  currents[17] =
      PDH_status2_channel_17_current_decode(statusFrame2.channel_17_current);
  currents[18] =
      PDH_status3_channel_18_current_decode(statusFrame3.channel_18_current);
  currents[19] =
      PDH_status3_channel_19_current_decode(statusFrame3.channel_19_current);
  currents[20] =
      PDH_status3_channel_20_current_decode(statusFrame3.channel_20_current);
  currents[21] =
      PDH_status3_channel_21_current_decode(statusFrame3.channel_21_current);
  currents[22] =
      PDH_status3_channel_22_current_decode(statusFrame3.channel_22_current);
  currents[23] =
      PDH_status3_channel_23_current_decode(statusFrame3.channel_23_current);
}

uint16_t HAL_REV_GetPDHTotalCurrent(HAL_REVPDHHandle handle, int32_t* status) {
  PDH_status4_t statusFrame = HAL_REV_GetPDHStatus4(handle, status);

  if (*status != 0) {
    return 0;
  }

  return PDH_status4_total_current_decode(statusFrame.total_current);
}

void HAL_REV_SetPDHSwitchableChannel(HAL_REVPDHHandle handle, HAL_Bool enabled,
                                     int32_t* status) {
  auto hpdh = REVPDHHandles->Get(handle);
  if (hpdh == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return;
  }

  uint8_t packedData[8] = {0};
  PDH_switch_channel_set_t frame;
  frame.output_set_value = enabled;
  frame.use_system_enable = false;
  PDH_switch_channel_set_pack(packedData, &frame, 1);

  HAL_WriteCANPacket(hpdh->hcan, packedData, PDH_SWITCH_CHANNEL_SET_LENGTH,
                     PDH_SWITCH_CHANNEL_SET_FRAME_API, status);
}

HAL_Bool HAL_REV_GetPDHSwitchableChannelState(HAL_REVPDHHandle handle,
                                              int32_t* status) {
  PDH_status4_t statusFrame = HAL_REV_GetPDHStatus4(handle, status);

  if (*status != 0) {
    return 0.0;
  }

  return PDH_status4_sw_state_decode(statusFrame.sw_state);
}

HAL_Bool HAL_REV_CheckPDHChannelBrownout(HAL_REVPDHHandle handle,
                                         int32_t channel, int32_t* status) {
  auto hpdh = REVPDHHandles->Get(handle);
  if (hpdh == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return 0;
  }

  if (!HAL_REV_CheckPDHChannelNumber(channel)) {
    *status = RESOURCE_OUT_OF_RANGE;
    return 0;
  }

  // Determine what periodic status the channel is in
  if (channel < 4) {
    // Periodic status 0
    PDH_status0_t statusFrame = HAL_REV_ReadPDHStatus0(hpdh->hcan, status);
    switch (channel) {
      case 0:
        return PDH_status0_channel_0_brownout_decode(
            statusFrame.channel_0_brownout);
      case 1:
        return PDH_status0_channel_1_brownout_decode(
            statusFrame.channel_1_brownout);
      case 2:
        return PDH_status0_channel_2_brownout_decode(
            statusFrame.channel_2_brownout);
      case 3:
        return PDH_status0_channel_3_brownout_decode(
            statusFrame.channel_3_brownout);
    }
  } else if (channel < 8) {
    // Periodic status 1
    PDH_status1_t statusFrame = HAL_REV_ReadPDHStatus1(hpdh->hcan, status);
    switch (channel) {
      case 4:
        return PDH_status1_channel_4_brownout_decode(
            statusFrame.channel_4_brownout);
      case 5:
        return PDH_status1_channel_5_brownout_decode(
            statusFrame.channel_5_brownout);
      case 6:
        return PDH_status1_channel_6_brownout_decode(
            statusFrame.channel_6_brownout);
      case 7:
        return PDH_status1_channel_7_brownout_decode(
            statusFrame.channel_7_brownout);
    }
  } else if (channel < 12) {
    // Periodic status 2
    PDH_status2_t statusFrame = HAL_REV_ReadPDHStatus2(hpdh->hcan, status);
    switch (channel) {
      case 8:
        return PDH_status2_channel_8_brownout_decode(
            statusFrame.channel_8_brownout);
      case 9:
        return PDH_status2_channel_9_brownout_decode(
            statusFrame.channel_9_brownout);
      case 10:
        return PDH_status2_channel_10_brownout_decode(
            statusFrame.channel_10_brownout);
      case 11:
        return PDH_status2_channel_11_brownout_decode(
            statusFrame.channel_11_brownout);
    }
  } else if (channel < 24) {
    // Periodic status 3
    PDH_status3_t statusFrame = HAL_REV_ReadPDHStatus3(hpdh->hcan, status);
    switch (channel) {
      case 12:
        return PDH_status3_channel_12_brownout_decode(
            statusFrame.channel_12_brownout);
      case 13:
        return PDH_status3_channel_13_brownout_decode(
            statusFrame.channel_13_brownout);
      case 14:
        return PDH_status3_channel_14_brownout_decode(
            statusFrame.channel_14_brownout);
      case 15:
        return PDH_status3_channel_15_brownout_decode(
            statusFrame.channel_15_brownout);
      case 16:
        return PDH_status3_channel_16_brownout_decode(
            statusFrame.channel_16_brownout);
      case 17:
        return PDH_status3_channel_17_brownout_decode(
            statusFrame.channel_17_brownout);
      case 18:
        return PDH_status3_channel_18_brownout_decode(
            statusFrame.channel_18_brownout);
      case 19:
        return PDH_status3_channel_19_brownout_decode(
            statusFrame.channel_19_brownout);
      case 20:
        return PDH_status3_channel_20_brownout_decode(
            statusFrame.channel_20_brownout);
      case 21:
        return PDH_status3_channel_21_brownout_decode(
            statusFrame.channel_21_brownout);
      case 22:
        return PDH_status3_channel_22_brownout_decode(
            statusFrame.channel_22_brownout);
      case 23:
        return PDH_status3_channel_23_brownout_decode(
            statusFrame.channel_23_brownout);
    }
  }
  return 0;
}

double HAL_REV_GetPDHSupplyVoltage(HAL_REVPDHHandle handle, int32_t* status) {
  PDH_status4_t statusFrame = HAL_REV_GetPDHStatus4(handle, status);

  if (*status != 0) {
    return 0.0;
  }

  return PDH_status4_v_bus_decode(statusFrame.v_bus);
}

HAL_Bool HAL_REV_IsPDHEnabled(HAL_REVPDHHandle handle, int32_t* status) {
  PDH_status4_t statusFrame = HAL_REV_GetPDHStatus4(handle, status);

  if (*status != 0) {
    return false;
  }

  return PDH_status4_system_enable_decode(statusFrame.system_enable);
}

HAL_Bool HAL_REV_CheckPDHBrownout(HAL_REVPDHHandle handle, int32_t* status) {
  PDH_status4_t statusFrame = HAL_REV_GetPDHStatus4(handle, status);

  if (*status != 0) {
    return false;
  }

  return PDH_status4_brownout_decode(statusFrame.brownout);
}

HAL_Bool HAL_REV_CheckPDHOverCurrent(HAL_REVPDHHandle handle, int32_t* status) {
  PDH_status4_t statusFrame = HAL_REV_GetPDHStatus4(handle, status);

  if (*status != 0) {
    return false;
  }

  return PDH_status4_over_current_decode(statusFrame.over_current);
}

HAL_Bool HAL_REV_CheckPDHCANWarning(HAL_REVPDHHandle handle, int32_t* status) {
  PDH_status4_t statusFrame = HAL_REV_GetPDHStatus4(handle, status);

  if (*status != 0) {
    return 0.0;
  }

  return PDH_status4_can_warning_decode(statusFrame.can_warning);
}

HAL_Bool HAL_REV_CheckPDHHardwareFault(HAL_REVPDHHandle handle,
                                       int32_t* status) {
  PDH_status4_t statusFrame = HAL_REV_GetPDHStatus4(handle, status);

  if (*status != 0) {
    return 0.0;
  }

  return PDH_status4_hardware_fault_decode(statusFrame.hardware_fault);
}

HAL_Bool HAL_REV_CheckPDHStickyBrownout(HAL_REVPDHHandle handle,
                                        int32_t* status) {
  PDH_status4_t statusFrame = HAL_REV_GetPDHStatus4(handle, status);

  if (*status != 0) {
    return 0.0;
  }

  return PDH_status4_sticky_brownout_decode(statusFrame.sticky_brownout);
}

HAL_Bool HAL_REV_CheckPDHStickyOverCurrent(HAL_REVPDHHandle handle,
                                           int32_t* status) {
  PDH_status4_t statusFrame = HAL_REV_GetPDHStatus4(handle, status);

  if (*status != 0) {
    return 0.0;
  }

  return PDH_status4_sticky_over_current_decode(
      statusFrame.sticky_over_current);
}

HAL_Bool HAL_REV_CheckPDHStickyCANWarning(HAL_REVPDHHandle handle,
                                          int32_t* status) {
  PDH_status4_t statusFrame = HAL_REV_GetPDHStatus4(handle, status);

  if (*status != 0) {
    return 0.0;
  }

  return PDH_status4_sticky_can_warning_decode(statusFrame.sticky_can_warning);
}

HAL_Bool HAL_REV_CheckPDHStickyCANBusOff(HAL_REVPDHHandle handle,
                                         int32_t* status) {
  PDH_status4_t statusFrame = HAL_REV_GetPDHStatus4(handle, status);

  if (*status != 0) {
    return 0.0;
  }

  return PDH_status4_sticky_can_bus_off_decode(statusFrame.sticky_can_bus_off);
}

HAL_Bool HAL_REV_CheckPDHStickyHardwareFault(HAL_REVPDHHandle handle,
                                             int32_t* status) {
  PDH_status4_t statusFrame = HAL_REV_GetPDHStatus4(handle, status);

  if (*status != 0) {
    return 0.0;
  }

  return PDH_status4_sticky_hardware_fault_decode(
      statusFrame.sticky_hardware_fault);
}

HAL_Bool HAL_REV_CheckPDHStickyFirmwareFault(HAL_REVPDHHandle handle,
                                             int32_t* status) {
  PDH_status4_t statusFrame = HAL_REV_GetPDHStatus4(handle, status);

  if (*status != 0) {
    return 0.0;
  }

  return PDH_status4_sticky_firmware_fault_decode(
      statusFrame.sticky_firmware_fault);
}

HAL_Bool HAL_REV_CheckPDHStickyChannelBrownout(HAL_REVPDHHandle handle,
                                               int32_t channel,
                                               int32_t* status) {
  if (channel < 20 || channel > 23) {
    *status = RESOURCE_OUT_OF_RANGE;
    return 0.0;
  }

  PDH_status4_t statusFrame = HAL_REV_GetPDHStatus4(handle, status);

  if (*status != 0) {
    return 0.0;
  }

  switch (channel) {
    case 20:
      return PDH_status4_sticky_ch20_brownout_decode(
          statusFrame.sticky_ch20_brownout);
    case 21:
      return PDH_status4_sticky_ch21_brownout_decode(
          statusFrame.sticky_ch21_brownout);
    case 22:
      return PDH_status4_sticky_ch22_brownout_decode(
          statusFrame.sticky_ch22_brownout);
    case 23:
      return PDH_status4_sticky_ch23_brownout_decode(
          statusFrame.sticky_ch23_brownout);
  }
  return 0;
}

HAL_Bool HAL_REV_CheckPDHStickyHasReset(HAL_REVPDHHandle handle,
                                        int32_t* status) {
  PDH_status4_t statusFrame = HAL_REV_GetPDHStatus4(handle, status);

  if (*status != 0) {
    return 0.0;
  }

  return PDH_status4_sticky_has_reset_decode(statusFrame.sticky_has_reset);
}

REV_PDH_Version HAL_REV_GetPDHVersion(HAL_REVPDHHandle handle,
                                      int32_t* status) {
  REV_PDH_Version version;
  std::memset(&version, 0, sizeof(version));
  uint8_t packedData[8] = {0};
  int32_t length = 0;
  uint64_t timestamp = 0;
  PDH_version_t result = {};
  auto hpdh = REVPDHHandles->Get(handle);
  if (hpdh == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return version;
  }

  HAL_WriteCANRTRFrame(hpdh->hcan, PDH_VERSION_LENGTH, PDH_VERSION_FRAME_API,
                       status);

  if (*status != 0) {
    return version;
  }

  HAL_ReadCANPacketTimeout(hpdh->hcan, PDH_VERSION_FRAME_API, packedData,
                           &length, &timestamp, kDefaultControlPeriod * 2,
                           status);

  if (*status != 0) {
    return version;
  }

  PDH_version_unpack(&result, packedData, PDH_VERSION_LENGTH);

  version.firmwareMajor = result.firmware_year;
  version.firmwareMinor = result.firmware_minor;
  version.firmwareFix = result.firmware_fix;
  version.hardwareRev = result.hardware_code;
  version.uniqueId = result.unique_id;

  return version;
}

void HAL_REV_ClearPDHFaults(HAL_REVPDHHandle handle, int32_t* status) {
  auto hpdh = REVPDHHandles->Get(handle);
  if (hpdh == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return;
  }

  uint8_t packedData[8] = {0};
  HAL_WriteCANPacket(hpdh->hcan, packedData, PDH_CLEAR_FAULTS_LENGTH,
                     PDH_CLEAR_FAULTS_FRAME_API, status);
}

void HAL_REV_IdentifyPDH(HAL_REVPDHHandle handle, int32_t* status) {
  auto hpdh = REVPDHHandles->Get(handle);
  if (hpdh == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return;
  }

  uint8_t packedData[8] = {0};
  HAL_WriteCANPacket(hpdh->hcan, packedData, PDH_IDENTIFY_LENGTH,
                     PDH_IDENTIFY_FRAME_API, status);
}

}  // extern "C"
