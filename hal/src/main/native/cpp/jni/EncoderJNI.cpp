// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <jni.h>

#include <cassert>

#include "HALUtil.h"
#include "edu_wpi_first_hal_EncoderJNI.h"
#include "hal/Encoder.h"
#include "hal/Errors.h"

using namespace hal;

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
  int32_t status = 0;
  auto encoder = HAL_InitializeEncoder(
      (HAL_Handle)digitalSourceHandleA,
      (HAL_AnalogTriggerType)analogTriggerTypeA,
      (HAL_Handle)digitalSourceHandleB,
      (HAL_AnalogTriggerType)analogTriggerTypeB, reverseDirection,
      (HAL_EncoderEncodingType)encodingType, &status);
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
  if (id != HAL_kInvalidHandle) {
    HAL_FreeEncoder((HAL_EncoderHandle)id);
  }
}

/*
 * Class:     edu_wpi_first_hal_EncoderJNI
 * Method:    setEncoderSimDevice
 * Signature: (II)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_EncoderJNI_setEncoderSimDevice
  (JNIEnv* env, jclass, jint handle, jint device)
{
  HAL_SetEncoderSimDevice((HAL_EncoderHandle)handle,
                          (HAL_SimDeviceHandle)device);
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
  int32_t status = 0;
  jint returnValue = HAL_GetEncoder((HAL_EncoderHandle)id, &status);
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
  int32_t status = 0;
  jint returnValue = HAL_GetEncoderRaw((HAL_EncoderHandle)id, &status);
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
  int32_t status = 0;
  jint returnValue =
      HAL_GetEncoderEncodingScale((HAL_EncoderHandle)id, &status);
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
  int32_t status = 0;
  HAL_ResetEncoder((HAL_EncoderHandle)id, &status);
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
  int32_t status = 0;
  double returnValue = HAL_GetEncoderPeriod((HAL_EncoderHandle)id, &status);
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
  int32_t status = 0;
  HAL_SetEncoderMaxPeriod((HAL_EncoderHandle)id, value, &status);
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
  int32_t status = 0;
  jboolean returnValue = HAL_GetEncoderStopped((HAL_EncoderHandle)id, &status);
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
  int32_t status = 0;
  jboolean returnValue =
      HAL_GetEncoderDirection((HAL_EncoderHandle)id, &status);
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
  int32_t status = 0;
  jdouble returnValue = HAL_GetEncoderDistance((HAL_EncoderHandle)id, &status);
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
  int32_t status = 0;
  jdouble returnValue = HAL_GetEncoderRate((HAL_EncoderHandle)id, &status);
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
  int32_t status = 0;
  HAL_SetEncoderMinRate((HAL_EncoderHandle)id, value, &status);
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
  int32_t status = 0;
  HAL_SetEncoderDistancePerPulse((HAL_EncoderHandle)id, value, &status);
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
  int32_t status = 0;
  HAL_SetEncoderReverseDirection((HAL_EncoderHandle)id, value, &status);
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
  int32_t status = 0;
  HAL_SetEncoderSamplesToAverage((HAL_EncoderHandle)id, value, &status);
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
  int32_t status = 0;
  jint returnValue =
      HAL_GetEncoderSamplesToAverage((HAL_EncoderHandle)id, &status);
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
  int32_t status = 0;
  HAL_SetEncoderIndexSource((HAL_EncoderHandle)id,
                            (HAL_Handle)digitalSourceHandle,
                            (HAL_AnalogTriggerType)analogTriggerType,
                            (HAL_EncoderIndexingType)type, &status);
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
  int32_t status = 0;
  jint returnValue = HAL_GetEncoderFPGAIndex((HAL_EncoderHandle)id, &status);
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
  int32_t status = 0;
  jint returnValue =
      HAL_GetEncoderEncodingScale((HAL_EncoderHandle)id, &status);
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
  int32_t status = 0;
  jdouble returnValue =
      HAL_GetEncoderDecodingScaleFactor((HAL_EncoderHandle)id, &status);
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
  int32_t status = 0;
  jdouble returnValue =
      HAL_GetEncoderDistancePerPulse((HAL_EncoderHandle)id, &status);
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
  int32_t status = 0;
  jint returnValue = HAL_GetEncoderEncodingType((HAL_EncoderHandle)id, &status);
  CheckStatus(env, status);
  return returnValue;
}

}  // extern "C"
