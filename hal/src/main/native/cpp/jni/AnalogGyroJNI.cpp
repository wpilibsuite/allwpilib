// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <jni.h>

#include <cassert>

#include <wpi/jni_util.h>

#include "HALUtil.h"
#include "edu_wpi_first_hal_AnalogGyroJNI.h"
#include "hal/AnalogGyro.h"
#include "hal/handles/HandlesInternal.h"

using namespace hal;

extern "C" {
/*
 * Class:     edu_wpi_first_hal_AnalogGyroJNI
 * Method:    initializeAnalogGyro
 * Signature: (I)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_AnalogGyroJNI_initializeAnalogGyro
  (JNIEnv* env, jclass, jint id)
{
  int32_t status = 0;
  auto stack = wpi::java::GetJavaStackTrace(env, "edu.wpi.first");
  HAL_GyroHandle handle = HAL_InitializeAnalogGyro((HAL_AnalogInputHandle)id,
                                                   stack.c_str(), &status);
  // Analog input does range checking, so we don't need to do so.
  CheckStatusForceThrow(env, status);
  return (jint)handle;
}

/*
 * Class:     edu_wpi_first_hal_AnalogGyroJNI
 * Method:    setupAnalogGyro
 * Signature: (I)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_AnalogGyroJNI_setupAnalogGyro
  (JNIEnv* env, jclass, jint id)
{
  int32_t status = 0;
  HAL_SetupAnalogGyro((HAL_GyroHandle)id, &status);
  CheckStatus(env, status);
}

/*
 * Class:     edu_wpi_first_hal_AnalogGyroJNI
 * Method:    freeAnalogGyro
 * Signature: (I)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_AnalogGyroJNI_freeAnalogGyro
  (JNIEnv* env, jclass, jint id)
{
  if (id != HAL_kInvalidHandle) {
    HAL_FreeAnalogGyro((HAL_GyroHandle)id);
  }
}

/*
 * Class:     edu_wpi_first_hal_AnalogGyroJNI
 * Method:    setAnalogGyroParameters
 * Signature: (IDDI)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_AnalogGyroJNI_setAnalogGyroParameters
  (JNIEnv* env, jclass, jint id, jdouble vPDPS, jdouble offset, jint center)
{
  int32_t status = 0;
  HAL_SetAnalogGyroParameters((HAL_GyroHandle)id, vPDPS, offset, center,
                              &status);
  CheckStatus(env, status);
}

/*
 * Class:     edu_wpi_first_hal_AnalogGyroJNI
 * Method:    setAnalogGyroVoltsPerDegreePerSecond
 * Signature: (ID)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_AnalogGyroJNI_setAnalogGyroVoltsPerDegreePerSecond
  (JNIEnv* env, jclass, jint id, jdouble vPDPS)
{
  int32_t status = 0;
  HAL_SetAnalogGyroVoltsPerDegreePerSecond((HAL_GyroHandle)id, vPDPS, &status);
  CheckStatus(env, status);
}

/*
 * Class:     edu_wpi_first_hal_AnalogGyroJNI
 * Method:    resetAnalogGyro
 * Signature: (I)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_AnalogGyroJNI_resetAnalogGyro
  (JNIEnv* env, jclass, jint id)
{
  int32_t status = 0;
  HAL_ResetAnalogGyro((HAL_GyroHandle)id, &status);
  CheckStatus(env, status);
}

/*
 * Class:     edu_wpi_first_hal_AnalogGyroJNI
 * Method:    calibrateAnalogGyro
 * Signature: (I)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_AnalogGyroJNI_calibrateAnalogGyro
  (JNIEnv* env, jclass, jint id)
{
  int32_t status = 0;
  HAL_CalibrateAnalogGyro((HAL_GyroHandle)id, &status);
  CheckStatus(env, status);
}

/*
 * Class:     edu_wpi_first_hal_AnalogGyroJNI
 * Method:    setAnalogGyroDeadband
 * Signature: (ID)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_AnalogGyroJNI_setAnalogGyroDeadband
  (JNIEnv* env, jclass, jint id, jdouble deadband)
{
  int32_t status = 0;
  HAL_SetAnalogGyroDeadband((HAL_GyroHandle)id, deadband, &status);
  CheckStatus(env, status);
}

/*
 * Class:     edu_wpi_first_hal_AnalogGyroJNI
 * Method:    getAnalogGyroAngle
 * Signature: (I)D
 */
JNIEXPORT jdouble JNICALL
Java_edu_wpi_first_hal_AnalogGyroJNI_getAnalogGyroAngle
  (JNIEnv* env, jclass, jint id)
{
  int32_t status = 0;
  jdouble value = HAL_GetAnalogGyroAngle((HAL_GyroHandle)id, &status);
  CheckStatus(env, status);
  return value;
}

/*
 * Class:     edu_wpi_first_hal_AnalogGyroJNI
 * Method:    getAnalogGyroRate
 * Signature: (I)D
 */
JNIEXPORT jdouble JNICALL
Java_edu_wpi_first_hal_AnalogGyroJNI_getAnalogGyroRate
  (JNIEnv* env, jclass, jint id)
{
  int32_t status = 0;
  jdouble value = HAL_GetAnalogGyroRate((HAL_GyroHandle)id, &status);
  CheckStatus(env, status);
  return value;
}

/*
 * Class:     edu_wpi_first_hal_AnalogGyroJNI
 * Method:    getAnalogGyroOffset
 * Signature: (I)D
 */
JNIEXPORT jdouble JNICALL
Java_edu_wpi_first_hal_AnalogGyroJNI_getAnalogGyroOffset
  (JNIEnv* env, jclass, jint id)
{
  int32_t status = 0;
  jdouble value = HAL_GetAnalogGyroOffset((HAL_GyroHandle)id, &status);
  CheckStatus(env, status);
  return value;
}

/*
 * Class:     edu_wpi_first_hal_AnalogGyroJNI
 * Method:    getAnalogGyroCenter
 * Signature: (I)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_AnalogGyroJNI_getAnalogGyroCenter
  (JNIEnv* env, jclass, jint id)
{
  int32_t status = 0;
  jint value = HAL_GetAnalogGyroCenter((HAL_GyroHandle)id, &status);
  CheckStatus(env, status);
  return value;
}

}  // extern "C"
