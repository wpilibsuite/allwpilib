/*----------------------------------------------------------------------------*/
/* Copyright (c) 2016-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "FPGAEncoder.h"

#include <memory>

#include "DigitalInternal.h"
#include "HALInitializer.h"
#include "PortsInternal.h"
#include "hal/handles/LimitedHandleResource.h"

using namespace hal;

namespace {

struct Encoder {
  std::unique_ptr<tEncoder> encoder;
  uint8_t index;
};

}  // namespace

static constexpr double DECODING_SCALING_FACTOR = 0.25;

static LimitedHandleResource<HAL_FPGAEncoderHandle, Encoder, kNumEncoders,
                             HAL_HandleEnum::FPGAEncoder>* fpgaEncoderHandles;

namespace hal {
namespace init {
void InitializeFPGAEncoder() {
  static LimitedHandleResource<HAL_FPGAEncoderHandle, Encoder, kNumEncoders,
                               HAL_HandleEnum::FPGAEncoder>
      feH;
  fpgaEncoderHandles = &feH;
}
}  // namespace init
}  // namespace hal

extern "C" {

HAL_FPGAEncoderHandle HAL_InitializeFPGAEncoder(
    HAL_Handle digitalSourceHandleA, HAL_AnalogTriggerType analogTriggerTypeA,
    HAL_Handle digitalSourceHandleB, HAL_AnalogTriggerType analogTriggerTypeB,
    HAL_Bool reverseDirection, int32_t* index, int32_t* status) {
  hal::init::CheckInit();
  bool routingAnalogTriggerA = false;
  uint8_t routingChannelA = 0;
  uint8_t routingModuleA = 0;
  bool successA = remapDigitalSource(digitalSourceHandleA, analogTriggerTypeA,
                                     routingChannelA, routingModuleA,
                                     routingAnalogTriggerA);
  bool routingAnalogTriggerB = false;
  uint8_t routingChannelB = 0;
  uint8_t routingModuleB = 0;
  bool successB = remapDigitalSource(digitalSourceHandleB, analogTriggerTypeB,
                                     routingChannelB, routingModuleB,
                                     routingAnalogTriggerB);

  if (!successA || !successB) {
    *status = HAL_HANDLE_ERROR;
    return HAL_kInvalidHandle;
  }

  auto handle = fpgaEncoderHandles->Allocate();
  if (handle == HAL_kInvalidHandle) {  // out of resources
    *status = NO_AVAILABLE_RESOURCES;
    return HAL_kInvalidHandle;
  }

  auto encoder = fpgaEncoderHandles->Get(handle);
  if (encoder == nullptr) {  // will only error on thread issue
    *status = HAL_HANDLE_ERROR;
    return HAL_kInvalidHandle;
  }

  encoder->index = static_cast<uint8_t>(getHandleIndex(handle));
  *index = encoder->index;
  // TODO: if (index == ~0ul) { CloneError(quadEncoders); return; }
  encoder->encoder.reset(tEncoder::create(encoder->index, status));
  encoder->encoder->writeConfig_ASource_Module(routingModuleA, status);
  encoder->encoder->writeConfig_ASource_Channel(routingChannelA, status);
  encoder->encoder->writeConfig_ASource_AnalogTrigger(routingAnalogTriggerA,
                                                      status);
  encoder->encoder->writeConfig_BSource_Module(routingModuleB, status);
  encoder->encoder->writeConfig_BSource_Channel(routingChannelB, status);
  encoder->encoder->writeConfig_BSource_AnalogTrigger(routingAnalogTriggerB,
                                                      status);
  encoder->encoder->strobeReset(status);
  encoder->encoder->writeConfig_Reverse(reverseDirection, status);
  encoder->encoder->writeTimerConfig_AverageSize(4, status);

  return handle;
}

void HAL_FreeFPGAEncoder(HAL_FPGAEncoderHandle fpgaEncoderHandle,
                         int32_t* status) {
  fpgaEncoderHandles->Free(fpgaEncoderHandle);
}

void HAL_ResetFPGAEncoder(HAL_FPGAEncoderHandle fpgaEncoderHandle,
                          int32_t* status) {
  auto encoder = fpgaEncoderHandles->Get(fpgaEncoderHandle);
  if (encoder == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return;
  }
  encoder->encoder->strobeReset(status);
}

int32_t HAL_GetFPGAEncoder(HAL_FPGAEncoderHandle fpgaEncoderHandle,
                           int32_t* status) {
  auto encoder = fpgaEncoderHandles->Get(fpgaEncoderHandle);
  if (encoder == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return 0;
  }
  return encoder->encoder->readOutput_Value(status);
}

double HAL_GetFPGAEncoderPeriod(HAL_FPGAEncoderHandle fpgaEncoderHandle,
                                int32_t* status) {
  auto encoder = fpgaEncoderHandles->Get(fpgaEncoderHandle);
  if (encoder == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return 0.0;
  }
  tEncoder::tTimerOutput output = encoder->encoder->readTimerOutput(status);
  double value;
  if (output.Stalled) {
    // Return infinity
    double zero = 0.0;
    value = 1.0 / zero;
  } else {
    // output.Period is a fixed point number that counts by 2 (24 bits, 25
    // integer bits)
    value = static_cast<double>(output.Period << 1) /
            static_cast<double>(output.Count);
  }
  double measuredPeriod = value * 2.5e-8;
  return measuredPeriod / DECODING_SCALING_FACTOR;
}

void HAL_SetFPGAEncoderMaxPeriod(HAL_FPGAEncoderHandle fpgaEncoderHandle,
                                 double maxPeriod, int32_t* status) {
  auto encoder = fpgaEncoderHandles->Get(fpgaEncoderHandle);
  if (encoder == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return;
  }
  encoder->encoder->writeTimerConfig_StallPeriod(
      static_cast<uint32_t>(maxPeriod * 4.0e8 * DECODING_SCALING_FACTOR),
      status);
}

HAL_Bool HAL_GetFPGAEncoderStopped(HAL_FPGAEncoderHandle fpgaEncoderHandle,
                                   int32_t* status) {
  auto encoder = fpgaEncoderHandles->Get(fpgaEncoderHandle);
  if (encoder == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return false;
  }
  return encoder->encoder->readTimerOutput_Stalled(status) != 0;
}

HAL_Bool HAL_GetFPGAEncoderDirection(HAL_FPGAEncoderHandle fpgaEncoderHandle,
                                     int32_t* status) {
  auto encoder = fpgaEncoderHandles->Get(fpgaEncoderHandle);
  if (encoder == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return false;
  }
  return encoder->encoder->readOutput_Direction(status);
}

void HAL_SetFPGAEncoderReverseDirection(HAL_FPGAEncoderHandle fpgaEncoderHandle,
                                        HAL_Bool reverseDirection,
                                        int32_t* status) {
  auto encoder = fpgaEncoderHandles->Get(fpgaEncoderHandle);
  if (encoder == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return;
  }
  encoder->encoder->writeConfig_Reverse(reverseDirection, status);
}

void HAL_SetFPGAEncoderSamplesToAverage(HAL_FPGAEncoderHandle fpgaEncoderHandle,
                                        int32_t samplesToAverage,
                                        int32_t* status) {
  auto encoder = fpgaEncoderHandles->Get(fpgaEncoderHandle);
  if (encoder == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return;
  }
  if (samplesToAverage < 1 || samplesToAverage > 127) {
    *status = PARAMETER_OUT_OF_RANGE;
  }
  encoder->encoder->writeTimerConfig_AverageSize(samplesToAverage, status);
}

int32_t HAL_GetFPGAEncoderSamplesToAverage(
    HAL_FPGAEncoderHandle fpgaEncoderHandle, int32_t* status) {
  auto encoder = fpgaEncoderHandles->Get(fpgaEncoderHandle);
  if (encoder == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return 0;
  }
  return encoder->encoder->readTimerConfig_AverageSize(status);
}

void HAL_SetFPGAEncoderIndexSource(HAL_FPGAEncoderHandle fpgaEncoderHandle,
                                   HAL_Handle digitalSourceHandle,
                                   HAL_AnalogTriggerType analogTriggerType,
                                   HAL_Bool activeHigh, HAL_Bool edgeSensitive,
                                   int32_t* status) {
  auto encoder = fpgaEncoderHandles->Get(fpgaEncoderHandle);
  if (encoder == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return;
  }

  bool routingAnalogTrigger = false;
  uint8_t routingChannel = 0;
  uint8_t routingModule = 0;
  bool success =
      remapDigitalSource(digitalSourceHandle, analogTriggerType, routingChannel,
                         routingModule, routingAnalogTrigger);
  if (!success) {
    *status = HAL_HANDLE_ERROR;
    return;
  }

  encoder->encoder->writeConfig_IndexSource_Channel(routingChannel, status);
  encoder->encoder->writeConfig_IndexSource_Module(routingModule, status);
  encoder->encoder->writeConfig_IndexSource_AnalogTrigger(routingAnalogTrigger,
                                                          status);
  encoder->encoder->writeConfig_IndexActiveHigh(activeHigh, status);
  encoder->encoder->writeConfig_IndexEdgeSensitive(edgeSensitive, status);
}

}  // extern "C"
