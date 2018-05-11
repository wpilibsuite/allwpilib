/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include <jni.h>

#include "CallbackStore.h"
#include "MockData/SPIAccelerometerData.h"
#include "edu_wpi_first_hal_sim_mockdata_SPIAccelerometerDataJNI.h"

extern "C" {

JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_sim_mockdata_SPIAccelerometerDataJNI_registerActiveCallback(
    JNIEnv* env, jclass, jint index, jobject callback, jboolean initialNotify) {
  return sim::AllocateCallback(env, index, callback, initialNotify,
                               &HALSIM_RegisterSPIAccelerometerActiveCallback);
}

JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_sim_mockdata_SPIAccelerometerDataJNI_cancelActiveCallback(
    JNIEnv* env, jclass, jint index, jint handle) {
  return sim::FreeCallback(env, handle, index,
                           &HALSIM_CancelSPIAccelerometerActiveCallback);
}

JNIEXPORT jboolean JNICALL
Java_edu_wpi_first_hal_sim_mockdata_SPIAccelerometerDataJNI_getActive(
    JNIEnv*, jclass, jint index) {
  return HALSIM_GetSPIAccelerometerActive(index);
}

JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_sim_mockdata_SPIAccelerometerDataJNI_setActive(
    JNIEnv*, jclass, jint index, jboolean value) {
  HALSIM_SetSPIAccelerometerActive(index, value);
}

JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_sim_mockdata_SPIAccelerometerDataJNI_registerRangeCallback(
    JNIEnv* env, jclass, jint index, jobject callback, jboolean initialNotify) {
  return sim::AllocateCallback(env, index, callback, initialNotify,
                               &HALSIM_RegisterSPIAccelerometerRangeCallback);
}

JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_sim_mockdata_SPIAccelerometerDataJNI_cancelRangeCallback(
    JNIEnv* env, jclass, jint index, jint handle) {
  return sim::FreeCallback(env, handle, index,
                           &HALSIM_CancelSPIAccelerometerRangeCallback);
}

JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_sim_mockdata_SPIAccelerometerDataJNI_getRange(
    JNIEnv*, jclass, jint index) {
  return HALSIM_GetSPIAccelerometerRange(index);
}

JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_sim_mockdata_SPIAccelerometerDataJNI_setRange(
    JNIEnv*, jclass, jint index, jint value) {
  HALSIM_SetSPIAccelerometerRange(index, value);
}

JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_sim_mockdata_SPIAccelerometerDataJNI_registerXCallback(
    JNIEnv* env, jclass, jint index, jobject callback, jboolean initialNotify) {
  return sim::AllocateCallback(env, index, callback, initialNotify,
                               &HALSIM_RegisterSPIAccelerometerXCallback);
}

JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_sim_mockdata_SPIAccelerometerDataJNI_cancelXCallback(
    JNIEnv* env, jclass, jint index, jint handle) {
  return sim::FreeCallback(env, handle, index,
                           &HALSIM_CancelSPIAccelerometerXCallback);
}

JNIEXPORT jdouble JNICALL
Java_edu_wpi_first_hal_sim_mockdata_SPIAccelerometerDataJNI_getX(JNIEnv*,
                                                                 jclass,
                                                                 jint index) {
  return HALSIM_GetSPIAccelerometerX(index);
}

JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_sim_mockdata_SPIAccelerometerDataJNI_setX(
    JNIEnv*, jclass, jint index, jdouble value) {
  HALSIM_SetSPIAccelerometerX(index, value);
}

JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_sim_mockdata_SPIAccelerometerDataJNI_registerYCallback(
    JNIEnv* env, jclass, jint index, jobject callback, jboolean initialNotify) {
  return sim::AllocateCallback(env, index, callback, initialNotify,
                               &HALSIM_RegisterSPIAccelerometerYCallback);
}

JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_sim_mockdata_SPIAccelerometerDataJNI_cancelYCallback(
    JNIEnv* env, jclass, jint index, jint handle) {
  return sim::FreeCallback(env, handle, index,
                           &HALSIM_CancelSPIAccelerometerYCallback);
}

JNIEXPORT jdouble JNICALL
Java_edu_wpi_first_hal_sim_mockdata_SPIAccelerometerDataJNI_getY(JNIEnv*,
                                                                 jclass,
                                                                 jint index) {
  return HALSIM_GetSPIAccelerometerY(index);
}

JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_sim_mockdata_SPIAccelerometerDataJNI_setY(
    JNIEnv*, jclass, jint index, jdouble value) {
  HALSIM_SetSPIAccelerometerY(index, value);
}

JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_sim_mockdata_SPIAccelerometerDataJNI_registerZCallback(
    JNIEnv* env, jclass, jint index, jobject callback, jboolean initialNotify) {
  return sim::AllocateCallback(env, index, callback, initialNotify,
                               &HALSIM_RegisterSPIAccelerometerZCallback);
}

JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_sim_mockdata_SPIAccelerometerDataJNI_cancelZCallback(
    JNIEnv* env, jclass, jint index, jint handle) {
  return sim::FreeCallback(env, handle, index,
                           &HALSIM_CancelSPIAccelerometerZCallback);
}

JNIEXPORT jdouble JNICALL
Java_edu_wpi_first_hal_sim_mockdata_SPIAccelerometerDataJNI_getZ(JNIEnv*,
                                                                 jclass,
                                                                 jint index) {
  return HALSIM_GetSPIAccelerometerZ(index);
}

JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_sim_mockdata_SPIAccelerometerDataJNI_setZ(
    JNIEnv*, jclass, jint index, jdouble value) {
  HALSIM_SetSPIAccelerometerZ(index, value);
}

JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_sim_mockdata_SPIAccelerometerDataJNI_resetData(
    JNIEnv*, jclass, jint index) {
  HALSIM_ResetSPIAccelerometerData(index);
}

}  // extern "C"
