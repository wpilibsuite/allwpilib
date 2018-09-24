/*----------------------------------------------------------------------------*/
/* Copyright (c) 2016-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include <jni.h>

#include "HALUtil.h"
#include "edu_wpi_first_hal_PowerJNI.h"
#include "hal/Power.h"

using namespace frc;

extern "C" {

/*
 * Class:     edu_wpi_first_hal_PowerJNI
 * Method:    getVinVoltage
 * Signature: ()D
 */
JNIEXPORT jdouble JNICALL
Java_edu_wpi_first_hal_PowerJNI_getVinVoltage
  (JNIEnv* env, jclass)
{
  int32_t status = 0;
  double val = HAL_GetVinVoltage(&status);
  CheckStatus(env, status);
  return val;
}

/*
 * Class:     edu_wpi_first_hal_PowerJNI
 * Method:    getVinCurrent
 * Signature: ()D
 */
JNIEXPORT jdouble JNICALL
Java_edu_wpi_first_hal_PowerJNI_getVinCurrent
  (JNIEnv* env, jclass)
{
  int32_t status = 0;
  double val = HAL_GetVinCurrent(&status);
  CheckStatus(env, status);
  return val;
}

/*
 * Class:     edu_wpi_first_hal_PowerJNI
 * Method:    getUserVoltage6V
 * Signature: ()D
 */
JNIEXPORT jdouble JNICALL
Java_edu_wpi_first_hal_PowerJNI_getUserVoltage6V
  (JNIEnv* env, jclass)
{
  int32_t status = 0;
  double val = HAL_GetUserVoltage6V(&status);
  CheckStatus(env, status);
  return val;
}

/*
 * Class:     edu_wpi_first_hal_PowerJNI
 * Method:    getUserCurrent6V
 * Signature: ()D
 */
JNIEXPORT jdouble JNICALL
Java_edu_wpi_first_hal_PowerJNI_getUserCurrent6V
  (JNIEnv* env, jclass)
{
  int32_t status = 0;
  double val = HAL_GetUserCurrent6V(&status);
  CheckStatus(env, status);
  return val;
}

/*
 * Class:     edu_wpi_first_hal_PowerJNI
 * Method:    getUserActive6V
 * Signature: ()Z
 */
JNIEXPORT jboolean JNICALL
Java_edu_wpi_first_hal_PowerJNI_getUserActive6V
  (JNIEnv* env, jclass)
{
  int32_t status = 0;
  bool val = HAL_GetUserActive6V(&status);
  CheckStatus(env, status);
  return val;
}

/*
 * Class:     edu_wpi_first_hal_PowerJNI
 * Method:    getUserCurrentFaults6V
 * Signature: ()I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_PowerJNI_getUserCurrentFaults6V
  (JNIEnv* env, jclass)
{
  int32_t status = 0;
  int32_t val = HAL_GetUserCurrentFaults6V(&status);
  CheckStatus(env, status);
  return val;
}

/*
 * Class:     edu_wpi_first_hal_PowerJNI
 * Method:    getUserVoltage5V
 * Signature: ()D
 */
JNIEXPORT jdouble JNICALL
Java_edu_wpi_first_hal_PowerJNI_getUserVoltage5V
  (JNIEnv* env, jclass)
{
  int32_t status = 0;
  double val = HAL_GetUserVoltage5V(&status);
  CheckStatus(env, status);
  return val;
}

/*
 * Class:     edu_wpi_first_hal_PowerJNI
 * Method:    getUserCurrent5V
 * Signature: ()D
 */
JNIEXPORT jdouble JNICALL
Java_edu_wpi_first_hal_PowerJNI_getUserCurrent5V
  (JNIEnv* env, jclass)
{
  int32_t status = 0;
  double val = HAL_GetUserCurrent5V(&status);
  CheckStatus(env, status);
  return val;
}

/*
 * Class:     edu_wpi_first_hal_PowerJNI
 * Method:    getUserActive5V
 * Signature: ()Z
 */
JNIEXPORT jboolean JNICALL
Java_edu_wpi_first_hal_PowerJNI_getUserActive5V
  (JNIEnv* env, jclass)
{
  int32_t status = 0;
  bool val = HAL_GetUserActive5V(&status);
  CheckStatus(env, status);
  return val;
}

/*
 * Class:     edu_wpi_first_hal_PowerJNI
 * Method:    getUserCurrentFaults5V
 * Signature: ()I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_PowerJNI_getUserCurrentFaults5V
  (JNIEnv* env, jclass)
{
  int32_t status = 0;
  int32_t val = HAL_GetUserCurrentFaults5V(&status);
  CheckStatus(env, status);
  return val;
}

/*
 * Class:     edu_wpi_first_hal_PowerJNI
 * Method:    getUserVoltage3V3
 * Signature: ()D
 */
JNIEXPORT jdouble JNICALL
Java_edu_wpi_first_hal_PowerJNI_getUserVoltage3V3
  (JNIEnv* env, jclass)
{
  int32_t status = 0;
  double val = HAL_GetUserVoltage3V3(&status);
  CheckStatus(env, status);
  return val;
}

/*
 * Class:     edu_wpi_first_hal_PowerJNI
 * Method:    getUserCurrent3V3
 * Signature: ()D
 */
JNIEXPORT jdouble JNICALL
Java_edu_wpi_first_hal_PowerJNI_getUserCurrent3V3
  (JNIEnv* env, jclass)
{
  int32_t status = 0;
  double val = HAL_GetUserCurrent3V3(&status);
  CheckStatus(env, status);
  return val;
}

/*
 * Class:     edu_wpi_first_hal_PowerJNI
 * Method:    getUserActive3V3
 * Signature: ()Z
 */
JNIEXPORT jboolean JNICALL
Java_edu_wpi_first_hal_PowerJNI_getUserActive3V3
  (JNIEnv* env, jclass)
{
  int32_t status = 0;
  bool val = HAL_GetUserActive3V3(&status);
  CheckStatus(env, status);
  return val;
}

/*
 * Class:     edu_wpi_first_hal_PowerJNI
 * Method:    getUserCurrentFaults3V3
 * Signature: ()I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_PowerJNI_getUserCurrentFaults3V3
  (JNIEnv* env, jclass)
{
  int32_t status = 0;
  int32_t val = HAL_GetUserCurrentFaults3V3(&status);
  CheckStatus(env, status);
  return val;
}

}  // extern "C"
