/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2016. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include <jni.h>
#include "edu_wpi_first_wpilibj_hal_PowerJNI.h"
#include "HAL/Power.hpp"
#include "HALUtil.h"

extern "C" {

/*
 * Class:     edu_wpi_first_wpilibj_hal_PowerJNI
 * Method:    getVinVoltage
 * Signature: ()F
 */
JNIEXPORT jfloat JNICALL Java_edu_wpi_first_wpilibj_hal_PowerJNI_getVinVoltage
  (JNIEnv * env, jclass)
{
	int32_t status = 0;
	float val = getVinVoltage(&status);
  CheckStatus(env, status);
  return val;
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_PowerJNI
 * Method:    getVinCurrent
 * Signature: ()F
 */
JNIEXPORT jfloat JNICALL Java_edu_wpi_first_wpilibj_hal_PowerJNI_getVinCurrent
  (JNIEnv * env, jclass)
{
	int32_t status = 0;
	float val = getVinCurrent(&status);
  CheckStatus(env, status);
  return val;
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_PowerJNI
 * Method:    getUserVoltage6V
 * Signature: ()F
 */
JNIEXPORT jfloat JNICALL Java_edu_wpi_first_wpilibj_hal_PowerJNI_getUserVoltage6V
  (JNIEnv * env, jclass)
{
	int32_t status = 0;
	float val = getUserVoltage6V(&status);
  CheckStatus(env, status);
  return val;
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_PowerJNI
 * Method:    getUserCurrent6V
 * Signature: ()F
 */
JNIEXPORT jfloat JNICALL Java_edu_wpi_first_wpilibj_hal_PowerJNI_getUserCurrent6V
  (JNIEnv * env, jclass)
{
	int32_t status = 0;
	float val = getUserCurrent6V(&status);
  CheckStatus(env, status);
  return val;
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_PowerJNI
 * Method:    getUserActive6V
 * Signature: ()Z
 */
JNIEXPORT jboolean JNICALL Java_edu_wpi_first_wpilibj_hal_PowerJNI_getUserActive6V
  (JNIEnv * env, jclass)
{
	int32_t status = 0;
	bool val = getUserActive6V(&status);
  CheckStatus(env, status);
  return val;
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_PowerJNI
 * Method:    getUserCurrentFaults6V
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_edu_wpi_first_wpilibj_hal_PowerJNI_getUserCurrentFaults6V
  (JNIEnv * env, jclass)
{
	int32_t status = 0;
	int val = getUserCurrentFaults6V(&status);
  CheckStatus(env, status);
  return val;
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_PowerJNI
 * Method:    getUserVoltage5V
 * Signature: ()F
 */
JNIEXPORT jfloat JNICALL Java_edu_wpi_first_wpilibj_hal_PowerJNI_getUserVoltage5V
  (JNIEnv * env, jclass)
{
	int32_t status = 0;
	float val = getUserVoltage5V(&status);
  CheckStatus(env, status);
  return val;
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_PowerJNI
 * Method:    getUserCurrent5V
 * Signature: ()F
 */
JNIEXPORT jfloat JNICALL Java_edu_wpi_first_wpilibj_hal_PowerJNI_getUserCurrent5V
  (JNIEnv * env, jclass)
{
	int32_t status = 0;
	float val = getUserCurrent5V(&status);
  CheckStatus(env, status);
  return val;
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_PowerJNI
 * Method:    getUserActive5V
 * Signature: ()Z
 */
JNIEXPORT jboolean JNICALL Java_edu_wpi_first_wpilibj_hal_PowerJNI_getUserActive5V
  (JNIEnv * env, jclass)
{
	int32_t status = 0;
	bool val = getUserActive5V(&status);
  CheckStatus(env, status);
  return val;
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_PowerJNI
 * Method:    getUserCurrentFaults5V
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_edu_wpi_first_wpilibj_hal_PowerJNI_getUserCurrentFaults5V
  (JNIEnv * env, jclass)
{
	int32_t status = 0;
	int val = getUserCurrentFaults5V(&status);
  CheckStatus(env, status);
  return val;
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_PowerJNI
 * Method:    getUserVoltage3V3
 * Signature: ()F
 */
JNIEXPORT jfloat JNICALL Java_edu_wpi_first_wpilibj_hal_PowerJNI_getUserVoltage3V3
  (JNIEnv * env, jclass)
{
	int32_t status = 0;
	float val = getUserVoltage3V3(&status);
  CheckStatus(env, status);
  return val;
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_PowerJNI
 * Method:    getUserCurrent3V3
 * Signature: ()F
 */
JNIEXPORT jfloat JNICALL Java_edu_wpi_first_wpilibj_hal_PowerJNI_getUserCurrent3V3
  (JNIEnv * env, jclass)
{
	int32_t status = 0;
	float val = getUserCurrent3V3(&status);
  CheckStatus(env, status);
  return val;
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_PowerJNI
 * Method:    getUserActive3V3
 * Signature: ()Z
 */
JNIEXPORT jboolean JNICALL Java_edu_wpi_first_wpilibj_hal_PowerJNI_getUserActive3V3
  (JNIEnv * env, jclass)
{
	int32_t status = 0;
	bool val = getUserActive3V3(&status);
  CheckStatus(env, status);
  return val;
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_PowerJNI
 * Method:    getUserCurrentFaults3V3
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_edu_wpi_first_wpilibj_hal_PowerJNI_getUserCurrentFaults3V3
  (JNIEnv * env, jclass)
{
	int32_t status = 0;
	int val = getUserCurrentFaults3V3(&status);
  CheckStatus(env, status);
  return val;
}

}  // extern "C"
