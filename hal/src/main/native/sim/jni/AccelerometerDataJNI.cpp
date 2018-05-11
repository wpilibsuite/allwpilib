/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include <jni.h>

#include "CallbackStore.h"
#include "MockData/AccelerometerData.h"
#include "edu_wpi_first_hal_sim_mockdata_AccelerometerDataJNI.h"

extern "C" {

JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_sim_mockdata_AccelerometerDataJNI_registerActiveCallback(
    JNIEnv* env, jclass, jint index, jobject callback, jboolean initialNotify) {
  return sim::AllocateCallback(env, index, callback, initialNotify,
                               &HALSIM_RegisterAccelerometerActiveCallback);
}

JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_sim_mockdata_AccelerometerDataJNI_cancelActiveCallback(
    JNIEnv* env, jclass, jint index, jint handle) {
  return sim::FreeCallback(env, handle, index,
                           &HALSIM_CancelAccelerometerActiveCallback);
}

JNIEXPORT jboolean JNICALL
Java_edu_wpi_first_hal_sim_mockdata_AccelerometerDataJNI_getActive(JNIEnv*,
                                                                   jclass,
                                                                   jint index) {
  return HALSIM_GetAccelerometerActive(index);
}

JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_sim_mockdata_AccelerometerDataJNI_setActive(
    JNIEnv*, jclass, jint index, jboolean value) {
  HALSIM_SetAccelerometerActive(index, value);
}

JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_sim_mockdata_AccelerometerDataJNI_registerRangeCallback(
    JNIEnv* env, jclass, jint index, jobject callback, jboolean initialNotify) {
  return sim::AllocateCallback(env, index, callback, initialNotify,
                               &HALSIM_RegisterAccelerometerRangeCallback);
}

JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_sim_mockdata_AccelerometerDataJNI_cancelRangeCallback(
    JNIEnv* env, jclass, jint index, jint handle) {
  return sim::FreeCallback(env, handle, index,
                           &HALSIM_CancelAccelerometerRangeCallback);
}

JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_sim_mockdata_AccelerometerDataJNI_getRange(JNIEnv*,
                                                                  jclass,
                                                                  jint index) {
  return HALSIM_GetAccelerometerRange(index);
}

JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_sim_mockdata_AccelerometerDataJNI_setRange(JNIEnv*,
                                                                  jclass,
                                                                  jint index,
                                                                  jint value) {
  HALSIM_SetAccelerometerRange(index,
                               static_cast<HAL_AccelerometerRange>(value));
}

JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_sim_mockdata_AccelerometerDataJNI_registerXCallback(
    JNIEnv* env, jclass, jint index, jobject callback, jboolean initialNotify) {
  return sim::AllocateCallback(env, index, callback, initialNotify,
                               &HALSIM_RegisterAccelerometerXCallback);
}

JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_sim_mockdata_AccelerometerDataJNI_cancelXCallback(
    JNIEnv* env, jclass, jint index, jint handle) {
  return sim::FreeCallback(env, handle, index,
                           &HALSIM_CancelAccelerometerXCallback);
}

JNIEXPORT jdouble JNICALL
Java_edu_wpi_first_hal_sim_mockdata_AccelerometerDataJNI_getX(JNIEnv*, jclass,
                                                              jint index) {
  return HALSIM_GetAccelerometerX(index);
}

JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_sim_mockdata_AccelerometerDataJNI_setX(JNIEnv*, jclass,
                                                              jint index,
                                                              jdouble value) {
  HALSIM_SetAccelerometerX(index, value);
}

JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_sim_mockdata_AccelerometerDataJNI_registerYCallback(
    JNIEnv* env, jclass, jint index, jobject callback, jboolean initialNotify) {
  return sim::AllocateCallback(env, index, callback, initialNotify,
                               &HALSIM_RegisterAccelerometerYCallback);
}

JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_sim_mockdata_AccelerometerDataJNI_cancelYCallback(
    JNIEnv* env, jclass, jint index, jint handle) {
  return sim::FreeCallback(env, handle, index,
                           &HALSIM_CancelAccelerometerYCallback);
}

JNIEXPORT jdouble JNICALL
Java_edu_wpi_first_hal_sim_mockdata_AccelerometerDataJNI_getY(JNIEnv*, jclass,
                                                              jint index) {
  return HALSIM_GetAccelerometerY(index);
}

JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_sim_mockdata_AccelerometerDataJNI_setY(JNIEnv*, jclass,
                                                              jint index,
                                                              jdouble value) {
  HALSIM_SetAccelerometerY(index, value);
}

JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_sim_mockdata_AccelerometerDataJNI_registerZCallback(
    JNIEnv* env, jclass, jint index, jobject callback, jboolean initialNotify) {
  return sim::AllocateCallback(env, index, callback, initialNotify,
                               &HALSIM_RegisterAccelerometerZCallback);
}

JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_sim_mockdata_AccelerometerDataJNI_cancelZCallback(
    JNIEnv* env, jclass, jint index, jint handle) {
  return sim::FreeCallback(env, handle, index,
                           &HALSIM_CancelAccelerometerZCallback);
}

JNIEXPORT jdouble JNICALL
Java_edu_wpi_first_hal_sim_mockdata_AccelerometerDataJNI_getZ(JNIEnv*, jclass,
                                                              jint index) {
  return HALSIM_GetAccelerometerZ(index);
}

JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_sim_mockdata_AccelerometerDataJNI_setZ(JNIEnv*, jclass,
                                                              jint index,
                                                              jdouble value) {
  HALSIM_SetAccelerometerZ(index, value);
}

JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_sim_mockdata_AccelerometerDataJNI_resetData(JNIEnv*,
                                                                   jclass,
                                                                   jint index) {
  HALSIM_ResetAccelerometerData(index);
}

}  // extern "C"
