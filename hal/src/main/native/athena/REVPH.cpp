// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "hal/REVPH.h"

#include <string>
#include <thread>

#include <fmt/format.h>

#include "HALInitializer.h"
#include "HALInternal.h"
#include "PortsInternal.h"
#include "hal/CANAPI.h"
#include "hal/Errors.h"
#include "hal/handles/IndexedHandleResource.h"
#include "rev/PHFrames.h"

using namespace hal;

static constexpr HAL_CANManufacturer manufacturer =
    HAL_CANManufacturer::HAL_CAN_Man_kREV;

static constexpr HAL_CANDeviceType deviceType =
    HAL_CANDeviceType::HAL_CAN_Dev_kPneumatics;

static constexpr int32_t kDefaultControlPeriod = 20;
static constexpr uint8_t kDefaultCompressorDuty = 255;
static constexpr uint8_t kDefaultPressureTarget = 120;
static constexpr uint8_t kDefaultPressureHysteresis = 60;

#define HAL_REVPH_MAX_PULSE_TIME 65534

static constexpr uint32_t APIFromExtId(uint32_t extId) {
  return (extId >> 6) & 0x3FF;
}

static constexpr uint32_t PH_STATUS_0_FRAME_API =
    APIFromExtId(PH_STATUS_0_FRAME_ID);
static constexpr uint32_t PH_STATUS_1_FRAME_API =
    APIFromExtId(PH_STATUS_1_FRAME_ID);

static constexpr uint32_t PH_SET_ALL_FRAME_API =
    APIFromExtId(PH_SET_ALL_FRAME_ID);
static constexpr uint32_t PH_PULSE_ONCE_FRAME_API =
    APIFromExtId(PH_PULSE_ONCE_FRAME_ID);

static constexpr uint32_t PH_COMPRESSOR_CONFIG_API =
    APIFromExtId(PH_COMPRESSOR_CONFIG_FRAME_ID);

static constexpr uint32_t PH_CLEAR_FAULTS_FRAME_API =
    APIFromExtId(PH_CLEAR_FAULTS_FRAME_ID);

static constexpr uint32_t PH_VERSION_FRAME_API =
    APIFromExtId(PH_VERSION_FRAME_ID);

static constexpr int32_t kPHFrameStatus0Timeout = 50;
static constexpr int32_t kPHFrameStatus1Timeout = 50;

namespace {

struct REV_PHObj {
  int32_t controlPeriod;
  PH_set_all_t desiredSolenoidsState;
  wpi::mutex solenoidLock;
  HAL_CANHandle hcan;
  std::string previousAllocation;
  HAL_REVPHVersion versionInfo;
};

}  // namespace

static IndexedHandleResource<HAL_REVPHHandle, REV_PHObj, 63,
                             HAL_HandleEnum::REVPH>* REVPHHandles;

namespace hal::init {
void InitializeREVPH() {
  static IndexedHandleResource<HAL_REVPHHandle, REV_PHObj, kNumREVPHModules,
                               HAL_HandleEnum::REVPH>
      rH;
  REVPHHandles = &rH;
}
}  // namespace hal::init

static PH_status_0_t HAL_ReadREVPHStatus0(HAL_CANHandle hcan, int32_t* status) {
  uint8_t packedData[8] = {0};
  int32_t length = 0;
  uint64_t timestamp = 0;
  PH_status_0_t result = {};

  HAL_ReadCANPacketTimeout(hcan, PH_STATUS_0_FRAME_API, packedData, &length,
                           &timestamp, kPHFrameStatus0Timeout * 2, status);

  if (*status != 0) {
    return result;
  }

  PH_status_0_unpack(&result, packedData, PH_STATUS_0_LENGTH);

  return result;
}

static PH_status_1_t HAL_ReadREVPHStatus1(HAL_CANHandle hcan, int32_t* status) {
  uint8_t packedData[8] = {0};
  int32_t length = 0;
  uint64_t timestamp = 0;
  PH_status_1_t result = {};

  HAL_ReadCANPacketTimeout(hcan, PH_STATUS_1_FRAME_API, packedData, &length,
                           &timestamp, kPHFrameStatus1Timeout * 2, status);

  if (*status != 0) {
    return result;
  }

  PH_status_1_unpack(&result, packedData, PH_STATUS_1_LENGTH);

  return result;
}

enum REV_SolenoidState {
  kSolenoidDisabled = 0,
  kSolenoidEnabled,
  kSolenoidControlledViaPulse
};

static void HAL_UpdateDesiredREVPHSolenoidState(REV_PHObj* hph,
                                                int32_t solenoid,
                                                REV_SolenoidState state) {
  switch (solenoid) {
    case 0:
      hph->desiredSolenoidsState.channel_0 = state;
      break;
    case 1:
      hph->desiredSolenoidsState.channel_1 = state;
      break;
    case 2:
      hph->desiredSolenoidsState.channel_2 = state;
      break;
    case 3:
      hph->desiredSolenoidsState.channel_3 = state;
      break;
    case 4:
      hph->desiredSolenoidsState.channel_4 = state;
      break;
    case 5:
      hph->desiredSolenoidsState.channel_5 = state;
      break;
    case 6:
      hph->desiredSolenoidsState.channel_6 = state;
      break;
    case 7:
      hph->desiredSolenoidsState.channel_7 = state;
      break;
    case 8:
      hph->desiredSolenoidsState.channel_8 = state;
      break;
    case 9:
      hph->desiredSolenoidsState.channel_9 = state;
      break;
    case 10:
      hph->desiredSolenoidsState.channel_10 = state;
      break;
    case 11:
      hph->desiredSolenoidsState.channel_11 = state;
      break;
    case 12:
      hph->desiredSolenoidsState.channel_12 = state;
      break;
    case 13:
      hph->desiredSolenoidsState.channel_13 = state;
      break;
    case 14:
      hph->desiredSolenoidsState.channel_14 = state;
      break;
    case 15:
      hph->desiredSolenoidsState.channel_15 = state;
      break;
  }
}

static void HAL_SendREVPHSolenoidsState(REV_PHObj* hph, int32_t* status) {
  uint8_t packedData[PH_SET_ALL_LENGTH] = {0};
  PH_set_all_pack(packedData, &(hph->desiredSolenoidsState), PH_SET_ALL_LENGTH);
  HAL_WriteCANPacketRepeating(hph->hcan, packedData, PH_SET_ALL_LENGTH,
                              PH_SET_ALL_FRAME_API, hph->controlPeriod, status);
}

static HAL_Bool HAL_CheckREVPHPulseTime(int32_t time) {
  return ((time > 0) && (time <= HAL_REVPH_MAX_PULSE_TIME)) ? 1 : 0;
}

HAL_REVPHHandle HAL_InitializeREVPH(int32_t module,
                                    const char* allocationLocation,
                                    int32_t* status) {
  hal::init::CheckInit();
  if (!HAL_CheckREVPHModuleNumber(module)) {
    *status = RESOURCE_OUT_OF_RANGE;
    hal::SetLastErrorIndexOutOfRange(status, "Invalid Index for REV PH", 1,
                                     kNumREVPHModules, module);
    return HAL_kInvalidHandle;
  }

  HAL_REVPHHandle handle;
  // Module starts at 1
  auto hph = REVPHHandles->Allocate(module - 1, &handle, status);
  if (*status != 0) {
    if (hph) {
      hal::SetLastErrorPreviouslyAllocated(status, "REV PH", module,
                                           hph->previousAllocation);
    } else {
      hal::SetLastErrorIndexOutOfRange(status, "Invalid Index for REV PH", 1,
                                       kNumREVPHModules, module);
    }
    return HAL_kInvalidHandle;  // failed to allocate. Pass error back.
  }

  HAL_CANHandle hcan =
      HAL_InitializeCAN(manufacturer, module, deviceType, status);

  if (*status != 0) {
    REVPHHandles->Free(handle);
    return HAL_kInvalidHandle;
  }

  hph->previousAllocation = allocationLocation ? allocationLocation : "";
  hph->hcan = hcan;
  hph->controlPeriod = kDefaultControlPeriod;
  std::memset(&hph->desiredSolenoidsState, 0,
              sizeof(hph->desiredSolenoidsState));
  std::memset(&hph->versionInfo, 0, sizeof(hph->versionInfo));

  int32_t can_status = 0;

  // Start closed-loop compressor control by starting solenoid state updates
  HAL_SendREVPHSolenoidsState(hph.get(), &can_status);

  return handle;
}

void HAL_FreeREVPH(HAL_REVPHHandle handle) {
  auto hph = REVPHHandles->Get(handle);
  if (hph) {
    HAL_CleanCAN(hph->hcan);
  }

  REVPHHandles->Free(handle);
}

HAL_Bool HAL_CheckREVPHModuleNumber(int32_t module) {
  return module >= 1 && module <= kNumREVPHModules;
}

HAL_Bool HAL_CheckREVPHSolenoidChannel(int32_t channel) {
  return channel >= 0 && channel < kNumREVPHChannels;
}

HAL_Bool HAL_GetREVPHCompressor(HAL_REVPHHandle handle, int32_t* status) {
  auto ph = REVPHHandles->Get(handle);
  if (ph == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return false;
  }

  PH_status_0_t status0 = HAL_ReadREVPHStatus0(ph->hcan, status);

  if (*status != 0) {
    return false;
  }

  return status0.compressor_on;
}

void HAL_SetREVPHCompressorConfig(HAL_REVPHHandle handle,
                                  const HAL_REVPHCompressorConfig* config,
                                  int32_t* status) {
  auto ph = REVPHHandles->Get(handle);
  if (ph == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return;
  }

  PH_compressor_config_t frameData;
  frameData.minimum_tank_pressure =
      PH_compressor_config_minimum_tank_pressure_encode(
          config->minAnalogVoltage);
  frameData.maximum_tank_pressure =
      PH_compressor_config_maximum_tank_pressure_encode(
          config->maxAnalogVoltage);
  frameData.force_disable = config->forceDisable;
  frameData.use_digital = config->useDigital;

  uint8_t packedData[PH_COMPRESSOR_CONFIG_LENGTH] = {0};
  PH_compressor_config_pack(packedData, &frameData,
                            PH_COMPRESSOR_CONFIG_LENGTH);
  HAL_WriteCANPacket(ph->hcan, packedData, PH_COMPRESSOR_CONFIG_LENGTH,
                     PH_COMPRESSOR_CONFIG_API, status);
}

void HAL_SetREVPHClosedLoopControlDisabled(HAL_REVPHHandle handle,
                                           int32_t* status) {
  HAL_REVPHCompressorConfig config = {0, 0, 0, 0};
  config.forceDisable = true;

  HAL_SetREVPHCompressorConfig(handle, &config, status);
}

void HAL_SetREVPHClosedLoopControlDigital(HAL_REVPHHandle handle,
                                          int32_t* status) {
  HAL_REVPHCompressorConfig config = {0, 0, 0, 0};
  config.useDigital = true;

  HAL_SetREVPHCompressorConfig(handle, &config, status);
}

void HAL_SetREVPHClosedLoopControlAnalog(HAL_REVPHHandle handle,
                                         double minAnalogVoltage,
                                         double maxAnalogVoltage,
                                         int32_t* status) {
  HAL_REVPHCompressorConfig config = {0, 0, 0, 0};
  config.minAnalogVoltage = minAnalogVoltage;
  config.maxAnalogVoltage = maxAnalogVoltage;

  HAL_SetREVPHCompressorConfig(handle, &config, status);
}

void HAL_SetREVPHClosedLoopControlHybrid(HAL_REVPHHandle handle,
                                         double minAnalogVoltage,
                                         double maxAnalogVoltage,
                                         int32_t* status) {
  HAL_REVPHCompressorConfig config = {0, 0, 0, 0};
  config.minAnalogVoltage = minAnalogVoltage;
  config.maxAnalogVoltage = maxAnalogVoltage;
  config.useDigital = true;

  HAL_SetREVPHCompressorConfig(handle, &config, status);
}

HAL_REVPHCompressorConfigType HAL_GetREVPHCompressorConfig(
    HAL_REVPHHandle handle, int32_t* status) {
  auto ph = REVPHHandles->Get(handle);
  if (ph == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return HAL_REVPHCompressorConfigType_kDisabled;
  }

  PH_status_0_t status0 = HAL_ReadREVPHStatus0(ph->hcan, status);

  if (*status != 0) {
    return HAL_REVPHCompressorConfigType_kDisabled;
  }

  return static_cast<HAL_REVPHCompressorConfigType>(status0.compressor_config);
}

HAL_Bool HAL_GetREVPHPressureSwitch(HAL_REVPHHandle handle, int32_t* status) {
  auto ph = REVPHHandles->Get(handle);
  if (ph == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return false;
  }

  PH_status_0_t status0 = HAL_ReadREVPHStatus0(ph->hcan, status);

  if (*status != 0) {
    return false;
  }

  return status0.digital_sensor;
}

double HAL_GetREVPHCompressorCurrent(HAL_REVPHHandle handle, int32_t* status) {
  auto ph = REVPHHandles->Get(handle);
  if (ph == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return 0;
  }

  PH_status_1_t status1 = HAL_ReadREVPHStatus1(ph->hcan, status);

  if (*status != 0) {
    return 0;
  }

  return PH_status_1_compressor_current_decode(status1.compressor_current);
}

double HAL_GetREVPHAnalogVoltage(HAL_REVPHHandle handle, int32_t channel,
                                 int32_t* status) {
  auto ph = REVPHHandles->Get(handle);
  if (ph == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return 0;
  }

  if (channel < 0 || channel > 1) {
    *status = PARAMETER_OUT_OF_RANGE;
    hal::SetLastErrorIndexOutOfRange(status, "Invalid REV Analog Index", 0, 2,
                                     channel);
    return 0;
  }

  PH_status_0_t status0 = HAL_ReadREVPHStatus0(ph->hcan, status);

  if (*status != 0) {
    return 0;
  }

  if (channel == 0) {
    return PH_status_0_analog_0_decode(status0.analog_0);
  }
  return PH_status_0_analog_1_decode(status0.analog_1);
}

double HAL_GetREVPHVoltage(HAL_REVPHHandle handle, int32_t* status) {
  auto ph = REVPHHandles->Get(handle);
  if (ph == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return 0;
  }

  PH_status_1_t status1 = HAL_ReadREVPHStatus1(ph->hcan, status);

  if (*status != 0) {
    return 0;
  }

  return PH_status_1_v_bus_decode(status1.v_bus);
}

double HAL_GetREVPH5VVoltage(HAL_REVPHHandle handle, int32_t* status) {
  auto ph = REVPHHandles->Get(handle);
  if (ph == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return 0;
  }

  PH_status_1_t status1 = HAL_ReadREVPHStatus1(ph->hcan, status);

  if (*status != 0) {
    return 0;
  }

  return PH_status_1_supply_voltage_5_v_decode(status1.supply_voltage_5_v);
}

double HAL_GetREVPHSolenoidCurrent(HAL_REVPHHandle handle, int32_t* status) {
  auto ph = REVPHHandles->Get(handle);
  if (ph == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return 0;
  }

  PH_status_1_t status1 = HAL_ReadREVPHStatus1(ph->hcan, status);

  if (*status != 0) {
    return 0;
  }

  return PH_status_1_solenoid_current_decode(status1.solenoid_current);
}

double HAL_GetREVPHSolenoidVoltage(HAL_REVPHHandle handle, int32_t* status) {
  auto ph = REVPHHandles->Get(handle);
  if (ph == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return 0;
  }

  PH_status_1_t status1 = HAL_ReadREVPHStatus1(ph->hcan, status);

  if (*status != 0) {
    return 0;
  }

  return PH_status_1_solenoid_voltage_decode(status1.solenoid_voltage);
}

void HAL_GetREVPHVersion(HAL_REVPHHandle handle, HAL_REVPHVersion* version,
                         int32_t* status) {
  std::memset(version, 0, sizeof(*version));
  uint8_t packedData[8] = {0};
  int32_t length = 0;
  uint64_t timestamp = 0;
  PH_version_t result = {};
  auto ph = REVPHHandles->Get(handle);
  if (ph == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return;
  }

  if (ph->versionInfo.firmwareMajor > 0) {
    version->firmwareMajor = ph->versionInfo.firmwareMajor;
    version->firmwareMinor = ph->versionInfo.firmwareMinor;
    version->firmwareFix = ph->versionInfo.firmwareFix;
    version->hardwareMajor = ph->versionInfo.hardwareMajor;
    version->hardwareMinor = ph->versionInfo.hardwareMinor;
    version->uniqueId = ph->versionInfo.uniqueId;

    *status = 0;
    return;
  }

  HAL_WriteCANRTRFrame(ph->hcan, PH_VERSION_LENGTH, PH_VERSION_FRAME_API,
                       status);

  if (*status != 0) {
    return;
  }

  uint32_t timeoutMs = 100;
  for (uint32_t i = 0; i <= timeoutMs; i++) {
    HAL_ReadCANPacketNew(ph->hcan, PH_VERSION_FRAME_API, packedData, &length,
                         &timestamp, status);
    if (*status == 0) {
      break;
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
  }

  if (*status != 0) {
    return;
  }

  PH_version_unpack(&result, packedData, PH_VERSION_LENGTH);

  version->firmwareMajor = result.firmware_year;
  version->firmwareMinor = result.firmware_minor;
  version->firmwareFix = result.firmware_fix;
  version->hardwareMinor = result.hardware_minor;
  version->hardwareMajor = result.hardware_major;
  version->uniqueId = result.unique_id;

  ph->versionInfo = *version;
}

int32_t HAL_GetREVPHSolenoids(HAL_REVPHHandle handle, int32_t* status) {
  auto ph = REVPHHandles->Get(handle);
  if (ph == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return 0;
  }

  PH_status_0_t status0 = HAL_ReadREVPHStatus0(ph->hcan, status);

  if (*status != 0) {
    return 0;
  }

  uint32_t result = status0.channel_0;
  result |= status0.channel_1 << 1;
  result |= status0.channel_2 << 2;
  result |= status0.channel_3 << 3;
  result |= status0.channel_4 << 4;
  result |= status0.channel_5 << 5;
  result |= status0.channel_6 << 6;
  result |= status0.channel_7 << 7;
  result |= status0.channel_8 << 8;
  result |= status0.channel_9 << 9;
  result |= status0.channel_10 << 10;
  result |= status0.channel_11 << 11;
  result |= status0.channel_12 << 12;
  result |= status0.channel_13 << 13;
  result |= status0.channel_14 << 14;
  result |= status0.channel_15 << 15;

  return result;
}

void HAL_SetREVPHSolenoids(HAL_REVPHHandle handle, int32_t mask, int32_t values,
                           int32_t* status) {
  auto ph = REVPHHandles->Get(handle);
  if (ph == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return;
  }

  std::scoped_lock lock{ph->solenoidLock};
  for (int solenoid = 0; solenoid < kNumREVPHChannels; solenoid++) {
    if (mask & (1 << solenoid)) {
      // The mask bit for the solenoid is set, so we update the solenoid state
      REV_SolenoidState desiredSolenoidState =
          values & (1 << solenoid) ? kSolenoidEnabled : kSolenoidDisabled;
      HAL_UpdateDesiredREVPHSolenoidState(ph.get(), solenoid,
                                          desiredSolenoidState);
    }
  }
  HAL_SendREVPHSolenoidsState(ph.get(), status);
}

void HAL_FireREVPHOneShot(HAL_REVPHHandle handle, int32_t index, int32_t durMs,
                          int32_t* status) {
  auto ph = REVPHHandles->Get(handle);
  if (ph == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return;
  }

  if (index >= kNumREVPHChannels || index < 0) {
    *status = PARAMETER_OUT_OF_RANGE;
    hal::SetLastError(
        status,
        fmt::format("Only [0-15] are valid index values. Requested {}", index));
    return;
  }

  if (!HAL_CheckREVPHPulseTime(durMs)) {
    *status = PARAMETER_OUT_OF_RANGE;
    hal::SetLastError(
        status,
        fmt::format("Time not within expected range [0-65534]. Requested {}",
                    durMs));
    return;
  }

  {
    std::scoped_lock lock{ph->solenoidLock};
    HAL_UpdateDesiredREVPHSolenoidState(ph.get(), index,
                                        kSolenoidControlledViaPulse);
    HAL_SendREVPHSolenoidsState(ph.get(), status);
  }

  if (*status != 0) {
    return;
  }

  PH_pulse_once_t pulse = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
  pulse.pulse_length_ms = durMs;

  // Specify which solenoid should be pulsed
  // The protocol supports specifying any number of solenoids to be pulsed at
  // the same time, should that functionality be exposed to users in the future.
  switch (index) {
    case 0:
      pulse.channel_0 = true;
      break;
    case 1:
      pulse.channel_1 = true;
      break;
    case 2:
      pulse.channel_2 = true;
      break;
    case 3:
      pulse.channel_3 = true;
      break;
    case 4:
      pulse.channel_4 = true;
      break;
    case 5:
      pulse.channel_5 = true;
      break;
    case 6:
      pulse.channel_6 = true;
      break;
    case 7:
      pulse.channel_7 = true;
      break;
    case 8:
      pulse.channel_8 = true;
      break;
    case 9:
      pulse.channel_9 = true;
      break;
    case 10:
      pulse.channel_10 = true;
      break;
    case 11:
      pulse.channel_11 = true;
      break;
    case 12:
      pulse.channel_12 = true;
      break;
    case 13:
      pulse.channel_13 = true;
      break;
    case 14:
      pulse.channel_14 = true;
      break;
    case 15:
      pulse.channel_15 = true;
      break;
  }

  // Send pulse command
  uint8_t packedData[PH_PULSE_ONCE_LENGTH] = {0};
  PH_pulse_once_pack(packedData, &pulse, PH_PULSE_ONCE_LENGTH);
  HAL_WriteCANPacket(ph->hcan, packedData, PH_PULSE_ONCE_LENGTH,
                     PH_PULSE_ONCE_FRAME_API, status);
}

void HAL_GetREVPHFaults(HAL_REVPHHandle handle, HAL_REVPHFaults* faults,
                        int32_t* status) {
  std::memset(faults, 0, sizeof(*faults));
  auto ph = REVPHHandles->Get(handle);
  if (ph == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return;
  }

  PH_status_0_t status0 = HAL_ReadREVPHStatus0(ph->hcan, status);
  faults->channel0Fault = status0.channel_0_fault;
  faults->channel1Fault = status0.channel_1_fault;
  faults->channel2Fault = status0.channel_2_fault;
  faults->channel3Fault = status0.channel_3_fault;
  faults->channel4Fault = status0.channel_4_fault;
  faults->channel5Fault = status0.channel_5_fault;
  faults->channel6Fault = status0.channel_6_fault;
  faults->channel7Fault = status0.channel_7_fault;
  faults->channel8Fault = status0.channel_8_fault;
  faults->channel9Fault = status0.channel_9_fault;
  faults->channel10Fault = status0.channel_10_fault;
  faults->channel11Fault = status0.channel_11_fault;
  faults->channel12Fault = status0.channel_12_fault;
  faults->channel13Fault = status0.channel_13_fault;
  faults->channel14Fault = status0.channel_14_fault;
  faults->channel15Fault = status0.channel_15_fault;
  faults->compressorOverCurrent = status0.compressor_oc_fault;
  faults->compressorOpen = status0.compressor_open_fault;
  faults->solenoidOverCurrent = status0.solenoid_oc_fault;
  faults->brownout = status0.brownout_fault;
  faults->canWarning = status0.can_warning_fault;
  faults->hardwareFault = status0.hardware_fault;
}

void HAL_GetREVPHStickyFaults(HAL_REVPHHandle handle,
                              HAL_REVPHStickyFaults* stickyFaults,
                              int32_t* status) {
  std::memset(stickyFaults, 0, sizeof(*stickyFaults));
  auto ph = REVPHHandles->Get(handle);
  if (ph == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return;
  }

  PH_status_1_t status1 = HAL_ReadREVPHStatus1(ph->hcan, status);
  stickyFaults->compressorOverCurrent = status1.sticky_compressor_oc_fault;
  stickyFaults->compressorOpen = status1.sticky_compressor_open_fault;
  stickyFaults->solenoidOverCurrent = status1.sticky_solenoid_oc_fault;
  stickyFaults->brownout = status1.sticky_brownout_fault;
  stickyFaults->canWarning = status1.sticky_can_warning_fault;
  stickyFaults->canBusOff = status1.sticky_can_bus_off_fault;
  stickyFaults->hardwareFault = status1.sticky_hardware_fault;
  stickyFaults->firmwareFault = status1.sticky_firmware_fault;
  stickyFaults->hasReset = status1.sticky_has_reset_fault;
}

void HAL_ClearREVPHStickyFaults(HAL_REVPHHandle handle, int32_t* status) {
  auto ph = REVPHHandles->Get(handle);
  if (ph == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return;
  }

  uint8_t packedData[8] = {0};
  HAL_WriteCANPacket(ph->hcan, packedData, PH_CLEAR_FAULTS_LENGTH,
                     PH_CLEAR_FAULTS_FRAME_API, status);
}

int32_t HAL_GetREVPHSolenoidDisabledList(HAL_REVPHHandle handle,
                                         int32_t* status) {
  auto ph = REVPHHandles->Get(handle);
  if (ph == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return false;
  }

  PH_status_0_t status0 = HAL_ReadREVPHStatus0(ph->hcan, status);
  if (*status != 0) {
    return 0;
  }

  uint32_t solenoidFaults = status0.channel_0_fault;
  solenoidFaults |= status0.channel_1_fault << 1;
  solenoidFaults |= status0.channel_2_fault << 2;
  solenoidFaults |= status0.channel_3_fault << 3;
  solenoidFaults |= status0.channel_4_fault << 4;
  solenoidFaults |= status0.channel_5_fault << 5;
  solenoidFaults |= status0.channel_6_fault << 6;
  solenoidFaults |= status0.channel_7_fault << 7;
  solenoidFaults |= status0.channel_8_fault << 8;
  solenoidFaults |= status0.channel_9_fault << 9;
  solenoidFaults |= status0.channel_10_fault << 10;
  solenoidFaults |= status0.channel_11_fault << 11;
  solenoidFaults |= status0.channel_12_fault << 12;
  solenoidFaults |= status0.channel_13_fault << 13;
  solenoidFaults |= status0.channel_14_fault << 14;
  solenoidFaults |= status0.channel_15_fault << 15;
  return solenoidFaults;
}
