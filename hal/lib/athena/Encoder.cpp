/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2016. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "HAL/Encoder.h"

#include "ChipObject.h"
#include "EncoderInternal.h"
#include "FPGAEncoder.h"
#include "HAL/Counter.h"
#include "HAL/Errors.h"
#include "handles/LimitedClassedHandleResource.h"

using namespace hal;

Encoder::Encoder(uint8_t port_a_module, uint32_t port_a_pin,
                 bool port_a_analog_trigger, uint8_t port_b_module,
                 uint32_t port_b_pin, bool port_b_analog_trigger,
                 bool reverseDirection, EncoderEncodingType encodingType,
                 int32_t* status) {
  m_encodingType = encodingType;
  switch (encodingType) {
    case HAL_Encoder_k4X: {
      m_encodingScale = 4;
      m_encoder = initializeFPGAEncoder(port_a_module, port_a_pin,
                                        port_a_analog_trigger, port_b_module,
                                        port_b_pin, port_b_analog_trigger,
                                        reverseDirection, &m_index, status);
      if (*status != 0) {
        return;
      }
      m_counter = HAL_INVALID_HANDLE;
      SetMaxPeriod(.5, status);
      break;
    }
    case HAL_Encoder_k1X:
    case HAL_Encoder_k2X: {
      SetupCounter(port_a_module, port_a_pin, port_a_analog_trigger,
                   port_b_module, port_b_pin, port_b_analog_trigger,
                   reverseDirection, encodingType, status);

      m_encodingScale = encodingType == HAL_Encoder_k1X ? 1 : 2;
      break;
    }
    default:
      *status = PARAMETER_OUT_OF_RANGE;
      return;
  }
}

void Encoder::SetupCounter(uint8_t port_a_module, uint32_t port_a_pin,
                           bool port_a_analog_trigger, uint8_t port_b_module,
                           uint32_t port_b_pin, bool port_b_analog_trigger,
                           bool reverseDirection,
                           EncoderEncodingType encodingType, int32_t* status) {
  m_encodingScale = encodingType == HAL_Encoder_k1X ? 1 : 2;
  m_counter = initializeCounter(kExternalDirection, &m_index, status);
  if (*status != 0) return;
  setCounterMaxPeriod(m_counter, 0.5, status);
  if (*status != 0) return;
  setCounterUpSource(m_counter, port_a_pin, port_a_analog_trigger, status);
  if (*status != 0) return;
  setCounterDownSource(m_counter, port_b_pin, port_b_analog_trigger, status);
  if (*status != 0) return;
  if (encodingType == HAL_Encoder_k1X) {
    setCounterUpSourceEdge(m_counter, true, false, status);
    setCounterAverageSize(m_counter, 1, status);
  } else {
    setCounterUpSourceEdge(m_counter, true, true, status);
    setCounterAverageSize(m_counter, 2, status);
  }
  setCounterDownSourceEdge(m_counter, reverseDirection, true, status);
}

Encoder::~Encoder() {
  if (m_counter != HAL_INVALID_HANDLE) {
    int32_t status = 0;
    freeCounter(m_counter, &status);
  } else {
    int32_t status = 0;
    freeFPGAEncoder(m_encoder, &status);
  }
}

// CounterBase interface
int32_t Encoder::Get(int32_t* status) const {
  return (int32_t)(GetRaw(status) * DecodingScaleFactor());
}

int32_t Encoder::GetRaw(int32_t* status) const {
  if (m_counter) {
    return getCounter(m_counter, status);
  } else {
    return getFPGAEncoder(m_encoder, status);
  }
}

int32_t Encoder::GetEncodingScale(int32_t* status) const {
  return m_encodingScale;
}

void Encoder::Reset(int32_t* status) {
  if (m_counter) {
    resetCounter(m_counter, status);
  } else {
    resetFPGAEncoder(m_encoder, status);
  }
}

double Encoder::GetPeriod(int32_t* status) const {
  if (m_counter) {
    return getCounterPeriod(m_counter, status) / DecodingScaleFactor();
  } else {
    return getFPGAEncoderPeriod(m_encoder, status);
  }
}

void Encoder::SetMaxPeriod(double maxPeriod, int32_t* status) {
  if (m_counter) {
    setCounterMaxPeriod(m_counter, maxPeriod, status);
  } else {
    setFPGAEncoderMaxPeriod(m_encoder, maxPeriod, status);
  }
}

bool Encoder::GetStopped(int32_t* status) const {
  if (m_counter) {
    return getCounterStopped(m_counter, status);
  } else {
    return getFPGAEncoderStopped(m_encoder, status);
  }
}

bool Encoder::GetDirection(int32_t* status) const {
  if (m_counter) {
    return getCounterDirection(m_counter, status);
  } else {
    return getFPGAEncoderDirection(m_encoder, status);
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
    setCounterReverseDirection(m_counter, reverseDirection, status);
  } else {
    setFPGAEncoderReverseDirection(m_encoder, reverseDirection, status);
  }
}

void Encoder::SetSamplesToAverage(int samplesToAverage, int32_t* status) {
  if (samplesToAverage < 1 || samplesToAverage > 127) {
    *status = PARAMETER_OUT_OF_RANGE;
    return;
  }
  if (m_counter) {
    setCounterSamplesToAverage(m_counter, samplesToAverage, status);
  } else {
    setFPGAEncoderSamplesToAverage(m_encoder, samplesToAverage, status);
  }
}

int32_t Encoder::GetSamplesToAverage(int32_t* status) const {
  if (m_counter) {
    return getCounterSamplesToAverage(m_counter, status);
  } else {
    return getFPGAEncoderSamplesToAverage(m_encoder, status);
  }
}

void Encoder::SetIndexSource(uint32_t pin, bool analogTrigger,
                             EncoderIndexingType type, int32_t* status) {
  if (m_counter) {
    *status = HAL_COUNTER_NOT_SUPPORTED;
    return;
  }
  bool activeHigh =
      (type == HAL_kResetWhileHigh) || (type == HAL_kResetOnRisingEdge);
  bool edgeSensitive =
      (type == HAL_kResetOnFallingEdge) || (type == HAL_kResetOnRisingEdge);
  setFPGAEncoderIndexSource(m_encoder, pin, analogTrigger, activeHigh,
                            edgeSensitive, status);
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

static LimitedClassedHandleResource<
    HalEncoderHandle, Encoder, tEncoder::kNumSystems + tCounter::kNumSystems,
    HalHandleEnum::Encoder>
    encoderHandles;

extern "C" {
HalEncoderHandle initializeEncoder(
    uint8_t port_a_module, uint32_t port_a_pin, bool port_a_analog_trigger,
    uint8_t port_b_module, uint32_t port_b_pin, bool port_b_analog_trigger,
    bool reverseDirection, EncoderEncodingType encodingType, int32_t* status) {
  auto encoder = std::make_shared<Encoder>(
      port_a_module, port_a_pin, port_a_analog_trigger, port_b_module,
      port_b_pin, port_b_analog_trigger, reverseDirection, encodingType,
      status);
  if (*status != 0) return HAL_INVALID_HANDLE;  // return in creation error
  auto handle = encoderHandles.Allocate(encoder);
  if (handle == HAL_INVALID_HANDLE) {
    *status = NO_AVAILABLE_RESOURCES;
    return HAL_INVALID_HANDLE;
  }
  return handle;
}

void freeEncoder(HalEncoderHandle encoder_handle, int32_t* status) {
  encoderHandles.Free(encoder_handle);
}

int32_t getEncoder(HalEncoderHandle encoder_handle, int32_t* status) {
  auto encoder = encoderHandles.Get(encoder_handle);
  if (encoder == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return 0;
  }
  return encoder->Get(status);
}

int32_t getEncoderRaw(HalEncoderHandle encoder_handle, int32_t* status) {
  auto encoder = encoderHandles.Get(encoder_handle);
  if (encoder == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return 0;
  }
  return encoder->GetRaw(status);
}

int32_t getEncoderEncodingScale(HalEncoderHandle encoder_handle,
                                int32_t* status) {
  auto encoder = encoderHandles.Get(encoder_handle);
  if (encoder == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return 0;
  }
  return encoder->GetEncodingScale(status);
}

void resetEncoder(HalEncoderHandle encoder_handle, int32_t* status) {
  auto encoder = encoderHandles.Get(encoder_handle);
  if (encoder == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return;
  }
  encoder->Reset(status);
}

int32_t getEncoderPeriod(HalEncoderHandle encoder_handle, int32_t* status) {
  auto encoder = encoderHandles.Get(encoder_handle);
  if (encoder == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return 0;
  }
  return encoder->GetPeriod(status);
}

void setEncoderMaxPeriod(HalEncoderHandle encoder_handle, double maxPeriod,
                         int32_t* status) {
  auto encoder = encoderHandles.Get(encoder_handle);
  if (encoder == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return;
  }
  encoder->SetMaxPeriod(maxPeriod, status);
}

uint8_t getEncoderStopped(HalEncoderHandle encoder_handle, int32_t* status) {
  auto encoder = encoderHandles.Get(encoder_handle);
  if (encoder == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return 0;
  }
  return encoder->GetStopped(status);
}

uint8_t getEncoderDirection(HalEncoderHandle encoder_handle, int32_t* status) {
  auto encoder = encoderHandles.Get(encoder_handle);
  if (encoder == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return 0;
  }
  return encoder->GetDirection(status);
}

double getEncoderDistance(HalEncoderHandle encoder_handle, int32_t* status) {
  auto encoder = encoderHandles.Get(encoder_handle);
  if (encoder == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return 0;
  }
  return encoder->GetDistance(status);
}

double getEncoderRate(HalEncoderHandle encoder_handle, int32_t* status) {
  auto encoder = encoderHandles.Get(encoder_handle);
  if (encoder == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return 0;
  }
  return encoder->GetRate(status);
}

void setEncoderMinRate(HalEncoderHandle encoder_handle, double minRate,
                       int32_t* status) {
  auto encoder = encoderHandles.Get(encoder_handle);
  if (encoder == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return;
  }
  encoder->SetMinRate(minRate, status);
}

void setEncoderDistancePerPulse(HalEncoderHandle encoder_handle,
                                double distancePerPulse, int32_t* status) {
  auto encoder = encoderHandles.Get(encoder_handle);
  if (encoder == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return;
  }
  encoder->SetDistancePerPulse(distancePerPulse, status);
}

void setEncoderReverseDirection(HalEncoderHandle encoder_handle,
                                uint8_t reverseDirection, int32_t* status) {
  auto encoder = encoderHandles.Get(encoder_handle);
  if (encoder == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return;
  }
  encoder->SetReverseDirection(reverseDirection, status);
}

void setEncoderSamplesToAverage(HalEncoderHandle encoder_handle,
                                int32_t samplesToAverage, int32_t* status) {
  auto encoder = encoderHandles.Get(encoder_handle);
  if (encoder == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return;
  }
  encoder->SetSamplesToAverage(samplesToAverage, status);
}

int32_t getEncoderSamplesToAverage(HalEncoderHandle encoder_handle,
                                   int32_t* status) {
  auto encoder = encoderHandles.Get(encoder_handle);
  if (encoder == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return 0;
  }
  return encoder->GetSamplesToAverage(status);
}

double getEncoderDecodingScaleFactor(HalEncoderHandle encoder_handle,
                                     int32_t* status) {
  auto encoder = encoderHandles.Get(encoder_handle);
  if (encoder == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return 0;
  }
  return encoder->DecodingScaleFactor();
}

double getEncoderDistancePerPulse(HalEncoderHandle encoder_handle,
                                  int32_t* status) {
  auto encoder = encoderHandles.Get(encoder_handle);
  if (encoder == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return 0;
  }
  return encoder->GetDistancePerPulse();
}

EncoderEncodingType getEncoderEncodingType(HalEncoderHandle encoder_handle,
                                           int32_t* status) {
  auto encoder = encoderHandles.Get(encoder_handle);
  if (encoder == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return HAL_Encoder_k4X;  // default to k4X
  }
  return encoder->GetEncodingType();
}

void setEncoderIndexSource(HalEncoderHandle encoder_handle, uint32_t pin,
                           uint8_t analogTrigger, EncoderIndexingType type,
                           int32_t* status) {
  auto encoder = encoderHandles.Get(encoder_handle);
  if (encoder == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return;
  }
  encoder->SetIndexSource(pin, analogTrigger, type, status);
}

int32_t getEncoderFPGAIndex(HalEncoderHandle encoder_handle, int32_t* status) {
  auto encoder = encoderHandles.Get(encoder_handle);
  if (encoder == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return 0;
  }
  return encoder->GetFPGAIndex();
}
}
