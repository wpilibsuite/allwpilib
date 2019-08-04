/*----------------------------------------------------------------------------*/
/* Copyright (c) 2016-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include <jni.h>

#include <cassert>

#include "HALUtil.h"
#include "edu_wpi_first_hal_AnalogJNI.h"
#include "hal/AnalogAccumulator.h"
#include "hal/AnalogInput.h"
#include "hal/AnalogOutput.h"
#include "hal/AnalogTrigger.h"
#include "hal/Ports.h"
#include "hal/cpp/Log.h"
#include "hal/handles/HandlesInternal.h"

using namespace frc;

// set the logging level
TLogLevel analogJNILogLevel = logWARNING;

#define ANALOGJNI_LOG(level)     \
  if (level > analogJNILogLevel) \
    ;                            \
  else                           \
    Log().Get(level)

extern "C" {

/*
 * Class:     edu_wpi_first_hal_AnalogJNI
 * Method:    initializeAnalogInputPort
 * Signature: (I)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_AnalogJNI_initializeAnalogInputPort
  (JNIEnv* env, jclass, jint id)
{
  ANALOGJNI_LOG(logDEBUG) << "Port Handle = " << (HAL_PortHandle)id;
  int32_t status = 0;
  auto analog = HAL_InitializeAnalogInputPort((HAL_PortHandle)id, &status);
  ANALOGJNI_LOG(logDEBUG) << "Status = " << status;
  ANALOGJNI_LOG(logDEBUG) << "Analog Handle = " << analog;
  CheckStatusRange(env, status, 0, HAL_GetNumAnalogInputs(),
                   hal::getPortHandleChannel((HAL_PortHandle)id));
  return (jint)analog;
}

/*
 * Class:     edu_wpi_first_hal_AnalogJNI
 * Method:    freeAnalogInputPort
 * Signature: (I)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_AnalogJNI_freeAnalogInputPort
  (JNIEnv* env, jclass, jint id)
{
  ANALOGJNI_LOG(logDEBUG) << "Port Handle = " << (HAL_AnalogInputHandle)id;
  HAL_FreeAnalogInputPort((HAL_AnalogInputHandle)id);
}

/*
 * Class:     edu_wpi_first_hal_AnalogJNI
 * Method:    initializeAnalogOutputPort
 * Signature: (I)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_AnalogJNI_initializeAnalogOutputPort
  (JNIEnv* env, jclass, jint id)
{
  ANALOGJNI_LOG(logDEBUG) << "Port Handle = " << (HAL_PortHandle)id;
  int32_t status = 0;
  HAL_AnalogOutputHandle analog =
      HAL_InitializeAnalogOutputPort((HAL_PortHandle)id, &status);
  ANALOGJNI_LOG(logDEBUG) << "Status = " << status;
  ANALOGJNI_LOG(logDEBUG) << "Analog Handle = " << analog;
  CheckStatusRange(env, status, 0, HAL_GetNumAnalogOutputs(),
                   hal::getPortHandleChannel((HAL_PortHandle)id));
  return (jlong)analog;
}

/*
 * Class:     edu_wpi_first_hal_AnalogJNI
 * Method:    freeAnalogOutputPort
 * Signature: (I)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_AnalogJNI_freeAnalogOutputPort
  (JNIEnv* env, jclass, jint id)
{
  ANALOGJNI_LOG(logDEBUG) << "Port Handle = " << id;
  HAL_FreeAnalogOutputPort((HAL_AnalogOutputHandle)id);
}

/*
 * Class:     edu_wpi_first_hal_AnalogJNI
 * Method:    checkAnalogModule
 * Signature: (B)Z
 */
JNIEXPORT jboolean JNICALL
Java_edu_wpi_first_hal_AnalogJNI_checkAnalogModule
  (JNIEnv*, jclass, jbyte value)
{
  // ANALOGJNI_LOG(logDEBUG) << "Module = " << (jint)value;
  jboolean returnValue = HAL_CheckAnalogModule(value);
  // ANALOGJNI_LOG(logDEBUG) << "checkAnalogModuleResult = " <<
  // (jint)returnValue;
  return returnValue;
}

/*
 * Class:     edu_wpi_first_hal_AnalogJNI
 * Method:    checkAnalogInputChannel
 * Signature: (I)Z
 */
JNIEXPORT jboolean JNICALL
Java_edu_wpi_first_hal_AnalogJNI_checkAnalogInputChannel
  (JNIEnv*, jclass, jint value)
{
  // ANALOGJNI_LOG(logDEBUG) << "Channel = " << value;
  jboolean returnValue = HAL_CheckAnalogInputChannel(value);
  // ANALOGJNI_LOG(logDEBUG) << "checkAnalogChannelResult = " <<
  // (jint)returnValue;
  return returnValue;
}

/*
 * Class:     edu_wpi_first_hal_AnalogJNI
 * Method:    checkAnalogOutputChannel
 * Signature: (I)Z
 */
JNIEXPORT jboolean JNICALL
Java_edu_wpi_first_hal_AnalogJNI_checkAnalogOutputChannel
  (JNIEnv*, jclass, jint value)
{
  // ANALOGJNI_LOG(logDEBUG) << "Channel = " << value;
  jboolean returnValue = HAL_CheckAnalogOutputChannel(value);
  // ANALOGJNI_LOG(logDEBUG) << "checkAnalogChannelResult = " <<
  // (jint)returnValue;
  return returnValue;
}

/*
 * Class:     edu_wpi_first_hal_AnalogJNI
 * Method:    setAnalogOutput
 * Signature: (ID)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_AnalogJNI_setAnalogOutput
  (JNIEnv* env, jclass, jint id, jdouble voltage)
{
  ANALOGJNI_LOG(logDEBUG) << "Calling setAnalogOutput";
  ANALOGJNI_LOG(logDEBUG) << "Voltage = " << voltage;
  ANALOGJNI_LOG(logDEBUG) << "Analog Handle = " << id;
  int32_t status = 0;
  HAL_SetAnalogOutput((HAL_AnalogOutputHandle)id, voltage, &status);
  CheckStatus(env, status);
}

/*
 * Class:     edu_wpi_first_hal_AnalogJNI
 * Method:    getAnalogOutput
 * Signature: (I)D
 */
JNIEXPORT jdouble JNICALL
Java_edu_wpi_first_hal_AnalogJNI_getAnalogOutput
  (JNIEnv* env, jclass, jint id)
{
  int32_t status = 0;
  double val = HAL_GetAnalogOutput((HAL_AnalogOutputHandle)id, &status);
  CheckStatus(env, status);
  return val;
}

/*
 * Class:     edu_wpi_first_hal_AnalogJNI
 * Method:    setAnalogSampleRate
 * Signature: (D)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_AnalogJNI_setAnalogSampleRate
  (JNIEnv* env, jclass, jdouble value)
{
  ANALOGJNI_LOG(logDEBUG) << "SampleRate = " << value;
  int32_t status = 0;
  HAL_SetAnalogSampleRate(value, &status);
  ANALOGJNI_LOG(logDEBUG) << "Status = " << status;
  CheckStatus(env, status);
}

/*
 * Class:     edu_wpi_first_hal_AnalogJNI
 * Method:    getAnalogSampleRate
 * Signature: ()D
 */
JNIEXPORT jdouble JNICALL
Java_edu_wpi_first_hal_AnalogJNI_getAnalogSampleRate
  (JNIEnv* env, jclass)
{
  int32_t status = 0;
  double returnValue = HAL_GetAnalogSampleRate(&status);
  ANALOGJNI_LOG(logDEBUG) << "Status = " << status;
  ANALOGJNI_LOG(logDEBUG) << "SampleRate = " << returnValue;
  CheckStatus(env, status);
  return returnValue;
}

/*
 * Class:     edu_wpi_first_hal_AnalogJNI
 * Method:    setAnalogAverageBits
 * Signature: (II)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_AnalogJNI_setAnalogAverageBits
  (JNIEnv* env, jclass, jint id, jint value)
{
  ANALOGJNI_LOG(logDEBUG) << "AverageBits = " << value;
  ANALOGJNI_LOG(logDEBUG) << "Analog Handle = " << (HAL_AnalogInputHandle)id;
  int32_t status = 0;
  HAL_SetAnalogAverageBits((HAL_AnalogInputHandle)id, value, &status);
  ANALOGJNI_LOG(logDEBUG) << "Status = " << status;
  CheckStatus(env, status);
}

/*
 * Class:     edu_wpi_first_hal_AnalogJNI
 * Method:    getAnalogAverageBits
 * Signature: (I)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_AnalogJNI_getAnalogAverageBits
  (JNIEnv* env, jclass, jint id)
{
  ANALOGJNI_LOG(logDEBUG) << "Analog Handle = " << (HAL_AnalogInputHandle)id;
  int32_t status = 0;
  jint returnValue =
      HAL_GetAnalogAverageBits((HAL_AnalogInputHandle)id, &status);
  ANALOGJNI_LOG(logDEBUG) << "Status = " << status;
  ANALOGJNI_LOG(logDEBUG) << "AverageBits = " << returnValue;
  CheckStatus(env, status);
  return returnValue;
}

/*
 * Class:     edu_wpi_first_hal_AnalogJNI
 * Method:    setAnalogOversampleBits
 * Signature: (II)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_AnalogJNI_setAnalogOversampleBits
  (JNIEnv* env, jclass, jint id, jint value)
{
  ANALOGJNI_LOG(logDEBUG) << "OversampleBits = " << value;
  ANALOGJNI_LOG(logDEBUG) << "Analog Handle = " << (HAL_AnalogInputHandle)id;
  int32_t status = 0;
  HAL_SetAnalogOversampleBits((HAL_AnalogInputHandle)id, value, &status);
  ANALOGJNI_LOG(logDEBUG) << "Status = " << status;
  CheckStatus(env, status);
}

/*
 * Class:     edu_wpi_first_hal_AnalogJNI
 * Method:    getAnalogOversampleBits
 * Signature: (I)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_AnalogJNI_getAnalogOversampleBits
  (JNIEnv* env, jclass, jint id)
{
  ANALOGJNI_LOG(logDEBUG) << "Analog Handle = " << (HAL_AnalogInputHandle)id;
  int32_t status = 0;
  jint returnValue =
      HAL_GetAnalogOversampleBits((HAL_AnalogInputHandle)id, &status);
  ANALOGJNI_LOG(logDEBUG) << "Status = " << status;
  ANALOGJNI_LOG(logDEBUG) << "OversampleBits = " << returnValue;
  CheckStatus(env, status);
  return returnValue;
}

/*
 * Class:     edu_wpi_first_hal_AnalogJNI
 * Method:    getAnalogValue
 * Signature: (I)S
 */
JNIEXPORT jshort JNICALL
Java_edu_wpi_first_hal_AnalogJNI_getAnalogValue
  (JNIEnv* env, jclass, jint id)
{
  // ANALOGJNI_LOG(logDEBUG) << "Analog Handle = " << (void*)id;
  int32_t status = 0;
  jshort returnValue = HAL_GetAnalogValue((HAL_AnalogInputHandle)id, &status);
  // ANALOGJNI_LOG(logDEBUG) << "Status = " << status;
  // ANALOGJNI_LOG(logDEBUG) << "Value = " << returnValue;
  CheckStatus(env, status);
  return returnValue;
}

/*
 * Class:     edu_wpi_first_hal_AnalogJNI
 * Method:    getAnalogAverageValue
 * Signature: (I)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_AnalogJNI_getAnalogAverageValue
  (JNIEnv* env, jclass, jint id)
{
  ANALOGJNI_LOG(logDEBUG) << "Analog Handle = " << (HAL_AnalogInputHandle)id;
  int32_t status = 0;
  jint returnValue =
      HAL_GetAnalogAverageValue((HAL_AnalogInputHandle)id, &status);
  ANALOGJNI_LOG(logDEBUG) << "Status = " << status;
  ANALOGJNI_LOG(logDEBUG) << "AverageValue = " << returnValue;
  CheckStatus(env, status);
  return returnValue;
}

/*
 * Class:     edu_wpi_first_hal_AnalogJNI
 * Method:    getAnalogVoltsToValue
 * Signature: (ID)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_AnalogJNI_getAnalogVoltsToValue
  (JNIEnv* env, jclass, jint id, jdouble voltageValue)
{
  ANALOGJNI_LOG(logDEBUG) << "Analog Handle = " << (HAL_AnalogInputHandle)id;
  ANALOGJNI_LOG(logDEBUG) << "VoltageValue = " << voltageValue;
  int32_t status = 0;
  jint returnValue = HAL_GetAnalogVoltsToValue((HAL_AnalogInputHandle)id,
                                               voltageValue, &status);
  ANALOGJNI_LOG(logDEBUG) << "Status = " << status;
  ANALOGJNI_LOG(logDEBUG) << "Value = " << returnValue;
  CheckStatus(env, status);
  return returnValue;
}

/*
 * Class:     edu_wpi_first_hal_AnalogJNI
 * Method:    getAnalogVoltage
 * Signature: (I)D
 */
JNIEXPORT jdouble JNICALL
Java_edu_wpi_first_hal_AnalogJNI_getAnalogVoltage
  (JNIEnv* env, jclass, jint id)
{
  // ANALOGJNI_LOG(logDEBUG) << "Analog Handle = " << (void*)id;
  int32_t status = 0;
  jdouble returnValue =
      HAL_GetAnalogVoltage((HAL_AnalogInputHandle)id, &status);
  // ANALOGJNI_LOG(logDEBUG) << "Status = " << status;
  // ANALOGJNI_LOG(logDEBUG) << "Voltage = " << returnValue;
  CheckStatus(env, status);
  return returnValue;
}

/*
 * Class:     edu_wpi_first_hal_AnalogJNI
 * Method:    getAnalogAverageVoltage
 * Signature: (I)D
 */
JNIEXPORT jdouble JNICALL
Java_edu_wpi_first_hal_AnalogJNI_getAnalogAverageVoltage
  (JNIEnv* env, jclass, jint id)
{
  ANALOGJNI_LOG(logDEBUG) << "Analog Handle = " << (HAL_AnalogInputHandle)id;
  int32_t status = 0;
  jdouble returnValue =
      HAL_GetAnalogAverageVoltage((HAL_AnalogInputHandle)id, &status);
  ANALOGJNI_LOG(logDEBUG) << "Status = " << status;
  ANALOGJNI_LOG(logDEBUG) << "AverageVoltage = " << returnValue;
  CheckStatus(env, status);
  return returnValue;
}

/*
 * Class:     edu_wpi_first_hal_AnalogJNI
 * Method:    getAnalogLSBWeight
 * Signature: (I)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_AnalogJNI_getAnalogLSBWeight
  (JNIEnv* env, jclass, jint id)
{
  ANALOGJNI_LOG(logDEBUG) << "Analog Handle = " << (HAL_AnalogInputHandle)id;
  int32_t status = 0;

  jint returnValue = HAL_GetAnalogLSBWeight((HAL_AnalogInputHandle)id, &status);
  ANALOGJNI_LOG(logDEBUG) << "Status = " << status;
  ANALOGJNI_LOG(logDEBUG) << "AnalogLSBWeight = " << returnValue;
  CheckStatus(env, status);
  return returnValue;
}

/*
 * Class:     edu_wpi_first_hal_AnalogJNI
 * Method:    getAnalogOffset
 * Signature: (I)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_AnalogJNI_getAnalogOffset
  (JNIEnv* env, jclass, jint id)
{
  ANALOGJNI_LOG(logDEBUG) << "Analog Handle = " << (HAL_AnalogInputHandle)id;
  int32_t status = 0;

  jint returnValue = HAL_GetAnalogOffset((HAL_AnalogInputHandle)id, &status);
  ANALOGJNI_LOG(logDEBUG) << "Status = " << status;
  ANALOGJNI_LOG(logDEBUG) << "AnalogOffset = " << returnValue;
  CheckStatus(env, status);
  return returnValue;
}

/*
 * Class:     edu_wpi_first_hal_AnalogJNI
 * Method:    isAccumulatorChannel
 * Signature: (I)Z
 */
JNIEXPORT jboolean JNICALL
Java_edu_wpi_first_hal_AnalogJNI_isAccumulatorChannel
  (JNIEnv* env, jclass, jint id)
{
  ANALOGJNI_LOG(logDEBUG) << "isAccumulatorChannel";
  ANALOGJNI_LOG(logDEBUG) << "Analog Handle = " << (HAL_AnalogInputHandle)id;
  int32_t status = 0;

  jboolean returnValue =
      HAL_IsAccumulatorChannel((HAL_AnalogInputHandle)id, &status);
  ANALOGJNI_LOG(logDEBUG) << "Status = " << status;
  ANALOGJNI_LOG(logDEBUG) << "AnalogOffset = " << returnValue;
  CheckStatus(env, status);
  return returnValue;
}

/*
 * Class:     edu_wpi_first_hal_AnalogJNI
 * Method:    initAccumulator
 * Signature: (I)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_AnalogJNI_initAccumulator
  (JNIEnv* env, jclass, jint id)
{
  ANALOGJNI_LOG(logDEBUG) << "Analog Handle = " << (HAL_AnalogInputHandle)id;
  int32_t status = 0;
  HAL_InitAccumulator((HAL_AnalogInputHandle)id, &status);
  ANALOGJNI_LOG(logDEBUG) << "Status = " << status;
  CheckStatus(env, status);
}

/*
 * Class:     edu_wpi_first_hal_AnalogJNI
 * Method:    resetAccumulator
 * Signature: (I)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_AnalogJNI_resetAccumulator
  (JNIEnv* env, jclass, jint id)
{
  ANALOGJNI_LOG(logDEBUG) << "Analog Handle = " << (HAL_AnalogInputHandle)id;
  int32_t status = 0;
  HAL_ResetAccumulator((HAL_AnalogInputHandle)id, &status);
  ANALOGJNI_LOG(logDEBUG) << "Status = " << status;
  CheckStatus(env, status);
}

/*
 * Class:     edu_wpi_first_hal_AnalogJNI
 * Method:    setAccumulatorCenter
 * Signature: (II)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_AnalogJNI_setAccumulatorCenter
  (JNIEnv* env, jclass, jint id, jint center)
{
  ANALOGJNI_LOG(logDEBUG) << "Analog Handle = " << (HAL_AnalogInputHandle)id;
  int32_t status = 0;
  HAL_SetAccumulatorCenter((HAL_AnalogInputHandle)id, center, &status);
  ANALOGJNI_LOG(logDEBUG) << "Status = " << status;
  CheckStatus(env, status);
}

/*
 * Class:     edu_wpi_first_hal_AnalogJNI
 * Method:    setAccumulatorDeadband
 * Signature: (II)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_AnalogJNI_setAccumulatorDeadband
  (JNIEnv* env, jclass, jint id, jint deadband)
{
  ANALOGJNI_LOG(logDEBUG) << "Analog Handle = " << (HAL_AnalogInputHandle)id;
  int32_t status = 0;
  HAL_SetAccumulatorDeadband((HAL_AnalogInputHandle)id, deadband, &status);
  ANALOGJNI_LOG(logDEBUG) << "Status = " << status;
  CheckStatus(env, status);
}

/*
 * Class:     edu_wpi_first_hal_AnalogJNI
 * Method:    getAccumulatorValue
 * Signature: (I)J
 */
JNIEXPORT jlong JNICALL
Java_edu_wpi_first_hal_AnalogJNI_getAccumulatorValue
  (JNIEnv* env, jclass, jint id)
{
  ANALOGJNI_LOG(logDEBUG) << "Analog Handle = " << (HAL_AnalogInputHandle)id;
  int32_t status = 0;
  jlong returnValue =
      HAL_GetAccumulatorValue((HAL_AnalogInputHandle)id, &status);
  ANALOGJNI_LOG(logDEBUG) << "Status = " << status;
  ANALOGJNI_LOG(logDEBUG) << "AccumulatorValue = " << returnValue;
  CheckStatus(env, status);

  return returnValue;
}

/*
 * Class:     edu_wpi_first_hal_AnalogJNI
 * Method:    getAccumulatorCount
 * Signature: (I)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_AnalogJNI_getAccumulatorCount
  (JNIEnv* env, jclass, jint id)
{
  ANALOGJNI_LOG(logDEBUG) << "Analog Handle = " << (HAL_AnalogInputHandle)id;
  int32_t status = 0;
  jint returnValue =
      HAL_GetAccumulatorCount((HAL_AnalogInputHandle)id, &status);
  ANALOGJNI_LOG(logDEBUG) << "Status = " << status;
  ANALOGJNI_LOG(logDEBUG) << "AccumulatorCount = " << returnValue;
  CheckStatus(env, status);
  return returnValue;
}

/*
 * Class:     edu_wpi_first_hal_AnalogJNI
 * Method:    getAccumulatorOutput
 * Signature: (ILjava/lang/Object;)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_AnalogJNI_getAccumulatorOutput
  (JNIEnv* env, jclass, jint id, jobject accumulatorResult)
{
  ANALOGJNI_LOG(logDEBUG) << "Analog Handle = " << (HAL_AnalogInputHandle)id;
  int32_t status = 0;
  int64_t value = 0;
  int64_t count = 0;
  HAL_GetAccumulatorOutput((HAL_AnalogInputHandle)id, &value, &count, &status);
  SetAccumulatorResultObject(env, accumulatorResult, value, count);
  ANALOGJNI_LOG(logDEBUG) << "Value = " << value;
  ANALOGJNI_LOG(logDEBUG) << "Count = " << count;
  ANALOGJNI_LOG(logDEBUG) << "Status = " << status;
  CheckStatus(env, status);
}

/*
 * Class:     edu_wpi_first_hal_AnalogJNI
 * Method:    initializeAnalogTrigger
 * Signature: (ILjava/lang/Object;)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_AnalogJNI_initializeAnalogTrigger
  (JNIEnv* env, jclass, jint id, jobject index)
{
  ANALOGJNI_LOG(logDEBUG) << "Port Handle = " << (HAL_AnalogInputHandle)id;
  jint* indexHandle =
      reinterpret_cast<jint*>(env->GetDirectBufferAddress(index));
  ANALOGJNI_LOG(logDEBUG) << "Index Ptr = " << indexHandle;
  int32_t status = 0;
  HAL_AnalogTriggerHandle analogTrigger = HAL_InitializeAnalogTrigger(
      (HAL_AnalogInputHandle)id, reinterpret_cast<int32_t*>(indexHandle),
      &status);
  ANALOGJNI_LOG(logDEBUG) << "Status = " << status;
  ANALOGJNI_LOG(logDEBUG) << "AnalogTrigger Handle = " << analogTrigger;
  CheckStatus(env, status);
  return (jint)analogTrigger;
}

/*
 * Class:     edu_wpi_first_hal_AnalogJNI
 * Method:    cleanAnalogTrigger
 * Signature: (I)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_AnalogJNI_cleanAnalogTrigger
  (JNIEnv* env, jclass, jint id)
{
  ANALOGJNI_LOG(logDEBUG) << "Analog Trigger Handle = "
                          << (HAL_AnalogTriggerHandle)id;
  int32_t status = 0;
  HAL_CleanAnalogTrigger((HAL_AnalogTriggerHandle)id, &status);
  CheckStatus(env, status);
}

/*
 * Class:     edu_wpi_first_hal_AnalogJNI
 * Method:    setAnalogTriggerLimitsRaw
 * Signature: (III)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_AnalogJNI_setAnalogTriggerLimitsRaw
  (JNIEnv* env, jclass, jint id, jint lower, jint upper)
{
  ANALOGJNI_LOG(logDEBUG) << "Analog Trigger Handle = "
                          << (HAL_AnalogTriggerHandle)id;
  int32_t status = 0;
  HAL_SetAnalogTriggerLimitsRaw((HAL_AnalogTriggerHandle)id, lower, upper,
                                &status);
  CheckStatus(env, status);
}

/*
 * Class:     edu_wpi_first_hal_AnalogJNI
 * Method:    setAnalogTriggerLimitsVoltage
 * Signature: (IDD)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_AnalogJNI_setAnalogTriggerLimitsVoltage
  (JNIEnv* env, jclass, jint id, jdouble lower, jdouble upper)
{
  ANALOGJNI_LOG(logDEBUG) << "Analog Trigger Handle = "
                          << (HAL_AnalogTriggerHandle)id;
  int32_t status = 0;
  HAL_SetAnalogTriggerLimitsVoltage((HAL_AnalogTriggerHandle)id, lower, upper,
                                    &status);
  CheckStatus(env, status);
}

/*
 * Class:     edu_wpi_first_hal_AnalogJNI
 * Method:    setAnalogTriggerAveraged
 * Signature: (IZ)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_AnalogJNI_setAnalogTriggerAveraged
  (JNIEnv* env, jclass, jint id, jboolean averaged)
{
  ANALOGJNI_LOG(logDEBUG) << "Analog Trigger Handle = "
                          << (HAL_AnalogTriggerHandle)id;
  int32_t status = 0;
  HAL_SetAnalogTriggerAveraged((HAL_AnalogTriggerHandle)id, averaged, &status);
  CheckStatus(env, status);
}

/*
 * Class:     edu_wpi_first_hal_AnalogJNI
 * Method:    setAnalogTriggerFiltered
 * Signature: (IZ)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_AnalogJNI_setAnalogTriggerFiltered
  (JNIEnv* env, jclass, jint id, jboolean filtered)
{
  ANALOGJNI_LOG(logDEBUG) << "Analog Trigger Handle = "
                          << (HAL_AnalogTriggerHandle)id;
  int32_t status = 0;
  HAL_SetAnalogTriggerFiltered((HAL_AnalogTriggerHandle)id, filtered, &status);
  CheckStatus(env, status);
}

/*
 * Class:     edu_wpi_first_hal_AnalogJNI
 * Method:    getAnalogTriggerInWindow
 * Signature: (I)Z
 */
JNIEXPORT jboolean JNICALL
Java_edu_wpi_first_hal_AnalogJNI_getAnalogTriggerInWindow
  (JNIEnv* env, jclass, jint id)
{
  ANALOGJNI_LOG(logDEBUG) << "Analog Trigger Handle = "
                          << (HAL_AnalogTriggerHandle)id;
  int32_t status = 0;
  jboolean val =
      HAL_GetAnalogTriggerInWindow((HAL_AnalogTriggerHandle)id, &status);
  CheckStatus(env, status);
  return val;
}

/*
 * Class:     edu_wpi_first_hal_AnalogJNI
 * Method:    getAnalogTriggerTriggerState
 * Signature: (I)Z
 */
JNIEXPORT jboolean JNICALL
Java_edu_wpi_first_hal_AnalogJNI_getAnalogTriggerTriggerState
  (JNIEnv* env, jclass, jint id)
{
  ANALOGJNI_LOG(logDEBUG) << "Analog Trigger Handle = "
                          << (HAL_AnalogTriggerHandle)id;
  int32_t status = 0;
  jboolean val =
      HAL_GetAnalogTriggerTriggerState((HAL_AnalogTriggerHandle)id, &status);
  CheckStatus(env, status);
  return val;
}

/*
 * Class:     edu_wpi_first_hal_AnalogJNI
 * Method:    getAnalogTriggerOutput
 * Signature: (II)Z
 */
JNIEXPORT jboolean JNICALL
Java_edu_wpi_first_hal_AnalogJNI_getAnalogTriggerOutput
  (JNIEnv* env, jclass, jint id, jint type)
{
  ANALOGJNI_LOG(logDEBUG) << "Analog Trigger Handle = "
                          << (HAL_AnalogTriggerHandle)id;
  int32_t status = 0;
  jboolean val = HAL_GetAnalogTriggerOutput(
      (HAL_AnalogTriggerHandle)id, (HAL_AnalogTriggerType)type, &status);
  CheckStatus(env, status);
  return val;
}

}  // extern "C"
