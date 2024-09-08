// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <algorithm>
#include <cstring>

#include <wpi/jni_util.h>

#include "HALUtil.h"
#include "edu_wpi_first_hal_DMAJNI.h"
#include "hal/DMA.h"
#include "hal/handles/HandlesInternal.h"

using namespace hal;
using namespace wpi::java;

namespace hal {
bool GetEncoderBaseHandle(HAL_EncoderHandle handle,
                          HAL_FPGAEncoderHandle* fpgaEncoderHandle,
                          HAL_CounterHandle* counterHandle);
}  // namespace hal

extern "C" {
/*
 * Class:     edu_wpi_first_hal_DMAJNI
 * Method:    initialize
 * Signature: ()I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_DMAJNI_initialize
  (JNIEnv* env, jclass)
{
  int32_t status = 0;
  auto handle = HAL_InitializeDMA(&status);
  CheckStatusForceThrow(env, status);
  return handle;
}

/*
 * Class:     edu_wpi_first_hal_DMAJNI
 * Method:    free
 * Signature: (I)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_DMAJNI_free
  (JNIEnv* env, jclass, jint handle)
{
  if (handle != HAL_kInvalidHandle) {
    HAL_FreeDMA(handle);
  }
}

/*
 * Class:     edu_wpi_first_hal_DMAJNI
 * Method:    setPause
 * Signature: (IZ)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_DMAJNI_setPause
  (JNIEnv* env, jclass, jint handle, jboolean pause)
{
  int32_t status = 0;
  HAL_SetDMAPause(handle, pause, &status);
  CheckStatus(env, status);
}

/*
 * Class:     edu_wpi_first_hal_DMAJNI
 * Method:    setTimedTrigger
 * Signature: (ID)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_DMAJNI_setTimedTrigger
  (JNIEnv* env, jclass, jint handle, jdouble seconds)
{
  int32_t status = 0;
  HAL_SetDMATimedTrigger(handle, seconds, &status);
  CheckStatus(env, status);
}

/*
 * Class:     edu_wpi_first_hal_DMAJNI
 * Method:    setTimedTriggerCycles
 * Signature: (II)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_DMAJNI_setTimedTriggerCycles
  (JNIEnv* env, jclass, jint handle, jint cycles)
{
  int32_t status = 0;
  HAL_SetDMATimedTriggerCycles(handle, static_cast<uint32_t>(cycles), &status);
  CheckStatus(env, status);
}

/*
 * Class:     edu_wpi_first_hal_DMAJNI
 * Method:    addEncoder
 * Signature: (II)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_DMAJNI_addEncoder
  (JNIEnv* env, jclass, jint handle, jint encoderHandle)
{
  int32_t status = 0;
  HAL_AddDMAEncoder(handle, encoderHandle, &status);
  CheckStatus(env, status);
}

/*
 * Class:     edu_wpi_first_hal_DMAJNI
 * Method:    addEncoderPeriod
 * Signature: (II)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_DMAJNI_addEncoderPeriod
  (JNIEnv* env, jclass, jint handle, jint encoderHandle)
{
  int32_t status = 0;
  HAL_AddDMAEncoderPeriod(handle, encoderHandle, &status);
  CheckStatus(env, status);
}

/*
 * Class:     edu_wpi_first_hal_DMAJNI
 * Method:    addCounter
 * Signature: (II)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_DMAJNI_addCounter
  (JNIEnv* env, jclass, jint handle, jint counterHandle)
{
  int32_t status = 0;
  HAL_AddDMACounter(handle, counterHandle, &status);
  CheckStatus(env, status);
}

/*
 * Class:     edu_wpi_first_hal_DMAJNI
 * Method:    addCounterPeriod
 * Signature: (II)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_DMAJNI_addCounterPeriod
  (JNIEnv* env, jclass, jint handle, jint counterHandle)
{
  int32_t status = 0;
  HAL_AddDMACounterPeriod(handle, counterHandle, &status);
  CheckStatus(env, status);
}

/*
 * Class:     edu_wpi_first_hal_DMAJNI
 * Method:    addDutyCycle
 * Signature: (II)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_DMAJNI_addDutyCycle
  (JNIEnv* env, jclass, jint handle, jint dutyCycleHandle)
{
  int32_t status = 0;
  HAL_AddDMADutyCycle(handle, dutyCycleHandle, &status);
  CheckStatus(env, status);
}

/*
 * Class:     edu_wpi_first_hal_DMAJNI
 * Method:    addDigitalSource
 * Signature: (II)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_DMAJNI_addDigitalSource
  (JNIEnv* env, jclass, jint handle, jint digitalSourceHandle)
{
  int32_t status = 0;
  HAL_AddDMADigitalSource(handle, digitalSourceHandle, &status);
  CheckStatus(env, status);
}

/*
 * Class:     edu_wpi_first_hal_DMAJNI
 * Method:    addAnalogInput
 * Signature: (II)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_DMAJNI_addAnalogInput
  (JNIEnv* env, jclass, jint handle, jint analogInputHandle)
{
  int32_t status = 0;
  HAL_AddDMAAnalogInput(handle, analogInputHandle, &status);
  CheckStatus(env, status);
}

/*
 * Class:     edu_wpi_first_hal_DMAJNI
 * Method:    addAveragedAnalogInput
 * Signature: (II)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_DMAJNI_addAveragedAnalogInput
  (JNIEnv* env, jclass, jint handle, jint analogInputHandle)
{
  int32_t status = 0;
  HAL_AddDMAAveragedAnalogInput(handle, analogInputHandle, &status);
  CheckStatus(env, status);
}

/*
 * Class:     edu_wpi_first_hal_DMAJNI
 * Method:    addAnalogAccumulator
 * Signature: (II)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_DMAJNI_addAnalogAccumulator
  (JNIEnv* env, jclass, jint handle, jint analogInputHandle)
{
  int32_t status = 0;
  HAL_AddDMAAnalogAccumulator(handle, analogInputHandle, &status);
  CheckStatus(env, status);
}

/*
 * Class:     edu_wpi_first_hal_DMAJNI
 * Method:    setExternalTrigger
 * Signature: (IIIZZ)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_DMAJNI_setExternalTrigger
  (JNIEnv* env, jclass, jint handle, jint digitalSourceHandle,
   jint analogTriggerType, jboolean rising, jboolean falling)
{
  int32_t status = 0;
  int32_t idx = HAL_SetDMAExternalTrigger(
      handle, digitalSourceHandle,
      static_cast<HAL_AnalogTriggerType>(analogTriggerType), rising, falling,
      &status);
  CheckStatus(env, status);
  return idx;
}

/*
 * Class:     edu_wpi_first_hal_DMAJNI
 * Method:    clearSensors
 * Signature: (I)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_DMAJNI_clearSensors
  (JNIEnv* env, jclass, jint handle)
{
  int32_t status = 0;
  HAL_ClearDMASensors(handle, &status);
  CheckStatus(env, status);
}

/*
 * Class:     edu_wpi_first_hal_DMAJNI
 * Method:    clearExternalTriggers
 * Signature: (I)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_DMAJNI_clearExternalTriggers
  (JNIEnv* env, jclass, jint handle)
{
  int32_t status = 0;
  HAL_ClearDMAExternalTriggers(handle, &status);
  CheckStatus(env, status);
}

/*
 * Class:     edu_wpi_first_hal_DMAJNI
 * Method:    startDMA
 * Signature: (II)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_DMAJNI_startDMA
  (JNIEnv* env, jclass, jint handle, jint queueDepth)
{
  int32_t status = 0;
  HAL_StartDMA(handle, queueDepth, &status);
  CheckStatus(env, status);
}

/*
 * Class:     edu_wpi_first_hal_DMAJNI
 * Method:    stopDMA
 * Signature: (I)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_DMAJNI_stopDMA
  (JNIEnv* env, jclass, jint handle)
{
  int32_t status = 0;
  HAL_StopDMA(handle, &status);
  CheckStatus(env, status);
}

/*
 * Class:     edu_wpi_first_hal_DMAJNI
 * Method:    readDMA
 * Signature: (ID[I[I)J
 */
JNIEXPORT jlong JNICALL
Java_edu_wpi_first_hal_DMAJNI_readDMA
  (JNIEnv* env, jclass, jint handle, jdouble timeoutSeconds, jintArray buf,
   jintArray store)
{
  int32_t status = 0;
  HAL_DMASample dmaSample;
  std::memset(&dmaSample, 0, sizeof(dmaSample));
  int32_t remaining = 0;
  HAL_DMAReadStatus readStatus =
      HAL_ReadDMA(handle, &dmaSample, timeoutSeconds, &remaining, &status);
  CheckStatus(env, status);

  static_assert(sizeof(uint32_t) == sizeof(jint), "Java ints must be 32 bits");

  env->SetIntArrayRegion(buf, 0, dmaSample.captureSize,
                         reinterpret_cast<jint*>(dmaSample.readBuffer));

  CriticalJSpan<jint> nativeArr{env, store};

  std::copy_n(
      dmaSample.channelOffsets,
      sizeof(dmaSample.channelOffsets) / sizeof(dmaSample.channelOffsets[0]),
      nativeArr.data());
  nativeArr[22] = static_cast<int32_t>(dmaSample.captureSize);
  nativeArr[23] = static_cast<int32_t>(dmaSample.triggerChannels);
  nativeArr[24] = remaining;
  nativeArr[25] = readStatus;

  return dmaSample.timeStamp;
}

// TODO sync these up
enum DMAOffsetConstants {
  kEnable_AI0_Low = 0,
  kEnable_AI0_High = 1,
  kEnable_AIAveraged0_Low = 2,
  kEnable_AIAveraged0_High = 3,
  kEnable_AI1_Low = 4,
  kEnable_AI1_High = 5,
  kEnable_AIAveraged1_Low = 6,
  kEnable_AIAveraged1_High = 7,
  kEnable_Accumulator0 = 8,
  kEnable_Accumulator1 = 9,
  kEnable_DI = 10,
  kEnable_AnalogTriggers = 11,
  kEnable_Counters_Low = 12,
  kEnable_Counters_High = 13,
  kEnable_CounterTimers_Low = 14,
  kEnable_CounterTimers_High = 15,
  kEnable_Encoders_Low = 16,
  kEnable_Encoders_High = 17,
  kEnable_EncoderTimers_Low = 18,
  kEnable_EncoderTimers_High = 19,
  kEnable_DutyCycle_Low = 20,
  kEnable_DutyCycle_High = 21,
};

/*
 * Class:     edu_wpi_first_hal_DMAJNI
 * Method:    getSensorReadData
 * Signature: (I)Ljava/lang/Object;
 */
JNIEXPORT jobject JNICALL
Java_edu_wpi_first_hal_DMAJNI_getSensorReadData
  (JNIEnv* env, jclass, jint handle)
{
  HAL_Handle halHandle = static_cast<HAL_Handle>(handle);

  // Check for encoder/counter handle
  HAL_FPGAEncoderHandle fpgaEncoderHandle = 0;
  HAL_CounterHandle counterHandle = 0;
  bool validEncoderHandle =
      hal::GetEncoderBaseHandle(halHandle, &fpgaEncoderHandle, &counterHandle);

  if (validEncoderHandle) {
    if (counterHandle != HAL_kInvalidHandle) {
      int32_t cindex = getHandleIndex(counterHandle);
      if (cindex < 4) {
        return CreateDMABaseStore(env, kEnable_Counters_Low, cindex);
      } else {
        return CreateDMABaseStore(env, kEnable_Counters_High, cindex - 4);
      }
    } else {
      int32_t cindex = getHandleIndex(fpgaEncoderHandle);
      if (cindex < 4) {
        return CreateDMABaseStore(env, kEnable_Encoders_Low, cindex);
      } else {
        return CreateDMABaseStore(env, kEnable_Encoders_High, cindex - 4);
      }
    }
  }

  HAL_HandleEnum handleType = getHandleType(halHandle);
  int32_t index = getHandleIndex(halHandle);
  if (handleType == HAL_HandleEnum::DIO) {
    return CreateDMABaseStore(env, kEnable_DI, index);
  } else if (handleType == HAL_HandleEnum::AnalogTrigger) {
    return CreateDMABaseStore(env, kEnable_AnalogTriggers, index);
  } else if (handleType == HAL_HandleEnum::AnalogInput) {
    if (index < 4) {
      return CreateDMABaseStore(env, kEnable_AI0_Low, index);
    } else {
      return CreateDMABaseStore(env, kEnable_AI0_High, index - 4);
    }
  } else if (handleType == HAL_HandleEnum::DutyCycle) {
    if (index < 4) {
      return CreateDMABaseStore(env, kEnable_DutyCycle_Low, index);
    } else {
      return CreateDMABaseStore(env, kEnable_DutyCycle_High, index - 4);
    }
  } else {
    return nullptr;
  }
}

}  // extern "C"
