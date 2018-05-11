/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include <jni.h>

#include "CallbackStore.h"
#include "MockData/PWMData.h"
#include "edu_wpi_first_hal_sim_mockdata_PWMDataJNI.h"

extern "C" {

JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_sim_mockdata_PWMDataJNI_registerInitializedCallback(
    JNIEnv* env, jclass, jint index, jobject callback, jboolean initialNotify) {
  return sim::AllocateCallback(env, index, callback, initialNotify,
                               &HALSIM_RegisterPWMInitializedCallback);
}

JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_sim_mockdata_PWMDataJNI_cancelInitializedCallback(
    JNIEnv* env, jclass, jint index, jint handle) {
  return sim::FreeCallback(env, handle, index,
                           &HALSIM_CancelPWMInitializedCallback);
}

JNIEXPORT jboolean JNICALL
Java_edu_wpi_first_hal_sim_mockdata_PWMDataJNI_getInitialized(JNIEnv*, jclass,
                                                              jint index) {
  return HALSIM_GetPWMInitialized(index);
}

JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_sim_mockdata_PWMDataJNI_setInitialized(JNIEnv*, jclass,
                                                              jint index,
                                                              jboolean value) {
  HALSIM_SetPWMInitialized(index, value);
}

JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_sim_mockdata_PWMDataJNI_registerRawValueCallback(
    JNIEnv* env, jclass, jint index, jobject callback, jboolean initialNotify) {
  return sim::AllocateCallback(env, index, callback, initialNotify,
                               &HALSIM_RegisterPWMRawValueCallback);
}

JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_sim_mockdata_PWMDataJNI_cancelRawValueCallback(
    JNIEnv* env, jclass, jint index, jint handle) {
  return sim::FreeCallback(env, handle, index,
                           &HALSIM_CancelPWMRawValueCallback);
}

JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_sim_mockdata_PWMDataJNI_getRawValue(JNIEnv*, jclass,
                                                           jint index) {
  return HALSIM_GetPWMRawValue(index);
}

JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_sim_mockdata_PWMDataJNI_setRawValue(JNIEnv*, jclass,
                                                           jint index,
                                                           jint value) {
  HALSIM_SetPWMRawValue(index, value);
}

JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_sim_mockdata_PWMDataJNI_registerSpeedCallback(
    JNIEnv* env, jclass, jint index, jobject callback, jboolean initialNotify) {
  return sim::AllocateCallback(env, index, callback, initialNotify,
                               &HALSIM_RegisterPWMSpeedCallback);
}

JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_sim_mockdata_PWMDataJNI_cancelSpeedCallback(
    JNIEnv* env, jclass, jint index, jint handle) {
  return sim::FreeCallback(env, handle, index, &HALSIM_CancelPWMSpeedCallback);
}

JNIEXPORT jdouble JNICALL
Java_edu_wpi_first_hal_sim_mockdata_PWMDataJNI_getSpeed(JNIEnv*, jclass,
                                                        jint index) {
  return HALSIM_GetPWMSpeed(index);
}

JNIEXPORT void JNICALL Java_edu_wpi_first_hal_sim_mockdata_PWMDataJNI_setSpeed(
    JNIEnv*, jclass, jint index, jdouble value) {
  HALSIM_SetPWMSpeed(index, value);
}

JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_sim_mockdata_PWMDataJNI_registerPositionCallback(
    JNIEnv* env, jclass, jint index, jobject callback, jboolean initialNotify) {
  return sim::AllocateCallback(env, index, callback, initialNotify,
                               &HALSIM_RegisterPWMPositionCallback);
}

JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_sim_mockdata_PWMDataJNI_cancelPositionCallback(
    JNIEnv* env, jclass, jint index, jint handle) {
  return sim::FreeCallback(env, handle, index,
                           &HALSIM_CancelPWMPositionCallback);
}

JNIEXPORT jdouble JNICALL
Java_edu_wpi_first_hal_sim_mockdata_PWMDataJNI_getPosition(JNIEnv*, jclass,
                                                           jint index) {
  return HALSIM_GetPWMPosition(index);
}

JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_sim_mockdata_PWMDataJNI_setPosition(JNIEnv*, jclass,
                                                           jint index,
                                                           jdouble value) {
  HALSIM_SetPWMPosition(index, value);
}

JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_sim_mockdata_PWMDataJNI_registerPeriodScaleCallback(
    JNIEnv* env, jclass, jint index, jobject callback, jboolean initialNotify) {
  return sim::AllocateCallback(env, index, callback, initialNotify,
                               &HALSIM_RegisterPWMPeriodScaleCallback);
}

JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_sim_mockdata_PWMDataJNI_cancelPeriodScaleCallback(
    JNIEnv* env, jclass, jint index, jint handle) {
  return sim::FreeCallback(env, handle, index,
                           &HALSIM_CancelPWMPeriodScaleCallback);
}

JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_sim_mockdata_PWMDataJNI_getPeriodScale(JNIEnv*, jclass,
                                                              jint index) {
  return HALSIM_GetPWMPeriodScale(index);
}

JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_sim_mockdata_PWMDataJNI_setPeriodScale(JNIEnv*, jclass,
                                                              jint index,
                                                              jint value) {
  HALSIM_SetPWMPeriodScale(index, value);
}

JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_sim_mockdata_PWMDataJNI_registerZeroLatchCallback(
    JNIEnv* env, jclass, jint index, jobject callback, jboolean initialNotify) {
  return sim::AllocateCallback(env, index, callback, initialNotify,
                               &HALSIM_RegisterPWMZeroLatchCallback);
}

JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_sim_mockdata_PWMDataJNI_cancelZeroLatchCallback(
    JNIEnv* env, jclass, jint index, jint handle) {
  return sim::FreeCallback(env, handle, index,
                           &HALSIM_CancelPWMZeroLatchCallback);
}

JNIEXPORT jboolean JNICALL
Java_edu_wpi_first_hal_sim_mockdata_PWMDataJNI_getZeroLatch(JNIEnv*, jclass,
                                                            jint index) {
  return HALSIM_GetPWMZeroLatch(index);
}

JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_sim_mockdata_PWMDataJNI_setZeroLatch(JNIEnv*, jclass,
                                                            jint index,
                                                            jboolean value) {
  HALSIM_SetPWMZeroLatch(index, value);
}

JNIEXPORT void JNICALL Java_edu_wpi_first_hal_sim_mockdata_PWMDataJNI_resetData(
    JNIEnv*, jclass, jint index) {
  HALSIM_ResetPWMData(index);
}

}  // extern "C"
