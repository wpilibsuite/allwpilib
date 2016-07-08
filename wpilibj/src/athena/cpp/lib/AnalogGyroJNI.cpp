/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2016. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include <assert.h>
#include <jni.h>
#include "Log.h"

#include "edu_wpi_first_wpilibj_hal_AnalogGyroJNI.h"

#include "HAL/AnalogGyro.h"
#include "HALUtil.h"

// set the logging level
TLogLevel analogGyroJNILogLevel = logWARNING;

#define ANALOGGYROJNI_LOG(level)     \
  if (level > analogGyroJNILogLevel) \
    ;                           \
  else                          \
  Log().Get(level)

extern "C" {
/*
 * Class:     edu_wpi_first_wpilibj_hal_AnalogGyroJNI
 * Method:    initializeAnalogGyro
 * Signature: (I)I
 */
JNIEXPORT jint JNICALL Java_edu_wpi_first_wpilibj_hal_AnalogGyroJNI_initializeAnalogGyro(
    JNIEnv* env, jclass, jint id) {
  ANALOGGYROJNI_LOG(logDEBUG) << "Calling ANALOGGYROJNI initializeAnalogGyro";
  ANALOGGYROJNI_LOG(logDEBUG) << "Analog Input Handle = " << (HalAnalogInputHandle)id;
  int32_t status = 0;
  HalGyroHandle handle = initializeAnalogGyro((HalAnalogInputHandle)id, &status);
  ANALOGGYROJNI_LOG(logDEBUG) << "Status = " << status;
  ANALOGGYROJNI_LOG(logDEBUG) << "Gyro Handle = " << handle;
  return (jint) handle;
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_AnalogGyroJNI
 * Method:    setupAnalogGyro
 * Signature: (I)V
 */
JNIEXPORT void JNICALL Java_edu_wpi_first_wpilibj_hal_AnalogGyroJNI_setupAnalogGyro(
    JNIEnv* env, jclass, jint id) {
  ANALOGGYROJNI_LOG(logDEBUG) << "Calling ANALOGGYROJNI setupAnalogGyro";
  ANALOGGYROJNI_LOG(logDEBUG) << "Gyro Handle = " << (HalGyroHandle)id;
  int32_t status = 0;
  setupAnalogGyro((HalGyroHandle)id, &status);
  ANALOGGYROJNI_LOG(logDEBUG) << "Status = " << status;
  CheckStatus(env, status);
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_AnalogGyroJNI
 * Method:    freeAnalogGyro
 * Signature: (I)V
 */
JNIEXPORT void JNICALL Java_edu_wpi_first_wpilibj_hal_AnalogGyroJNI_freeAnalogGyro(
    JNIEnv* env, jclass, jint id) {
  ANALOGGYROJNI_LOG(logDEBUG) << "Calling ANALOGGYROJNI freeAnalogGyro";
  ANALOGGYROJNI_LOG(logDEBUG) << "Gyro Handle = " << (HalGyroHandle)id;
  freeAnalogGyro((HalGyroHandle)id);
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_AnalogGyroJNI
 * Method:    setAnalogGyroParameters
 * Signature: (IFFI)V
 */
JNIEXPORT void JNICALL Java_edu_wpi_first_wpilibj_hal_AnalogGyroJNI_setAnalogGyroParameters(
    JNIEnv* env, jclass, jint id, jfloat vPDPS, jfloat offset, jint center) {
  ANALOGGYROJNI_LOG(logDEBUG) << "Calling ANALOGGYROJNI setAnalogGyroParameters";
  ANALOGGYROJNI_LOG(logDEBUG) << "Gyro Handle = " << (HalGyroHandle)id;
  int32_t status = 0;
  setAnalogGyroParameters((HalGyroHandle)id, vPDPS, offset, center, &status);
  ANALOGGYROJNI_LOG(logDEBUG) << "Status = " << status;
  CheckStatus(env, status);
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_AnalogGyroJNI
 * Method:    setAnalogGyroVoltsPerDegreePerSecond
 * Signature: (IF)V
 */
JNIEXPORT void JNICALL Java_edu_wpi_first_wpilibj_hal_AnalogGyroJNI_setAnalogGyroVoltsPerDegreePerSecond(
    JNIEnv* env, jclass, jint id, jfloat vPDPS) {
  ANALOGGYROJNI_LOG(logDEBUG) << "Calling ANALOGGYROJNI setAnalogGyroVoltsPerDegreePerSecond";
  ANALOGGYROJNI_LOG(logDEBUG) << "Gyro Handle = " << (HalGyroHandle)id;
  ANALOGGYROJNI_LOG(logDEBUG) << "vPDPS = " << vPDPS;
  int32_t status = 0;
  setAnalogGyroVoltsPerDegreePerSecond((HalGyroHandle)id, vPDPS, &status);
  ANALOGGYROJNI_LOG(logDEBUG) << "Status = " << status;
  CheckStatus(env, status);
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_AnalogGyroJNI
 * Method:    resetAnalogGyro
 * Signature: (I)V
 */
JNIEXPORT void JNICALL Java_edu_wpi_first_wpilibj_hal_AnalogGyroJNI_resetAnalogGyro(
    JNIEnv* env, jclass, jint id) {
  ANALOGGYROJNI_LOG(logDEBUG) << "Calling ANALOGGYROJNI resetAnalogGyro";
  ANALOGGYROJNI_LOG(logDEBUG) << "Gyro Handle = " << (HalGyroHandle)id;
  int32_t status = 0;
  resetAnalogGyro((HalGyroHandle)id, &status);
  ANALOGGYROJNI_LOG(logDEBUG) << "Status = " << status;
  CheckStatus(env, status);
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_AnalogGyroJNI
 * Method:    calibrateAnalogGyro
 * Signature: (I)V
 */
JNIEXPORT void JNICALL Java_edu_wpi_first_wpilibj_hal_AnalogGyroJNI_calibrateAnalogGyro(
    JNIEnv* env, jclass, jint id) {
  ANALOGGYROJNI_LOG(logDEBUG) << "Calling ANALOGGYROJNI calibrateAnalogGyro";
  ANALOGGYROJNI_LOG(logDEBUG) << "Gyro Handle = " << (HalGyroHandle)id;
  int32_t status = 0;
  calibrateAnalogGyro((HalGyroHandle)id, &status);
  ANALOGGYROJNI_LOG(logDEBUG) << "Status = " << status;
  CheckStatus(env, status);
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_AnalogGyroJNI
 * Method:    setAnalogGyroDeadband
 * Signature: (IF)V
 */
JNIEXPORT void JNICALL Java_edu_wpi_first_wpilibj_hal_AnalogGyroJNI_setAnalogGyroDeadband(
    JNIEnv* env, jclass, jint id, jfloat deadband) {
  ANALOGGYROJNI_LOG(logDEBUG) << "Calling ANALOGGYROJNI setAnalogGyroDeadband";
  ANALOGGYROJNI_LOG(logDEBUG) << "Gyro Handle = " << (HalGyroHandle)id;
  int32_t status = 0;
  setAnalogGyroDeadband((HalGyroHandle)id, deadband, &status);
  ANALOGGYROJNI_LOG(logDEBUG) << "Status = " << status;
  CheckStatus(env, status);
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_AnalogGyroJNI
 * Method:    getAnalogGyroAngle
 * Signature: (I)F
 */
JNIEXPORT jfloat JNICALL Java_edu_wpi_first_wpilibj_hal_AnalogGyroJNI_getAnalogGyroAngle(
    JNIEnv* env, jclass, jint id) {
  ANALOGGYROJNI_LOG(logDEBUG) << "Calling ANALOGGYROJNI getAnalogGyroAngle";
  ANALOGGYROJNI_LOG(logDEBUG) << "Gyro Handle = " << (HalGyroHandle)id;
  int32_t status = 0;
  jfloat value = getAnalogGyroAngle((HalGyroHandle)id, &status);
  ANALOGGYROJNI_LOG(logDEBUG) << "Status = " << status;
  ANALOGGYROJNI_LOG(logDEBUG) << "Result = " << value;
  CheckStatus(env, status);
  return value;
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_AnalogGyroJNI
 * Method:    getAnalogGyroRate
 * Signature: (I)D
 */
JNIEXPORT jdouble JNICALL Java_edu_wpi_first_wpilibj_hal_AnalogGyroJNI_getAnalogGyroRate(
    JNIEnv* env, jclass, jint id) {
  ANALOGGYROJNI_LOG(logDEBUG) << "Calling ANALOGGYROJNI getAnalogGyroRate";
  ANALOGGYROJNI_LOG(logDEBUG) << "Gyro Handle = " << (HalGyroHandle)id;
  int32_t status = 0;
  jdouble value = getAnalogGyroRate((HalGyroHandle)id, &status);
  ANALOGGYROJNI_LOG(logDEBUG) << "Status = " << status;
  ANALOGGYROJNI_LOG(logDEBUG) << "Result = " << value;
  CheckStatus(env, status);
  return value;
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_AnalogGyroJNI
 * Method:    getAnalogGyroOffset
 * Signature: (I)F
 */
JNIEXPORT jfloat JNICALL Java_edu_wpi_first_wpilibj_hal_AnalogGyroJNI_getAnalogGyroOffset(
    JNIEnv* env, jclass, jint id) {
  ANALOGGYROJNI_LOG(logDEBUG) << "Calling ANALOGGYROJNI getAnalogGyroOffset";
  ANALOGGYROJNI_LOG(logDEBUG) << "Gyro Handle = " << (HalGyroHandle)id;
  int32_t status = 0;
  jfloat value = getAnalogGyroOffset((HalGyroHandle)id, &status);
  ANALOGGYROJNI_LOG(logDEBUG) << "Status = " << status;
  ANALOGGYROJNI_LOG(logDEBUG) << "Result = " << value;
  CheckStatus(env, status);
  return value;
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_AnalogGyroJNI
 * Method:    getAnalogGyroCenter
 * Signature: (I)I
 */
JNIEXPORT jint JNICALL Java_edu_wpi_first_wpilibj_hal_AnalogGyroJNI_getAnalogGyroCenter(
    JNIEnv* env, jclass, jint id) {
  ANALOGGYROJNI_LOG(logDEBUG) << "Calling ANALOGGYROJNI getAnalogGyroCenter";
  ANALOGGYROJNI_LOG(logDEBUG) << "Gyro Handle = " << (HalGyroHandle)id;
  int32_t status = 0;
  jint value = getAnalogGyroCenter((HalGyroHandle)id, &status);
  ANALOGGYROJNI_LOG(logDEBUG) << "Status = " << status;
  ANALOGGYROJNI_LOG(logDEBUG) << "Result = " << value;
  CheckStatus(env, status);
  return value;
}

}  // extern "C"
