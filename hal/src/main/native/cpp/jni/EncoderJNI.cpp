/*----------------------------------------------------------------------------*/
/* Copyright (c) 2016-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include <jni.h>

#include <cassert>

#include "HALUtil.h"
#include "edu_wpi_first_hal_EncoderJNI.h"
#include "hal/Encoder.h"
#include "hal/Errors.h"
#include "hal/cpp/Log.h"

using namespace frc;

// set the logging level
TLogLevel encoderJNILogLevel = logWARNING;

#define ENCODERJNI_LOG(level)     \
  if (level > encoderJNILogLevel) \
    ;                             \
  else                            \
    Log().Get(level)

extern "C" {

/*
 * Class:     edu_wpi_first_hal_EncoderJNI
 * Method:    initializeEncoder
 * Signature: (IIIIZI)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_EncoderJNI_initializeEncoder
  (JNIEnv* env, jclass, jint digitalSourceHandleA, jint analogTriggerTypeA,
   jint digitalSourceHandleB, jint analogTriggerTypeB,
   jboolean reverseDirection, jint encodingType)
{
  ENCODERJNI_LOG(logDEBUG) << "Calling ENCODERJNI initializeEncoder";
  ENCODERJNI_LOG(logDEBUG) << "Source Handle A = " << digitalSourceHandleA;
  ENCODERJNI_LOG(logDEBUG) << "Analog Trigger Type A = " << analogTriggerTypeA;
  ENCODERJNI_LOG(logDEBUG) << "Source Handle B = " << digitalSourceHandleB;
  ENCODERJNI_LOG(logDEBUG) << "Analog Trigger Type B = " << analogTriggerTypeB;
  ENCODERJNI_LOG(logDEBUG) << "Reverse direction = " << (jint)reverseDirection;
  ENCODERJNI_LOG(logDEBUG) << "EncodingType = " << encodingType;
  int32_t status = 0;
  auto encoder = HAL_InitializeEncoder(
      (HAL_Handle)digitalSourceHandleA,
      (HAL_AnalogTriggerType)analogTriggerTypeA,
      (HAL_Handle)digitalSourceHandleB,
      (HAL_AnalogTriggerType)analogTriggerTypeB, reverseDirection,
      (HAL_EncoderEncodingType)encodingType, &status);

  ENCODERJNI_LOG(logDEBUG) << "Status = " << status;
  ENCODERJNI_LOG(logDEBUG) << "ENCODER Handle = " << encoder;
  CheckStatusForceThrow(env, status);
  return (jint)encoder;
}

/*
 * Class:     edu_wpi_first_hal_EncoderJNI
 * Method:    freeEncoder
 * Signature: (I)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_EncoderJNI_freeEncoder
  (JNIEnv* env, jclass, jint id)
{
  ENCODERJNI_LOG(logDEBUG) << "Calling ENCODERJNI freeEncoder";
  ENCODERJNI_LOG(logDEBUG) << "Encoder Handle = " << (HAL_EncoderHandle)id;
  int32_t status = 0;
  HAL_FreeEncoder((HAL_EncoderHandle)id, &status);
  ENCODERJNI_LOG(logDEBUG) << "Status = " << status;
  CheckStatus(env, status);
}

/*
 * Class:     edu_wpi_first_hal_EncoderJNI
 * Method:    getEncoder
 * Signature: (I)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_EncoderJNI_getEncoder
  (JNIEnv* env, jclass, jint id)
{
  ENCODERJNI_LOG(logDEBUG) << "Calling ENCODERJNI getEncoder";
  ENCODERJNI_LOG(logDEBUG) << "Encoder Handle = " << (HAL_EncoderHandle)id;
  int32_t status = 0;
  jint returnValue = HAL_GetEncoder((HAL_EncoderHandle)id, &status);
  ENCODERJNI_LOG(logDEBUG) << "Status = " << status;
  ENCODERJNI_LOG(logDEBUG) << "getEncoderResult = " << returnValue;
  CheckStatus(env, status);
  return returnValue;
}

/*
 * Class:     edu_wpi_first_hal_EncoderJNI
 * Method:    getEncoderRaw
 * Signature: (I)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_EncoderJNI_getEncoderRaw
  (JNIEnv* env, jclass, jint id)
{
  ENCODERJNI_LOG(logDEBUG) << "Calling ENCODERJNI getEncoderRaw";
  ENCODERJNI_LOG(logDEBUG) << "Encoder Handle = " << (HAL_EncoderHandle)id;
  int32_t status = 0;
  jint returnValue = HAL_GetEncoderRaw((HAL_EncoderHandle)id, &status);
  ENCODERJNI_LOG(logDEBUG) << "Status = " << status;
  ENCODERJNI_LOG(logDEBUG) << "getRawEncoderResult = " << returnValue;
  CheckStatus(env, status);
  return returnValue;
}

/*
 * Class:     edu_wpi_first_hal_EncoderJNI
 * Method:    getEncodingScaleFactor
 * Signature: (I)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_EncoderJNI_getEncodingScaleFactor
  (JNIEnv* env, jclass, jint id)
{
  ENCODERJNI_LOG(logDEBUG) << "Calling ENCODERJNI getEncodingScaleFactor";
  ENCODERJNI_LOG(logDEBUG) << "Encoder Handle = " << (HAL_EncoderHandle)id;
  int32_t status = 0;
  jint returnValue =
      HAL_GetEncoderEncodingScale((HAL_EncoderHandle)id, &status);
  ENCODERJNI_LOG(logDEBUG) << "Status = " << status;
  ENCODERJNI_LOG(logDEBUG) << "getEncodingScaleFactorResult = " << returnValue;
  CheckStatus(env, status);
  return returnValue;
}

/*
 * Class:     edu_wpi_first_hal_EncoderJNI
 * Method:    resetEncoder
 * Signature: (I)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_EncoderJNI_resetEncoder
  (JNIEnv* env, jclass, jint id)
{
  ENCODERJNI_LOG(logDEBUG) << "Calling ENCODERJNI resetEncoder";
  ENCODERJNI_LOG(logDEBUG) << "Encoder Handle = " << (HAL_EncoderHandle)id;
  int32_t status = 0;
  HAL_ResetEncoder((HAL_EncoderHandle)id, &status);
  ENCODERJNI_LOG(logDEBUG) << "Status = " << status;
  CheckStatus(env, status);
}

/*
 * Class:     edu_wpi_first_hal_EncoderJNI
 * Method:    getEncoderPeriod
 * Signature: (I)D
 */
JNIEXPORT jdouble JNICALL
Java_edu_wpi_first_hal_EncoderJNI_getEncoderPeriod
  (JNIEnv* env, jclass, jint id)
{
  ENCODERJNI_LOG(logDEBUG) << "Calling ENCODERJNI getEncoderPeriod";
  ENCODERJNI_LOG(logDEBUG) << "Encoder Handle = " << (HAL_EncoderHandle)id;
  int32_t status = 0;
  double returnValue = HAL_GetEncoderPeriod((HAL_EncoderHandle)id, &status);
  ENCODERJNI_LOG(logDEBUG) << "Status = " << status;
  ENCODERJNI_LOG(logDEBUG) << "getEncoderPeriodEncoderResult = " << returnValue;
  CheckStatus(env, status);
  return returnValue;
}

/*
 * Class:     edu_wpi_first_hal_EncoderJNI
 * Method:    setEncoderMaxPeriod
 * Signature: (ID)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_EncoderJNI_setEncoderMaxPeriod
  (JNIEnv* env, jclass, jint id, jdouble value)
{
  ENCODERJNI_LOG(logDEBUG) << "Calling ENCODERJNI setEncoderMaxPeriod";
  ENCODERJNI_LOG(logDEBUG) << "Encoder Handle = " << (HAL_EncoderHandle)id;
  int32_t status = 0;
  HAL_SetEncoderMaxPeriod((HAL_EncoderHandle)id, value, &status);
  ENCODERJNI_LOG(logDEBUG) << "Status = " << status;
  CheckStatus(env, status);
}

/*
 * Class:     edu_wpi_first_hal_EncoderJNI
 * Method:    getEncoderStopped
 * Signature: (I)Z
 */
JNIEXPORT jboolean JNICALL
Java_edu_wpi_first_hal_EncoderJNI_getEncoderStopped
  (JNIEnv* env, jclass, jint id)
{
  ENCODERJNI_LOG(logDEBUG) << "Calling ENCODERJNI getEncoderStopped";
  ENCODERJNI_LOG(logDEBUG) << "Encoder Handle = " << (HAL_EncoderHandle)id;
  int32_t status = 0;
  jboolean returnValue = HAL_GetEncoderStopped((HAL_EncoderHandle)id, &status);
  ENCODERJNI_LOG(logDEBUG) << "Status = " << status;
  ENCODERJNI_LOG(logDEBUG) << "getStoppedEncoderResult = " << returnValue;
  CheckStatus(env, status);
  return returnValue;
}

/*
 * Class:     edu_wpi_first_hal_EncoderJNI
 * Method:    getEncoderDirection
 * Signature: (I)Z
 */
JNIEXPORT jboolean JNICALL
Java_edu_wpi_first_hal_EncoderJNI_getEncoderDirection
  (JNIEnv* env, jclass, jint id)
{
  ENCODERJNI_LOG(logDEBUG) << "Calling ENCODERJNI getEncoderDirection";
  ENCODERJNI_LOG(logDEBUG) << "Encoder Handle = " << (HAL_EncoderHandle)id;
  int32_t status = 0;
  jboolean returnValue =
      HAL_GetEncoderDirection((HAL_EncoderHandle)id, &status);
  ENCODERJNI_LOG(logDEBUG) << "Status = " << status;
  ENCODERJNI_LOG(logDEBUG) << "getDirectionEncoderResult = " << returnValue;
  CheckStatus(env, status);
  return returnValue;
}

/*
 * Class:     edu_wpi_first_hal_EncoderJNI
 * Method:    getEncoderDistance
 * Signature: (I)D
 */
JNIEXPORT jdouble JNICALL
Java_edu_wpi_first_hal_EncoderJNI_getEncoderDistance
  (JNIEnv* env, jclass, jint id)
{
  ENCODERJNI_LOG(logDEBUG) << "Calling ENCODERJNI getEncoderDistance";
  ENCODERJNI_LOG(logDEBUG) << "Encoder Handle = " << (HAL_EncoderHandle)id;
  int32_t status = 0;
  jdouble returnValue = HAL_GetEncoderDistance((HAL_EncoderHandle)id, &status);
  ENCODERJNI_LOG(logDEBUG) << "Status = " << status;
  ENCODERJNI_LOG(logDEBUG) << "getDistanceEncoderResult = " << returnValue;
  CheckStatus(env, status);
  return returnValue;
}

/*
 * Class:     edu_wpi_first_hal_EncoderJNI
 * Method:    getEncoderRate
 * Signature: (I)D
 */
JNIEXPORT jdouble JNICALL
Java_edu_wpi_first_hal_EncoderJNI_getEncoderRate
  (JNIEnv* env, jclass, jint id)
{
  ENCODERJNI_LOG(logDEBUG) << "Calling ENCODERJNI getEncoderRate";
  ENCODERJNI_LOG(logDEBUG) << "Encoder Handle = " << (HAL_EncoderHandle)id;
  int32_t status = 0;
  jdouble returnValue = HAL_GetEncoderRate((HAL_EncoderHandle)id, &status);
  ENCODERJNI_LOG(logDEBUG) << "Status = " << status;
  ENCODERJNI_LOG(logDEBUG) << "getRateEncoderResult = " << returnValue;
  CheckStatus(env, status);
  return returnValue;
}

/*
 * Class:     edu_wpi_first_hal_EncoderJNI
 * Method:    setEncoderMinRate
 * Signature: (ID)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_EncoderJNI_setEncoderMinRate
  (JNIEnv* env, jclass, jint id, jdouble value)
{
  ENCODERJNI_LOG(logDEBUG) << "Calling ENCODERJNI setEncoderMinRate";
  ENCODERJNI_LOG(logDEBUG) << "Encoder Handle = " << (HAL_EncoderHandle)id;
  int32_t status = 0;
  HAL_SetEncoderMinRate((HAL_EncoderHandle)id, value, &status);
  ENCODERJNI_LOG(logDEBUG) << "Status = " << status;
  CheckStatus(env, status);
}

/*
 * Class:     edu_wpi_first_hal_EncoderJNI
 * Method:    setEncoderDistancePerPulse
 * Signature: (ID)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_EncoderJNI_setEncoderDistancePerPulse
  (JNIEnv* env, jclass, jint id, jdouble value)
{
  ENCODERJNI_LOG(logDEBUG) << "Calling ENCODERJNI setEncoderDistancePerPulse";
  ENCODERJNI_LOG(logDEBUG) << "Encoder Handle = " << (HAL_EncoderHandle)id;
  int32_t status = 0;
  HAL_SetEncoderDistancePerPulse((HAL_EncoderHandle)id, value, &status);
  ENCODERJNI_LOG(logDEBUG) << "Status = " << status;
  CheckStatus(env, status);
}

/*
 * Class:     edu_wpi_first_hal_EncoderJNI
 * Method:    setEncoderReverseDirection
 * Signature: (IZ)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_EncoderJNI_setEncoderReverseDirection
  (JNIEnv* env, jclass, jint id, jboolean value)
{
  ENCODERJNI_LOG(logDEBUG) << "Calling ENCODERJNI setEncoderReverseDirection";
  ENCODERJNI_LOG(logDEBUG) << "Encoder Handle = " << (HAL_EncoderHandle)id;
  int32_t status = 0;
  HAL_SetEncoderReverseDirection((HAL_EncoderHandle)id, value, &status);
  ENCODERJNI_LOG(logDEBUG) << "Status = " << status;
  CheckStatus(env, status);
}

/*
 * Class:     edu_wpi_first_hal_EncoderJNI
 * Method:    setEncoderSamplesToAverage
 * Signature: (II)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_EncoderJNI_setEncoderSamplesToAverage
  (JNIEnv* env, jclass, jint id, jint value)
{
  ENCODERJNI_LOG(logDEBUG) << "Calling ENCODERJNI setEncoderSamplesToAverage";
  ENCODERJNI_LOG(logDEBUG) << "Encoder Handle = " << (HAL_EncoderHandle)id;
  int32_t status = 0;
  HAL_SetEncoderSamplesToAverage((HAL_EncoderHandle)id, value, &status);
  ENCODERJNI_LOG(logDEBUG) << "Status = " << status;
  if (status == PARAMETER_OUT_OF_RANGE) {
    ThrowBoundaryException(env, value, 1, 127);
    return;
  }
  CheckStatus(env, status);
}

/*
 * Class:     edu_wpi_first_hal_EncoderJNI
 * Method:    getEncoderSamplesToAverage
 * Signature: (I)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_EncoderJNI_getEncoderSamplesToAverage
  (JNIEnv* env, jclass, jint id)
{
  ENCODERJNI_LOG(logDEBUG) << "Calling ENCODERJNI getEncoderSamplesToAverage";
  ENCODERJNI_LOG(logDEBUG) << "Encoder Handle = " << (HAL_EncoderHandle)id;
  int32_t status = 0;
  jint returnValue =
      HAL_GetEncoderSamplesToAverage((HAL_EncoderHandle)id, &status);
  ENCODERJNI_LOG(logDEBUG) << "Status = " << status;
  ENCODERJNI_LOG(logDEBUG) << "getEncoderSamplesToAverageResult = "
                           << returnValue;
  CheckStatus(env, status);
  return returnValue;
}

/*
 * Class:     edu_wpi_first_hal_EncoderJNI
 * Method:    setEncoderIndexSource
 * Signature: (IIII)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_EncoderJNI_setEncoderIndexSource
  (JNIEnv* env, jclass, jint id, jint digitalSourceHandle,
   jint analogTriggerType, jint type)
{
  ENCODERJNI_LOG(logDEBUG) << "Calling ENCODERJNI setEncoderIndexSource";
  ENCODERJNI_LOG(logDEBUG) << "Encoder Handle = " << (HAL_EncoderHandle)id;
  ENCODERJNI_LOG(logDEBUG) << "Source Handle = " << digitalSourceHandle;
  ENCODERJNI_LOG(logDEBUG) << "Analog Trigger Type = " << analogTriggerType;
  ENCODERJNI_LOG(logDEBUG) << "IndexingType = " << type;
  int32_t status = 0;
  HAL_SetEncoderIndexSource((HAL_EncoderHandle)id,
                            (HAL_Handle)digitalSourceHandle,
                            (HAL_AnalogTriggerType)analogTriggerType,
                            (HAL_EncoderIndexingType)type, &status);
  ENCODERJNI_LOG(logDEBUG) << "Status = " << status;
  CheckStatus(env, status);
}

/*
 * Class:     edu_wpi_first_hal_EncoderJNI
 * Method:    getEncoderFPGAIndex
 * Signature: (I)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_EncoderJNI_getEncoderFPGAIndex
  (JNIEnv* env, jclass, jint id)
{
  ENCODERJNI_LOG(logDEBUG) << "Calling ENCODERJNI getEncoderSamplesToAverage";
  ENCODERJNI_LOG(logDEBUG) << "Encoder Handle = " << (HAL_EncoderHandle)id;
  int32_t status = 0;
  jint returnValue = HAL_GetEncoderFPGAIndex((HAL_EncoderHandle)id, &status);
  ENCODERJNI_LOG(logDEBUG) << "Status = " << status;
  ENCODERJNI_LOG(logDEBUG) << "getEncoderSamplesToAverageResult = "
                           << returnValue;
  CheckStatus(env, status);
  return returnValue;
}

/*
 * Class:     edu_wpi_first_hal_EncoderJNI
 * Method:    getEncoderEncodingScale
 * Signature: (I)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_EncoderJNI_getEncoderEncodingScale
  (JNIEnv* env, jclass, jint id)
{
  ENCODERJNI_LOG(logDEBUG) << "Calling ENCODERJNI getEncoderSamplesToAverage";
  ENCODERJNI_LOG(logDEBUG) << "Encoder Handle = " << (HAL_EncoderHandle)id;
  int32_t status = 0;
  jint returnValue =
      HAL_GetEncoderEncodingScale((HAL_EncoderHandle)id, &status);
  ENCODERJNI_LOG(logDEBUG) << "Status = " << status;
  ENCODERJNI_LOG(logDEBUG) << "getEncoderSamplesToAverageResult = "
                           << returnValue;
  CheckStatus(env, status);
  return returnValue;
}

/*
 * Class:     edu_wpi_first_hal_EncoderJNI
 * Method:    getEncoderDecodingScaleFactor
 * Signature: (I)D
 */
JNIEXPORT jdouble JNICALL
Java_edu_wpi_first_hal_EncoderJNI_getEncoderDecodingScaleFactor
  (JNIEnv* env, jclass, jint id)
{
  ENCODERJNI_LOG(logDEBUG) << "Calling ENCODERJNI getEncoderSamplesToAverage";
  ENCODERJNI_LOG(logDEBUG) << "Encoder Handle = " << (HAL_EncoderHandle)id;
  int32_t status = 0;
  jdouble returnValue =
      HAL_GetEncoderDecodingScaleFactor((HAL_EncoderHandle)id, &status);
  ENCODERJNI_LOG(logDEBUG) << "Status = " << status;
  ENCODERJNI_LOG(logDEBUG) << "getEncoderSamplesToAverageResult = "
                           << returnValue;
  CheckStatus(env, status);
  return returnValue;
}

/*
 * Class:     edu_wpi_first_hal_EncoderJNI
 * Method:    getEncoderDistancePerPulse
 * Signature: (I)D
 */
JNIEXPORT jdouble JNICALL
Java_edu_wpi_first_hal_EncoderJNI_getEncoderDistancePerPulse
  (JNIEnv* env, jclass, jint id)
{
  ENCODERJNI_LOG(logDEBUG) << "Calling ENCODERJNI getEncoderSamplesToAverage";
  ENCODERJNI_LOG(logDEBUG) << "Encoder Handle = " << (HAL_EncoderHandle)id;
  int32_t status = 0;
  jdouble returnValue =
      HAL_GetEncoderDistancePerPulse((HAL_EncoderHandle)id, &status);
  ENCODERJNI_LOG(logDEBUG) << "Status = " << status;
  ENCODERJNI_LOG(logDEBUG) << "getEncoderSamplesToAverageResult = "
                           << returnValue;
  CheckStatus(env, status);
  return returnValue;
}

/*
 * Class:     edu_wpi_first_hal_EncoderJNI
 * Method:    getEncoderEncodingType
 * Signature: (I)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_EncoderJNI_getEncoderEncodingType
  (JNIEnv* env, jclass, jint id)
{
  ENCODERJNI_LOG(logDEBUG) << "Calling ENCODERJNI getEncoderSamplesToAverage";
  ENCODERJNI_LOG(logDEBUG) << "Encoder Handle = " << (HAL_EncoderHandle)id;
  int32_t status = 0;
  jint returnValue = HAL_GetEncoderEncodingType((HAL_EncoderHandle)id, &status);
  ENCODERJNI_LOG(logDEBUG) << "Status = " << status;
  ENCODERJNI_LOG(logDEBUG) << "getEncoderSamplesToAverageResult = "
                           << returnValue;
  CheckStatus(env, status);
  return returnValue;
}

}  // extern "C"
