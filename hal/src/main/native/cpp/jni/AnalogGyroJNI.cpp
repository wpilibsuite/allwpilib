/*----------------------------------------------------------------------------*/
/* Copyright (c) 2016-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include <jni.h>

#include <cassert>

#include "HALUtil.h"
#include "edu_wpi_first_hal_AnalogGyroJNI.h"
#include "hal/AnalogGyro.h"
#include "hal/cpp/Log.h"
#include "hal/handles/HandlesInternal.h"

using namespace frc;

// set the logging level
TLogLevel analogGyroJNILogLevel = logWARNING;

#define ANALOGGYROJNI_LOG(level)     \
  if (level > analogGyroJNILogLevel) \
    ;                                \
  else                               \
    Log().Get(level)

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
  ANALOGGYROJNI_LOG(logDEBUG) << "Calling ANALOGGYROJNI initializeAnalogGyro";
  ANALOGGYROJNI_LOG(logDEBUG)
      << "Analog Input Handle = " << (HAL_AnalogInputHandle)id;
  int32_t status = 0;
  HAL_GyroHandle handle =
      HAL_InitializeAnalogGyro((HAL_AnalogInputHandle)id, &status);
  ANALOGGYROJNI_LOG(logDEBUG) << "Status = " << status;
  ANALOGGYROJNI_LOG(logDEBUG) << "Gyro Handle = " << handle;
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
  ANALOGGYROJNI_LOG(logDEBUG) << "Calling ANALOGGYROJNI setupAnalogGyro";
  ANALOGGYROJNI_LOG(logDEBUG) << "Gyro Handle = " << (HAL_GyroHandle)id;
  int32_t status = 0;
  HAL_SetupAnalogGyro((HAL_GyroHandle)id, &status);
  ANALOGGYROJNI_LOG(logDEBUG) << "Status = " << status;
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
  ANALOGGYROJNI_LOG(logDEBUG) << "Calling ANALOGGYROJNI freeAnalogGyro";
  ANALOGGYROJNI_LOG(logDEBUG) << "Gyro Handle = " << (HAL_GyroHandle)id;
  HAL_FreeAnalogGyro((HAL_GyroHandle)id);
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
  ANALOGGYROJNI_LOG(logDEBUG)
      << "Calling ANALOGGYROJNI setAnalogGyroParameters";
  ANALOGGYROJNI_LOG(logDEBUG) << "Gyro Handle = " << (HAL_GyroHandle)id;
  int32_t status = 0;
  HAL_SetAnalogGyroParameters((HAL_GyroHandle)id, vPDPS, offset, center,
                              &status);
  ANALOGGYROJNI_LOG(logDEBUG) << "Status = " << status;
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
  ANALOGGYROJNI_LOG(logDEBUG)
      << "Calling ANALOGGYROJNI setAnalogGyroVoltsPerDegreePerSecond";
  ANALOGGYROJNI_LOG(logDEBUG) << "Gyro Handle = " << (HAL_GyroHandle)id;
  ANALOGGYROJNI_LOG(logDEBUG) << "vPDPS = " << vPDPS;
  int32_t status = 0;
  HAL_SetAnalogGyroVoltsPerDegreePerSecond((HAL_GyroHandle)id, vPDPS, &status);
  ANALOGGYROJNI_LOG(logDEBUG) << "Status = " << status;
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
  ANALOGGYROJNI_LOG(logDEBUG) << "Calling ANALOGGYROJNI resetAnalogGyro";
  ANALOGGYROJNI_LOG(logDEBUG) << "Gyro Handle = " << (HAL_GyroHandle)id;
  int32_t status = 0;
  HAL_ResetAnalogGyro((HAL_GyroHandle)id, &status);
  ANALOGGYROJNI_LOG(logDEBUG) << "Status = " << status;
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
  ANALOGGYROJNI_LOG(logDEBUG) << "Calling ANALOGGYROJNI calibrateAnalogGyro";
  ANALOGGYROJNI_LOG(logDEBUG) << "Gyro Handle = " << (HAL_GyroHandle)id;
  int32_t status = 0;
  HAL_CalibrateAnalogGyro((HAL_GyroHandle)id, &status);
  ANALOGGYROJNI_LOG(logDEBUG) << "Status = " << status;
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
  ANALOGGYROJNI_LOG(logDEBUG) << "Calling ANALOGGYROJNI setAnalogGyroDeadband";
  ANALOGGYROJNI_LOG(logDEBUG) << "Gyro Handle = " << (HAL_GyroHandle)id;
  int32_t status = 0;
  HAL_SetAnalogGyroDeadband((HAL_GyroHandle)id, deadband, &status);
  ANALOGGYROJNI_LOG(logDEBUG) << "Status = " << status;
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
  ANALOGGYROJNI_LOG(logDEBUG) << "Calling ANALOGGYROJNI getAnalogGyroAngle";
  ANALOGGYROJNI_LOG(logDEBUG) << "Gyro Handle = " << (HAL_GyroHandle)id;
  int32_t status = 0;
  jdouble value = HAL_GetAnalogGyroAngle((HAL_GyroHandle)id, &status);
  ANALOGGYROJNI_LOG(logDEBUG) << "Status = " << status;
  ANALOGGYROJNI_LOG(logDEBUG) << "Result = " << value;
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
  ANALOGGYROJNI_LOG(logDEBUG) << "Calling ANALOGGYROJNI getAnalogGyroRate";
  ANALOGGYROJNI_LOG(logDEBUG) << "Gyro Handle = " << (HAL_GyroHandle)id;
  int32_t status = 0;
  jdouble value = HAL_GetAnalogGyroRate((HAL_GyroHandle)id, &status);
  ANALOGGYROJNI_LOG(logDEBUG) << "Status = " << status;
  ANALOGGYROJNI_LOG(logDEBUG) << "Result = " << value;
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
  ANALOGGYROJNI_LOG(logDEBUG) << "Calling ANALOGGYROJNI getAnalogGyroOffset";
  ANALOGGYROJNI_LOG(logDEBUG) << "Gyro Handle = " << (HAL_GyroHandle)id;
  int32_t status = 0;
  jdouble value = HAL_GetAnalogGyroOffset((HAL_GyroHandle)id, &status);
  ANALOGGYROJNI_LOG(logDEBUG) << "Status = " << status;
  ANALOGGYROJNI_LOG(logDEBUG) << "Result = " << value;
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
  ANALOGGYROJNI_LOG(logDEBUG) << "Calling ANALOGGYROJNI getAnalogGyroCenter";
  ANALOGGYROJNI_LOG(logDEBUG) << "Gyro Handle = " << (HAL_GyroHandle)id;
  int32_t status = 0;
  jint value = HAL_GetAnalogGyroCenter((HAL_GyroHandle)id, &status);
  ANALOGGYROJNI_LOG(logDEBUG) << "Status = " << status;
  ANALOGGYROJNI_LOG(logDEBUG) << "Result = " << value;
  CheckStatus(env, status);
  return value;
}

}  // extern "C"
