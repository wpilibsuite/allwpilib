/*----------------------------------------------------------------------------*/
/* Copyright (c) 2016-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "hal/Encoder.h"

#include "EncoderInternal.h"
#include "FPGAEncoder.h"
#include "HALInitializer.h"
#include "PortsInternal.h"
#include "hal/ChipObject.h"
#include "hal/Counter.h"
#include "hal/Errors.h"
#include "hal/handles/LimitedClassedHandleResource.h"

using namespace hal;

Encoder::Encoder(HAL_Handle digitalSourceHandleA,
                 HAL_AnalogTriggerType analogTriggerTypeA,
                 HAL_Handle digitalSourceHandleB,
                 HAL_AnalogTriggerType analogTriggerTypeB,
                 bool reverseDirection, HAL_EncoderEncodingType encodingType,
                 int32_t* status) {
  m_encodingType = encodingType;
  switch (encodingType) {
    case HAL_Encoder_k4X: {
      m_encodingScale = 4;
      m_encoder = HAL_InitializeFPGAEncoder(
          digitalSourceHandleA, analogTriggerTypeA, digitalSourceHandleB,
          analogTriggerTypeB, reverseDirection, &m_index, status);
      if (*status != 0) {
        return;
      }
      m_counter = HAL_kInvalidHandle;
      SetMaxPeriod(.5, status);
      break;
    }
    case HAL_Encoder_k1X:
    case HAL_Encoder_k2X: {
      SetupCounter(digitalSourceHandleA, analogTriggerTypeA,
                   digitalSourceHandleB, analogTriggerTypeB, reverseDirection,
                   encodingType, status);

      m_encodingScale = encodingType == HAL_Encoder_k1X ? 1 : 2;
      break;
    }
    default:
      *status = PARAMETER_OUT_OF_RANGE;
      return;
  }
}

void Encoder::SetupCounter(HAL_Handle digitalSourceHandleA,
                           HAL_AnalogTriggerType analogTriggerTypeA,
                           HAL_Handle digitalSourceHandleB,
                           HAL_AnalogTriggerType analogTriggerTypeB,
                           bool reverseDirection,
                           HAL_EncoderEncodingType encodingType,
                           int32_t* status) {
  m_encodingScale = encodingType == HAL_Encoder_k1X ? 1 : 2;
  m_counter =
      HAL_InitializeCounter(HAL_Counter_kExternalDirection, &m_index, status);
  if (*status != 0) return;
  HAL_SetCounterMaxPeriod(m_counter, 0.5, status);
  if (*status != 0) return;
  HAL_SetCounterUpSource(m_counter, digitalSourceHandleA, analogTriggerTypeA,
                         status);
  if (*status != 0) return;
  HAL_SetCounterDownSource(m_counter, digitalSourceHandleB, analogTriggerTypeB,
                           status);
  if (*status != 0) return;
  if (encodingType == HAL_Encoder_k1X) {
    HAL_SetCounterUpSourceEdge(m_counter, true, false, status);
    HAL_SetCounterAverageSize(m_counter, 1, status);
  } else {
    HAL_SetCounterUpSourceEdge(m_counter, true, true, status);
    HAL_SetCounterAverageSize(m_counter, 2, status);
  }
  HAL_SetCounterDownSourceEdge(m_counter, reverseDirection, true, status);
}

Encoder::~Encoder() {
  if (m_counter != HAL_kInvalidHandle) {
    int32_t status = 0;
    HAL_FreeCounter(m_counter, &status);
  } else {
    int32_t status = 0;
    HAL_FreeFPGAEncoder(m_encoder, &status);
  }
}

// CounterBase interface
int32_t Encoder::Get(int32_t* status) const {
  return static_cast<int32_t>(GetRaw(status) * DecodingScaleFactor());
}

int32_t Encoder::GetRaw(int32_t* status) const {
  if (m_counter) {
    return HAL_GetCounter(m_counter, status);
  } else {
    return HAL_GetFPGAEncoder(m_encoder, status);
  }
}

int32_t Encoder::GetEncodingScale(int32_t* status) const {
  return m_encodingScale;
}

void Encoder::Reset(int32_t* status) {
  if (m_counter) {
    HAL_ResetCounter(m_counter, status);
  } else {
    HAL_ResetFPGAEncoder(m_encoder, status);
  }
}

double Encoder::GetPeriod(int32_t* status) const {
  if (m_counter) {
    return HAL_GetCounterPeriod(m_counter, status) / DecodingScaleFactor();
  } else {
    return HAL_GetFPGAEncoderPeriod(m_encoder, status);
  }
}

void Encoder::SetMaxPeriod(double maxPeriod, int32_t* status) {
  if (m_counter) {
    HAL_SetCounterMaxPeriod(m_counter, maxPeriod, status);
  } else {
    HAL_SetFPGAEncoderMaxPeriod(m_encoder, maxPeriod, status);
  }
}

bool Encoder::GetStopped(int32_t* status) const {
  if (m_counter) {
    return HAL_GetCounterStopped(m_counter, status);
  } else {
    return HAL_GetFPGAEncoderStopped(m_encoder, status);
  }
}

bool Encoder::GetDirection(int32_t* status) const {
  if (m_counter) {
    return HAL_GetCounterDirection(m_counter, status);
  } else {
    return HAL_GetFPGAEncoderDirection(m_encoder, status);
  }
}

double Encoder::GetDistance(int32_t* status) const {
  return GetRaw(status) * DecodingScaleFactor() * m_distancePerPulse;
}

double Encoder::GetRate(int32_t* status) const {
  return m_distancePerPulse / GetPeriod(status);
}

void Encoder::SetMinRate(double minRate, int32_t* status) {
  SetMaxPeriod(m_distancePerPulse / minRate, status);
}

void Encoder::SetDistancePerPulse(double distancePerPulse, int32_t* status) {
  m_distancePerPulse = distancePerPulse;
}

void Encoder::SetReverseDirection(bool reverseDirection, int32_t* status) {
  if (m_counter) {
    HAL_SetCounterReverseDirection(m_counter, reverseDirection, status);
  } else {
    HAL_SetFPGAEncoderReverseDirection(m_encoder, reverseDirection, status);
  }
}

void Encoder::SetSamplesToAverage(int32_t samplesToAverage, int32_t* status) {
  if (samplesToAverage < 1 || samplesToAverage > 127) {
    *status = PARAMETER_OUT_OF_RANGE;
    return;
  }
  if (m_counter) {
    HAL_SetCounterSamplesToAverage(m_counter, samplesToAverage, status);
  } else {
    HAL_SetFPGAEncoderSamplesToAverage(m_encoder, samplesToAverage, status);
  }
}

int32_t Encoder::GetSamplesToAverage(int32_t* status) const {
  if (m_counter) {
    return HAL_GetCounterSamplesToAverage(m_counter, status);
  } else {
    return HAL_GetFPGAEncoderSamplesToAverage(m_encoder, status);
  }
}

void Encoder::SetIndexSource(HAL_Handle digitalSourceHandle,
                             HAL_AnalogTriggerType analogTriggerType,
                             HAL_EncoderIndexingType type, int32_t* status) {
  if (m_counter) {
    *status = HAL_COUNTER_NOT_SUPPORTED;
    return;
  }
  bool activeHigh =
      (type == HAL_kResetWhileHigh) || (type == HAL_kResetOnRisingEdge);
  bool edgeSensitive =
      (type == HAL_kResetOnFallingEdge) || (type == HAL_kResetOnRisingEdge);
  HAL_SetFPGAEncoderIndexSource(m_encoder, digitalSourceHandle,
                                analogTriggerType, activeHigh, edgeSensitive,
                                status);
}

double Encoder::DecodingScaleFactor() const {
  switch (m_encodingType) {
    case HAL_Encoder_k1X:
      return 1.0;
    case HAL_Encoder_k2X:
      return 0.5;
    case HAL_Encoder_k4X:
      return 0.25;
    default:
      return 0.0;
  }
}

static LimitedClassedHandleResource<HAL_EncoderHandle, Encoder,
                                    kNumEncoders + kNumCounters,
                                    HAL_HandleEnum::Encoder>* encoderHandles;

namespace hal {
namespace init {
void InitializeEncoder() {
  static LimitedClassedHandleResource<HAL_EncoderHandle, Encoder,
                                      kNumEncoders + kNumCounters,
                                      HAL_HandleEnum::Encoder>
      eH;
  encoderHandles = &eH;
}
}  // namespace init
}  // namespace hal

extern "C" {
HAL_EncoderHandle HAL_InitializeEncoder(
    HAL_Handle digitalSourceHandleA, HAL_AnalogTriggerType analogTriggerTypeA,
    HAL_Handle digitalSourceHandleB, HAL_AnalogTriggerType analogTriggerTypeB,
    HAL_Bool reverseDirection, HAL_EncoderEncodingType encodingType,
    int32_t* status) {
  hal::init::CheckInit();
  auto encoder = std::make_shared<Encoder>(
      digitalSourceHandleA, analogTriggerTypeA, digitalSourceHandleB,
      analogTriggerTypeB, reverseDirection, encodingType, status);
  if (*status != 0) return HAL_kInvalidHandle;  // return in creation error
  auto handle = encoderHandles->Allocate(encoder);
  if (handle == HAL_kInvalidHandle) {
    *status = NO_AVAILABLE_RESOURCES;
    return HAL_kInvalidHandle;
  }
  return handle;
}

void HAL_FreeEncoder(HAL_EncoderHandle encoderHandle, int32_t* status) {
  encoderHandles->Free(encoderHandle);
}

int32_t HAL_GetEncoder(HAL_EncoderHandle encoderHandle, int32_t* status) {
  auto encoder = encoderHandles->Get(encoderHandle);
  if (encoder == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return 0;
  }
  return encoder->Get(status);
}

int32_t HAL_GetEncoderRaw(HAL_EncoderHandle encoderHandle, int32_t* status) {
  auto encoder = encoderHandles->Get(encoderHandle);
  if (encoder == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return 0;
  }
  return encoder->GetRaw(status);
}

int32_t HAL_GetEncoderEncodingScale(HAL_EncoderHandle encoderHandle,
                                    int32_t* status) {
  auto encoder = encoderHandles->Get(encoderHandle);
  if (encoder == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return 0;
  }
  return encoder->GetEncodingScale(status);
}

void HAL_ResetEncoder(HAL_EncoderHandle encoderHandle, int32_t* status) {
  auto encoder = encoderHandles->Get(encoderHandle);
  if (encoder == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return;
  }
  encoder->Reset(status);
}

double HAL_GetEncoderPeriod(HAL_EncoderHandle encoderHandle, int32_t* status) {
  auto encoder = encoderHandles->Get(encoderHandle);
  if (encoder == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return 0;
  }
  return encoder->GetPeriod(status);
}

void HAL_SetEncoderMaxPeriod(HAL_EncoderHandle encoderHandle, double maxPeriod,
                             int32_t* status) {
  auto encoder = encoderHandles->Get(encoderHandle);
  if (encoder == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return;
  }
  encoder->SetMaxPeriod(maxPeriod, status);
}

HAL_Bool HAL_GetEncoderStopped(HAL_EncoderHandle encoderHandle,
                               int32_t* status) {
  auto encoder = encoderHandles->Get(encoderHandle);
  if (encoder == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return 0;
  }
  return encoder->GetStopped(status);
}

HAL_Bool HAL_GetEncoderDirection(HAL_EncoderHandle encoderHandle,
                                 int32_t* status) {
  auto encoder = encoderHandles->Get(encoderHandle);
  if (encoder == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return 0;
  }
  return encoder->GetDirection(status);
}

double HAL_GetEncoderDistance(HAL_EncoderHandle encoderHandle,
                              int32_t* status) {
  auto encoder = encoderHandles->Get(encoderHandle);
  if (encoder == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return 0;
  }
  return encoder->GetDistance(status);
}

double HAL_GetEncoderRate(HAL_EncoderHandle encoderHandle, int32_t* status) {
  auto encoder = encoderHandles->Get(encoderHandle);
  if (encoder == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return 0;
  }
  return encoder->GetRate(status);
}

void HAL_SetEncoderMinRate(HAL_EncoderHandle encoderHandle, double minRate,
                           int32_t* status) {
  auto encoder = encoderHandles->Get(encoderHandle);
  if (encoder == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return;
  }
  encoder->SetMinRate(minRate, status);
}

void HAL_SetEncoderDistancePerPulse(HAL_EncoderHandle encoderHandle,
                                    double distancePerPulse, int32_t* status) {
  auto encoder = encoderHandles->Get(encoderHandle);
  if (encoder == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return;
  }
  encoder->SetDistancePerPulse(distancePerPulse, status);
}

void HAL_SetEncoderReverseDirection(HAL_EncoderHandle encoderHandle,
                                    HAL_Bool reverseDirection,
                                    int32_t* status) {
  auto encoder = encoderHandles->Get(encoderHandle);
  if (encoder == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return;
  }
  encoder->SetReverseDirection(reverseDirection, status);
}

void HAL_SetEncoderSamplesToAverage(HAL_EncoderHandle encoderHandle,
                                    int32_t samplesToAverage, int32_t* status) {
  auto encoder = encoderHandles->Get(encoderHandle);
  if (encoder == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return;
  }
  encoder->SetSamplesToAverage(samplesToAverage, status);
}

int32_t HAL_GetEncoderSamplesToAverage(HAL_EncoderHandle encoderHandle,
                                       int32_t* status) {
  auto encoder = encoderHandles->Get(encoderHandle);
  if (encoder == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return 0;
  }
  return encoder->GetSamplesToAverage(status);
}

double HAL_GetEncoderDecodingScaleFactor(HAL_EncoderHandle encoderHandle,
                                         int32_t* status) {
  auto encoder = encoderHandles->Get(encoderHandle);
  if (encoder == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return 0;
  }
  return encoder->DecodingScaleFactor();
}

double HAL_GetEncoderDistancePerPulse(HAL_EncoderHandle encoderHandle,
                                      int32_t* status) {
  auto encoder = encoderHandles->Get(encoderHandle);
  if (encoder == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return 0;
  }
  return encoder->GetDistancePerPulse();
}

HAL_EncoderEncodingType HAL_GetEncoderEncodingType(
    HAL_EncoderHandle encoderHandle, int32_t* status) {
  auto encoder = encoderHandles->Get(encoderHandle);
  if (encoder == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return HAL_Encoder_k4X;  // default to k4X
  }
  return encoder->GetEncodingType();
}

void HAL_SetEncoderIndexSource(HAL_EncoderHandle encoderHandle,
                               HAL_Handle digitalSourceHandle,
                               HAL_AnalogTriggerType analogTriggerType,
                               HAL_EncoderIndexingType type, int32_t* status) {
  auto encoder = encoderHandles->Get(encoderHandle);
  if (encoder == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return;
  }
  encoder->SetIndexSource(digitalSourceHandle, analogTriggerType, type, status);
}

int32_t HAL_GetEncoderFPGAIndex(HAL_EncoderHandle encoderHandle,
                                int32_t* status) {
  auto encoder = encoderHandles->Get(encoderHandle);
  if (encoder == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return 0;
  }
  return encoder->GetFPGAIndex();
}

}  // extern "C"
