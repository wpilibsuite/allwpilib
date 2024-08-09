// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <jni.h>

#include "HALUtil.h"
#include "edu_wpi_first_hal_PowerJNI.h"
#include "hal/Power.h"

using namespace hal;

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
 * Method:    setUserEnabled6V
 * Signature: (Z)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_PowerJNI_setUserEnabled6V
  (JNIEnv* env, jclass, jboolean enabled)
{
  int32_t status = 0;
  HAL_SetUserRailEnabled6V(enabled, &status);
  CheckStatus(env, status);
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
 * Method:    setUserEnabled5V
 * Signature: (Z)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_PowerJNI_setUserEnabled5V
  (JNIEnv* env, jclass, jboolean enabled)
{
  int32_t status = 0;
  HAL_SetUserRailEnabled5V(enabled, &status);
  CheckStatus(env, status);
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
 * Method:    setUserEnabled3V3
 * Signature: (Z)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_PowerJNI_setUserEnabled3V3
  (JNIEnv* env, jclass, jboolean enabled)
{
  int32_t status = 0;
  HAL_SetUserRailEnabled3V3(enabled, &status);
  CheckStatus(env, status);
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

/*
 * Class:     edu_wpi_first_hal_PowerJNI
 * Method:    resetUserCurrentFaults
 * Signature: ()V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_PowerJNI_resetUserCurrentFaults
  (JNIEnv* env, jclass)
{
  int32_t status = 0;
  HAL_ResetUserCurrentFaults(&status);
  CheckStatus(env, status);
}

/*
 * Class:     edu_wpi_first_hal_PowerJNI
 * Method:    setBrownoutVoltage
 * Signature: (D)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_PowerJNI_setBrownoutVoltage
  (JNIEnv* env, jclass, jdouble brownoutVoltage)
{
  int32_t status = 0;
  HAL_SetBrownoutVoltage(brownoutVoltage, &status);
  CheckStatus(env, status);
}

/*
 * Class:     edu_wpi_first_hal_PowerJNI
 * Method:    getBrownoutVoltage
 * Signature: ()D
 */
JNIEXPORT jdouble JNICALL
Java_edu_wpi_first_hal_PowerJNI_getBrownoutVoltage
  (JNIEnv* env, jclass)
{
  int32_t status = 0;
  double val = HAL_GetBrownoutVoltage(&status);
  CheckStatus(env, status);
  return val;
}

/*
 * Class:     edu_wpi_first_hal_PowerJNI
 * Method:    getCPUTemp
 * Signature: ()D
 */
JNIEXPORT jdouble JNICALL
Java_edu_wpi_first_hal_PowerJNI_getCPUTemp
  (JNIEnv* env, jclass)
{
  int32_t status = 0;
  double val = HAL_GetCPUTemp(&status);
  CheckStatus(env, status);
  return val;
}

}  // extern "C"
