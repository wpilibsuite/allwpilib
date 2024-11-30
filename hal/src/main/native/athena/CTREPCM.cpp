// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "hal/CTREPCM.h"

#include <string>

#include <fmt/format.h>

#include "HALInitializer.h"
#include "HALInternal.h"
#include "PortsInternal.h"
#include "hal/CANAPI.h"
#include "hal/Errors.h"
#include "hal/handles/IndexedHandleResource.h"

using namespace hal;

static constexpr HAL_CANManufacturer manufacturer =
    HAL_CANManufacturer::HAL_CAN_Man_kCTRE;

static constexpr HAL_CANDeviceType deviceType =
    HAL_CANDeviceType::HAL_CAN_Dev_kPneumatics;

static constexpr int32_t Status1 = 0x50;
static constexpr int32_t StatusSolFaults = 0x51;
static constexpr int32_t StatusDebug = 0x52;

static constexpr int32_t Control1 = 0x70;
static constexpr int32_t Control2 = 0x71;
static constexpr int32_t Control3 = 0x72;

static constexpr int32_t TimeoutMs = 100;
static constexpr int32_t SendPeriod = 20;

union PcmStatus {
  uint8_t data[8];
  struct Bits {
    /* Byte 0 */
    unsigned SolenoidBits : 8;
    /* Byte 1 */
    unsigned compressorOn : 1;
    unsigned stickyFaultFuseTripped : 1;
    unsigned stickyFaultCompCurrentTooHigh : 1;
    unsigned faultFuseTripped : 1;
    unsigned faultCompCurrentTooHigh : 1;
    unsigned faultHardwareFailure : 1;
    unsigned isCloseloopEnabled : 1;
    unsigned pressureSwitchEn : 1;
    /* Byte 2*/
    unsigned battVoltage : 8;
    /* Byte 3 */
    unsigned solenoidVoltageTop8 : 8;
    /* Byte 4 */
    unsigned compressorCurrentTop6 : 6;
    unsigned solenoidVoltageBtm2 : 2;
    /* Byte 5 */
    unsigned StickyFault_dItooHigh : 1;
    unsigned Fault_dItooHigh : 1;
    unsigned moduleEnabled : 1;
    unsigned closedLoopOutput : 1;
    unsigned compressorCurrentBtm4 : 4;
    /* Byte 6 */
    unsigned tokenSeedTop8 : 8;
    /* Byte 7 */
    unsigned tokenSeedBtm8 : 8;
  } bits;
};

union PcmControl {
  uint8_t data[8];
  struct Bits {
    /* Byte 0 */
    unsigned tokenTop8 : 8;
    /* Byte 1 */
    unsigned tokenBtm8 : 8;
    /* Byte 2 */
    unsigned solenoidBits : 8;
    /* Byte 3*/
    unsigned reserved : 4;
    unsigned closeLoopOutput : 1;
    unsigned compressorOn : 1;
    unsigned closedLoopEnable : 1;
    unsigned clearStickyFaults : 1;
    /* Byte 4 */
    unsigned OneShotField_h8 : 8;
    /* Byte 5 */
    unsigned OneShotField_l8 : 8;
  } bits;
};

struct PcmControlSetOneShotDur {
  uint8_t sol10MsPerUnit[8];
};

union PcmStatusFault {
  uint8_t data[8];
  struct Bits {
    /* Byte 0 */
    unsigned SolenoidDisabledList : 8;
    /* Byte 1 */
    unsigned reserved_bit0 : 1;
    unsigned reserved_bit1 : 1;
    unsigned reserved_bit2 : 1;
    unsigned reserved_bit3 : 1;
    unsigned StickyFault_CompNoCurrent : 1;
    unsigned Fault_CompNoCurrent : 1;
    unsigned StickyFault_SolenoidJumper : 1;
    unsigned Fault_SolenoidJumper : 1;
  } bits;
};

union PcmDebug {
  uint8_t data[8];
  struct Bits {
    unsigned tokFailsTop8 : 8;
    unsigned tokFailsBtm8 : 8;
    unsigned lastFailedTokTop8 : 8;
    unsigned lastFailedTokBtm8 : 8;
    unsigned tokSuccessTop8 : 8;
    unsigned tokSuccessBtm8 : 8;
  } bits;
};

namespace {
struct PCM {
  HAL_CANHandle canHandle;
  wpi::mutex lock;
  std::string previousAllocation;
  PcmControl control;
  PcmControlSetOneShotDur oneShot;
};
}  // namespace

static IndexedHandleResource<HAL_CTREPCMHandle, PCM, kNumCTREPCMModules,
                             HAL_HandleEnum::CTREPCM>* pcmHandles;

namespace hal::init {
void InitializeCTREPCM() {
  static IndexedHandleResource<HAL_CTREPCMHandle, PCM, kNumCTREPCMModules,
                               HAL_HandleEnum::CTREPCM>
      pH;
  pcmHandles = &pH;
}
}  // namespace hal::init

#define READ_PACKET(type, frame, failureValue)                             \
  auto pcm = pcmHandles->Get(handle);                                      \
  if (pcm == nullptr) {                                                    \
    *status = HAL_HANDLE_ERROR;                                            \
    return failureValue;                                                   \
  }                                                                        \
  type pcmStatus;                                                          \
  int32_t length = 0;                                                      \
  uint64_t receivedTimestamp = 0;                                          \
  HAL_ReadCANPacketTimeout(pcm->canHandle, frame, pcmStatus.data, &length, \
                           &receivedTimestamp, TimeoutMs, status);         \
  if (*status != 0) {                                                      \
    return failureValue;                                                   \
  }

#define READ_STATUS(failureValue) READ_PACKET(PcmStatus, Status1, failureValue)
#define READ_SOL_FAULTS(failureValue) \
  READ_PACKET(PcmStatusFault, StatusSolFaults, failureValue)

static void SendControl(PCM* pcm, int32_t* status) {
  HAL_WriteCANPacketRepeating(pcm->canHandle, pcm->control.data, 8, Control1,
                              SendPeriod, status);
}

extern "C" {

HAL_CTREPCMHandle HAL_InitializeCTREPCM(int32_t module,
                                        const char* allocationLocation,
                                        int32_t* status) {
  hal::init::CheckInit();

  HAL_CTREPCMHandle handle;
  auto pcm = pcmHandles->Allocate(module, &handle, status);

  if (*status != 0) {
    if (pcm) {
      hal::SetLastErrorPreviouslyAllocated(status, "CTRE PCM", module,
                                           pcm->previousAllocation);
    } else {
      hal::SetLastErrorIndexOutOfRange(status, "Invalid Index for CTRE PCM", 0,
                                       kNumCTREPCMModules - 1, module);
    }
    return HAL_kInvalidHandle;  // failed to allocate. Pass error back.
  }

  pcm->canHandle = HAL_InitializeCAN(manufacturer, module, deviceType, status);
  if (*status != 0) {
    pcmHandles->Free(handle);
    return HAL_kInvalidHandle;
  }

  std::memset(&pcm->oneShot, 0, sizeof(pcm->oneShot));
  std::memset(&pcm->control, 0, sizeof(pcm->control));

  pcm->previousAllocation = allocationLocation ? allocationLocation : "";

  // Enable closed loop control
  HAL_SetCTREPCMClosedLoopControl(handle, true, status);
  if (*status != 0) {
    HAL_FreeCTREPCM(handle);
    return HAL_kInvalidHandle;
  }
  return handle;
}

void HAL_FreeCTREPCM(HAL_CTREPCMHandle handle) {
  auto pcm = pcmHandles->Get(handle);
  if (pcm) {
    HAL_CleanCAN(pcm->canHandle);
  }
  pcmHandles->Free(handle);
}

HAL_Bool HAL_CheckCTREPCMSolenoidChannel(int32_t channel) {
  return channel < kNumCTRESolenoidChannels && channel >= 0;
}

HAL_Bool HAL_GetCTREPCMCompressor(HAL_CTREPCMHandle handle, int32_t* status) {
  READ_STATUS(false);
  return pcmStatus.bits.compressorOn;
}

void HAL_SetCTREPCMClosedLoopControl(HAL_CTREPCMHandle handle, HAL_Bool enabled,
                                     int32_t* status) {
  auto pcm = pcmHandles->Get(handle);
  if (pcm == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return;
  }

  int32_t can_status = 0;

  std::scoped_lock lock{pcm->lock};
  pcm->control.bits.closedLoopEnable = enabled ? 1 : 0;
  SendControl(pcm.get(), &can_status);
}

HAL_Bool HAL_GetCTREPCMClosedLoopControl(HAL_CTREPCMHandle handle,
                                         int32_t* status) {
  READ_STATUS(false);
  return pcmStatus.bits.isCloseloopEnabled;
}

HAL_Bool HAL_GetCTREPCMPressureSwitch(HAL_CTREPCMHandle handle,
                                      int32_t* status) {
  READ_STATUS(false);
  return pcmStatus.bits.pressureSwitchEn;
}

double HAL_GetCTREPCMCompressorCurrent(HAL_CTREPCMHandle handle,
                                       int32_t* status) {
  READ_STATUS(0);
  uint32_t result = pcmStatus.bits.compressorCurrentTop6;
  result <<= 4;
  result |= pcmStatus.bits.compressorCurrentBtm4;
  return result * 0.03125; /* 5.5 fixed pt value in Amps */
}

HAL_Bool HAL_GetCTREPCMCompressorCurrentTooHighFault(HAL_CTREPCMHandle handle,
                                                     int32_t* status) {
  READ_STATUS(false);
  return pcmStatus.bits.faultCompCurrentTooHigh;
}

HAL_Bool HAL_GetCTREPCMCompressorCurrentTooHighStickyFault(
    HAL_CTREPCMHandle handle, int32_t* status) {
  READ_STATUS(false);
  return pcmStatus.bits.stickyFaultCompCurrentTooHigh;
}
HAL_Bool HAL_GetCTREPCMCompressorShortedStickyFault(HAL_CTREPCMHandle handle,
                                                    int32_t* status) {
  READ_STATUS(false);
  return pcmStatus.bits.Fault_dItooHigh;
}
HAL_Bool HAL_GetCTREPCMCompressorShortedFault(HAL_CTREPCMHandle handle,
                                              int32_t* status) {
  READ_STATUS(false);
  return pcmStatus.bits.StickyFault_dItooHigh;
}
HAL_Bool HAL_GetCTREPCMCompressorNotConnectedStickyFault(
    HAL_CTREPCMHandle handle, int32_t* status) {
  READ_SOL_FAULTS(false);
  return pcmStatus.bits.StickyFault_CompNoCurrent;
}
HAL_Bool HAL_GetCTREPCMCompressorNotConnectedFault(HAL_CTREPCMHandle handle,
                                                   int32_t* status) {
  READ_SOL_FAULTS(false);
  return pcmStatus.bits.Fault_CompNoCurrent;
}

int32_t HAL_GetCTREPCMSolenoids(HAL_CTREPCMHandle handle, int32_t* status) {
  READ_STATUS(0);
  return pcmStatus.bits.SolenoidBits & 0xFF;
}

void HAL_SetCTREPCMSolenoids(HAL_CTREPCMHandle handle, int32_t mask,
                             int32_t values, int32_t* status) {
  auto pcm = pcmHandles->Get(handle);
  if (pcm == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return;
  }

  uint8_t smallMask = mask & 0xFF;
  uint8_t smallValues =
      (values & 0xFF) & smallMask;  // Enforce only masked values are set
  uint8_t invertMask = ~smallMask;

  std::scoped_lock lock{pcm->lock};
  uint8_t existingValue = invertMask & pcm->control.bits.solenoidBits;
  pcm->control.bits.solenoidBits = existingValue | smallValues;
  SendControl(pcm.get(), status);
}

int32_t HAL_GetCTREPCMSolenoidDisabledList(HAL_CTREPCMHandle handle,
                                           int32_t* status) {
  READ_SOL_FAULTS(0);
  return pcmStatus.bits.SolenoidDisabledList;
}

HAL_Bool HAL_GetCTREPCMSolenoidVoltageStickyFault(HAL_CTREPCMHandle handle,
                                                  int32_t* status) {
  READ_STATUS(false);
  return pcmStatus.bits.stickyFaultFuseTripped;
}

HAL_Bool HAL_GetCTREPCMSolenoidVoltageFault(HAL_CTREPCMHandle handle,
                                            int32_t* status) {
  READ_STATUS(false);
  return pcmStatus.bits.faultFuseTripped;
}

void HAL_ClearAllCTREPCMStickyFaults(HAL_CTREPCMHandle handle,
                                     int32_t* status) {
  auto pcm = pcmHandles->Get(handle);
  if (pcm == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return;
  }
  uint8_t controlData[] = {0, 0, 0, 0x80};
  HAL_WriteCANPacket(pcm->canHandle, controlData, sizeof(controlData), Control2,
                     status);
}

void HAL_FireCTREPCMOneShot(HAL_CTREPCMHandle handle, int32_t index,
                            int32_t* status) {
  if (index > 7 || index < 0) {
    *status = PARAMETER_OUT_OF_RANGE;
    hal::SetLastError(
        status,
        fmt::format("Only [0-7] are valid index values. Requested {}", index));
    return;
  }

  auto pcm = pcmHandles->Get(handle);
  if (pcm == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return;
  }

  std::scoped_lock lock{pcm->lock};
  uint16_t oneShotField = pcm->control.bits.OneShotField_h8;
  oneShotField <<= 8;
  oneShotField |= pcm->control.bits.OneShotField_l8;

  uint16_t shift = 2 * index;
  uint16_t mask = 3;
  uint8_t chBits = (oneShotField >> shift) & mask;
  chBits = (chBits % 3) + 1;
  oneShotField &= ~(mask << shift);
  oneShotField |= (chBits << shift);
  pcm->control.bits.OneShotField_h8 = oneShotField >> 8;
  pcm->control.bits.OneShotField_l8 = oneShotField;
  SendControl(pcm.get(), status);
}

void HAL_SetCTREPCMOneShotDuration(HAL_CTREPCMHandle handle, int32_t index,
                                   int32_t durMs, int32_t* status) {
  if (index > 7 || index < 0) {
    *status = PARAMETER_OUT_OF_RANGE;
    hal::SetLastError(
        status,
        fmt::format("Only [0-7] are valid index values. Requested {}", index));
    return;
  }

  auto pcm = pcmHandles->Get(handle);
  if (pcm == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return;
  }

  std::scoped_lock lock{pcm->lock};
  pcm->oneShot.sol10MsPerUnit[index] = (std::min)(
      static_cast<uint32_t>(durMs) / 10, static_cast<uint32_t>(0xFF));
  HAL_WriteCANPacketRepeating(pcm->canHandle, pcm->oneShot.sol10MsPerUnit, 8,
                              Control3, SendPeriod, status);
}

}  // extern "C"
