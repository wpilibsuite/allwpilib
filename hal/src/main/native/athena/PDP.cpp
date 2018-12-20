/*----------------------------------------------------------------------------*/
/* Copyright (c) 2016-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "hal/PDP.h"

#include <cstring>  // std::memcpy
#include <memory>

#include <FRC_NetworkCommunication/CANSessionMux.h>
#include <wpi/mutex.h>

#include "HALInitializer.h"
#include "PortsInternal.h"
#include "hal/Errors.h"
#include "hal/Ports.h"
#include "hal/handles/UnlimitedHandleResource.h"

using namespace hal;

static constexpr uint32_t kStatus1 = 0x08041400;
static constexpr uint32_t kStatus2 = 0x08041440;
static constexpr uint32_t kStatus3 = 0x08041480;
static constexpr uint32_t kStatusEnergy = 0x08041740;

static constexpr uint32_t kControl1 = 0x08041C00;

static constexpr int32_t kTimeoutMs = 255;
static constexpr int32_t kOneMinuteMs = 360000;

static int32_t GetTimeMs() {
  std::chrono::time_point<std::chrono::steady_clock> now;
  now = std::chrono::steady_clock::now();

  auto duration = now.time_since_epoch();
  auto millis =
      std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();

  return (int64_t)millis;
}

namespace {
struct PDPStorage {
  PDPStorage(uint32_t module_) : module(module_) {
    time0to5 = GetTimeMs() - kOneMinuteMs;
    time6to11 = GetTimeMs() - kOneMinuteMs;
    time12to15 = GetTimeMs() - kOneMinuteMs;
    timeEnergy = GetTimeMs() - kOneMinuteMs;
  }

  void GetStatus0to5();
  void GetStatus6to11();
  void GetStatus12to15();
  void GetStatusEnergy();

  uint32_t module;

  wpi::mutex pdpMutex;
  uint8_t stat0to5[8];
  uint8_t stat6to11[8];
  uint8_t stat12to15[8];
  uint8_t statEnergy[8];

  // back-date the starting time so we timeout immediately if no data is
  // received at start.
  int32_t time0to5;
  int32_t time6to11;
  int32_t time12to15;
  int32_t timeEnergy;
};
}

static UnlimitedHandleResource<HAL_PDPHandle, PDPStorage, HAL_HandleEnum::PDP>*
    pdpHandles;

/* encoder/decoders */
union PdpStatus1 {
  uint8_t data[8];
  struct Bits {
    unsigned chan1_h8 : 8;
    unsigned chan2_h6 : 6;
    unsigned chan1_l2 : 2;
    unsigned chan3_h4 : 4;
    unsigned chan2_l4 : 4;
    unsigned chan4_h2 : 2;
    unsigned chan3_l6 : 6;
    unsigned chan4_l8 : 8;
    unsigned chan5_h8 : 8;
    unsigned chan6_h6 : 6;
    unsigned chan5_l2 : 2;
    unsigned reserved4 : 4;
    unsigned chan6_l4 : 4;
  } bits;
};

union PdpStatus2 {
  uint8_t data[8];
  struct Bits {
    unsigned chan7_h8 : 8;
    unsigned chan8_h6 : 6;
    unsigned chan7_l2 : 2;
    unsigned chan9_h4 : 4;
    unsigned chan8_l4 : 4;
    unsigned chan10_h2 : 2;
    unsigned chan9_l6 : 6;
    unsigned chan10_l8 : 8;
    unsigned chan11_h8 : 8;
    unsigned chan12_h6 : 6;
    unsigned chan11_l2 : 2;
    unsigned reserved4 : 4;
    unsigned chan12_l4 : 4;
  } bits;
};

union PdpStatus3 {
  uint8_t data[8];
  struct Bits {
    unsigned chan13_h8 : 8;
    unsigned chan14_h6 : 6;
    unsigned chan13_l2 : 2;
    unsigned chan15_h4 : 4;
    unsigned chan14_l4 : 4;
    unsigned chan16_h2 : 2;
    unsigned chan15_l6 : 6;
    unsigned chan16_l8 : 8;
    unsigned internalResBattery_mOhms : 8;
    unsigned busVoltage : 8;
    unsigned temp : 8;
  } bits;
};

union PdpStatusEnergy {
  uint8_t data[8];
  struct Bits {
    unsigned TmeasMs_likelywillbe20ms_ : 8;
    unsigned TotalCurrent_125mAperunit_h8 : 8;
    unsigned Power_125mWperunit_h4 : 4;
    unsigned TotalCurrent_125mAperunit_l4 : 4;
    unsigned Power_125mWperunit_m8 : 8;
    unsigned Energy_125mWPerUnitXTmeas_h4 : 4;
    unsigned Power_125mWperunit_l4 : 4;
    unsigned Energy_125mWPerUnitXTmeas_mh8 : 8;
    unsigned Energy_125mWPerUnitXTmeas_ml8 : 8;
    unsigned Energy_125mWPerUnitXTmeas_l8 : 8;
  } bits;
};

namespace hal {
namespace init {
void InitializePDP() {
  static UnlimitedHandleResource<HAL_PDPHandle, PDPStorage, HAL_HandleEnum::PDP>
      pH;
  pdpHandles = &pH;
}
}  // namespace init
}  // namespace hal

/* ----------- CAN routines  ------- */
static int32_t PDP_ReadMsg(uint32_t messageID, uint32_t* timeStamp,
                           uint8_t data[8]) {
  uint8_t dataSize = 0;
  *timeStamp = 0;
  int32_t status = 0;
  FRC_NetworkCommunication_CANSessionMux_receiveMessage(
      &messageID, 0x1fffffff, data, &dataSize, timeStamp, &status);
  return status;
}

static int32_t PDP_WriteMsgOnce(uint32_t messageID, const uint8_t data[8],
                                uint8_t dataSize) {
  int32_t status = 0;
  FRC_NetworkCommunication_CANSessionMux_sendMessage(messageID, data, dataSize,
                                                     0, &status);
  return status;
}

/* ----------- util  ------- */
static int PDP_CheckTime(int32_t lastTime) {
  int32_t now = GetTimeMs();
  int32_t delta = now - lastTime;
  if (delta < 0) {
    /* not sure what this means, ignore for now */
    return 0;
  }
  if (delta > kTimeoutMs) {
    return HAL_CAN_TIMEOUT; /* too much time */
  }
  return 0;
}

/* ----------- update routines ------- */
void PDPStorage::GetStatus0to5() {
  uint8_t temp[8];
  uint32_t timeStamp;
  int32_t err = PDP_ReadMsg(kStatus1 | module, &timeStamp, temp);

  std::lock_guard<wpi::mutex> lock(pdpMutex);
  if (err == 0) {
    std::memcpy(stat0to5, temp, 8);
    time0to5 = GetTimeMs();
  }
}

void PDPStorage::GetStatus6to11() {
  uint8_t temp[8];
  uint32_t timeStamp;
  int32_t err = PDP_ReadMsg(kStatus2 | module, &timeStamp, temp);

  std::lock_guard<wpi::mutex> lock(pdpMutex);
  if (err == 0) {
    std::memcpy(stat6to11, temp, 8);
    time6to11 = GetTimeMs();
  }
}

void PDPStorage::GetStatus12to15() {
  uint8_t temp[8];
  uint32_t timeStamp;
  int32_t err = PDP_ReadMsg(kStatus3 | module, &timeStamp, temp);

  std::lock_guard<wpi::mutex> lock(pdpMutex);
  if (err == 0) {
    std::memcpy(stat12to15, temp, 8);
    time12to15 = GetTimeMs();
  }
}

void PDPStorage::GetStatusEnergy() {
  uint8_t temp[8];
  uint32_t timeStamp;
  int32_t err = PDP_ReadMsg(kStatusEnergy | module, &timeStamp, temp);

  std::lock_guard<wpi::mutex> lock(pdpMutex);
  if (err == 0) {
    std::memcpy(statEnergy, temp, 8);
    timeEnergy = GetTimeMs();
  }
}

extern "C" {
int HAL_InitializePDP(int32_t module, int32_t* status) {
  hal::init::CheckInit();
  if (!HAL_CheckPDPModule(module)) {
    *status = PARAMETER_OUT_OF_RANGE;
    return HAL_kInvalidHandle;
  }
  auto pdp = std::make_shared<PDPStorage>(module);
  auto handle = pdpHandles->Allocate(pdp);
  if (handle == HAL_kInvalidHandle) {
    *status = NO_AVAILABLE_RESOURCES;
    return HAL_kInvalidHandle;
  }
  return handle;
}

void HAL_CleanPDP(HAL_PDPHandle handle) {
  auto data = pdpHandles->Free(handle);
  // XXX: should this do a SEND_PERIOD_STOP_REPEATING?
}

HAL_Bool HAL_CheckPDPModule(int32_t module) {
  return module < kNumPDPModules && module >= 0;
}

HAL_Bool HAL_CheckPDPChannel(int32_t channel) {
  return channel < kNumPDPChannels && channel >= 0;
}

double HAL_GetPDPTemperature(HAL_PDPHandle handle, int32_t* status) {
  auto pdp = pdpHandles->Get(handle);
  if (!pdp) {
    *status = HAL_HANDLE_ERROR;
    return 0;
  }
  pdp->GetStatus12to15();
  {
    std::lock_guard<wpi::mutex> lock(pdp->pdpMutex);
    PdpStatus3 pdpStatus;
    std::memcpy(&pdpStatus, pdp->stat12to15, 8);
    *status = PDP_CheckTime(pdp->time12to15);
    return pdpStatus.bits.temp * 1.03250836957542 - 67.8564500484966;
  }
}

double HAL_GetPDPVoltage(HAL_PDPHandle handle, int32_t* status) {
  auto pdp = pdpHandles->Get(handle);
  if (!pdp) {
    *status = HAL_HANDLE_ERROR;
    return 0;
  }
  pdp->GetStatus12to15();
  {
    std::lock_guard<wpi::mutex> lock(pdp->pdpMutex);
    PdpStatus3 pdpStatus;
    std::memcpy(&pdpStatus, pdp->stat12to15, 8);
    *status = PDP_CheckTime(pdp->time12to15);
    return pdpStatus.bits.busVoltage * 0.05 + 4.0; /* 50mV per unit plus 4V. */
  }
}

double HAL_GetPDPChannelCurrent(HAL_PDPHandle handle, int32_t channel,
                                int32_t* status) {
  if (!HAL_CheckPDPChannel(channel)) {
    *status = PARAMETER_OUT_OF_RANGE;
    return 0;
  }
  auto pdp = pdpHandles->Get(handle);
  if (!pdp) {
    *status = HAL_HANDLE_ERROR;
    return 0;
  }

  if (channel <= 5) {
    pdp->GetStatus0to5();
  } else if (channel <= 11) {
    pdp->GetStatus6to11();
  } else {
    pdp->GetStatus12to15();
  }

  double raw = 0;

  if (channel <= 5) {
    std::lock_guard<wpi::mutex> lock(pdp->pdpMutex);
    PdpStatus1 pdpStatus;
    std::memcpy(&pdpStatus, pdp->stat0to5, 8);
    *status = PDP_CheckTime(pdp->time0to5);

    switch (channel) {
      case 0:
        raw = (static_cast<uint32_t>(pdpStatus.bits.chan1_h8) << 2) |
              pdpStatus.bits.chan1_l2;
        break;
      case 1:
        raw = (static_cast<uint32_t>(pdpStatus.bits.chan2_h6) << 4) |
              pdpStatus.bits.chan2_l4;
        break;
      case 2:
        raw = (static_cast<uint32_t>(pdpStatus.bits.chan3_h4) << 6) |
              pdpStatus.bits.chan3_l6;
        break;
      case 3:
        raw = (static_cast<uint32_t>(pdpStatus.bits.chan4_h2) << 8) |
              pdpStatus.bits.chan4_l8;
        break;
      case 4:
        raw = (static_cast<uint32_t>(pdpStatus.bits.chan5_h8) << 2) |
              pdpStatus.bits.chan5_l2;
        break;
      case 5:
        raw = (static_cast<uint32_t>(pdpStatus.bits.chan6_h6) << 4) |
              pdpStatus.bits.chan6_l4;
        break;
    }
  } else if (channel <= 11) {
    std::lock_guard<wpi::mutex> lock(pdp->pdpMutex);
    PdpStatus2 pdpStatus;
    std::memcpy(&pdpStatus, pdp->stat6to11, 8);
    *status = PDP_CheckTime(pdp->time6to11);

    switch (channel) {
      case 6:
        raw = (static_cast<uint32_t>(pdpStatus.bits.chan7_h8) << 2) |
              pdpStatus.bits.chan7_l2;
        break;
      case 7:
        raw = (static_cast<uint32_t>(pdpStatus.bits.chan8_h6) << 4) |
              pdpStatus.bits.chan8_l4;
        break;
      case 8:
        raw = (static_cast<uint32_t>(pdpStatus.bits.chan9_h4) << 6) |
              pdpStatus.bits.chan9_l6;
        break;
      case 9:
        raw = (static_cast<uint32_t>(pdpStatus.bits.chan10_h2) << 8) |
              pdpStatus.bits.chan10_l8;
        break;
      case 10:
        raw = (static_cast<uint32_t>(pdpStatus.bits.chan11_h8) << 2) |
              pdpStatus.bits.chan11_l2;
        break;
      case 11:
        raw = (static_cast<uint32_t>(pdpStatus.bits.chan12_h6) << 4) |
              pdpStatus.bits.chan12_l4;
        break;
    }
  } else {
    std::lock_guard<wpi::mutex> lock(pdp->pdpMutex);
    PdpStatus3 pdpStatus;
    std::memcpy(&pdpStatus, pdp->stat12to15, 8);
    *status = PDP_CheckTime(pdp->time12to15);

    switch (channel) {
      case 12:
        raw = (static_cast<uint32_t>(pdpStatus.bits.chan13_h8) << 2) |
              pdpStatus.bits.chan13_l2;
        break;
      case 13:
        raw = (static_cast<uint32_t>(pdpStatus.bits.chan14_h6) << 4) |
              pdpStatus.bits.chan14_l4;
        break;
      case 14:
        raw = (static_cast<uint32_t>(pdpStatus.bits.chan15_h4) << 6) |
              pdpStatus.bits.chan15_l6;
        break;
      case 15:
        raw = (static_cast<uint32_t>(pdpStatus.bits.chan16_h2) << 8) |
              pdpStatus.bits.chan16_l8;
        break;
    }
  }

  /* convert to amps */
  return raw * 0.125; /* 7.3 fixed pt value in Amps */
}

double HAL_GetPDPTotalCurrent(HAL_PDPHandle handle, int32_t* status) {
  auto pdp = pdpHandles->Get(handle);
  if (!pdp) {
    *status = HAL_HANDLE_ERROR;
    return 0;
  }

  pdp->GetStatusEnergy();
  {
    std::lock_guard<wpi::mutex> lock(pdp->pdpMutex);
    PdpStatusEnergy pdpStatus;
    std::memcpy(&pdpStatus, pdp->statEnergy, 8);
    *status = PDP_CheckTime(pdp->timeEnergy);

    uint32_t raw;
    raw = pdpStatus.bits.TotalCurrent_125mAperunit_h8;
    raw <<= 4;
    raw |= pdpStatus.bits.TotalCurrent_125mAperunit_l4;

    return raw * 0.125; /* 7.3 fixed pt value in Amps */
  }
}

double HAL_GetPDPTotalPower(HAL_PDPHandle handle, int32_t* status) {
  auto pdp = pdpHandles->Get(handle);
  if (!pdp) {
    *status = HAL_HANDLE_ERROR;
    return 0;
  }

  pdp->GetStatusEnergy();

  std::lock_guard<wpi::mutex> lock(pdp->pdpMutex);
  PdpStatusEnergy pdpStatus;
  std::memcpy(&pdpStatus, pdp->statEnergy, 8);
  *status = PDP_CheckTime(pdp->timeEnergy);

  uint32_t raw;
  raw = pdpStatus.bits.Power_125mWperunit_h4;
  raw <<= 8;
  raw |= pdpStatus.bits.Power_125mWperunit_m8;
  raw <<= 4;
  raw |= pdpStatus.bits.Power_125mWperunit_l4;
  return raw * 0.125; /* 7.3 fixed pt value in Watts */
}

double HAL_GetPDPTotalEnergy(HAL_PDPHandle handle, int32_t* status) {
  auto pdp = pdpHandles->Get(handle);
  if (!pdp) {
    *status = HAL_HANDLE_ERROR;
    return 0;
  }

  pdp->GetStatusEnergy();

  std::lock_guard<wpi::mutex> lock(pdp->pdpMutex);
  PdpStatusEnergy pdpStatus;
  std::memcpy(&pdpStatus, pdp->statEnergy, 8);
  *status = PDP_CheckTime(pdp->timeEnergy);

  uint32_t raw;
  raw = pdpStatus.bits.Energy_125mWPerUnitXTmeas_h4;
  raw <<= 8;
  raw |= pdpStatus.bits.Energy_125mWPerUnitXTmeas_mh8;
  raw <<= 8;
  raw |= pdpStatus.bits.Energy_125mWPerUnitXTmeas_ml8;
  raw <<= 8;
  raw |= pdpStatus.bits.Energy_125mWPerUnitXTmeas_l8;

  double energyJoules = raw * 0.125; /* mW integrated every TmeasMs */
  energyJoules *=
      pdpStatus.bits
          .TmeasMs_likelywillbe20ms_; /* multiplied by TmeasMs = joules */
  return energyJoules;
}

void HAL_ResetPDPTotalEnergy(HAL_PDPHandle handle, int32_t* status) {
  auto pdp = pdpHandles->Get(handle);
  if (!pdp) {
    *status = HAL_HANDLE_ERROR;
    return;
  }

  uint8_t pdpControl[] = {0x40}; /* only bit set is ResetEnergy */
  PDP_WriteMsgOnce(kControl1 | pdp->module, pdpControl, 1);
}

void HAL_ClearPDPStickyFaults(HAL_PDPHandle handle, int32_t* status) {
  auto pdp = pdpHandles->Get(handle);
  if (!pdp) {
    *status = HAL_HANDLE_ERROR;
    return;
  }

  uint8_t pdpControl[] = {0x80}; /* only bit set is ClearStickyFaults */
  PDP_WriteMsgOnce(kControl1 | pdp->module, pdpControl, 1);
}

}  // extern "C"
