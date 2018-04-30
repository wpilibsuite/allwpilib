/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include <jni.h>

#include "CallbackStore.h"
#include "MockData/AnalogTriggerData.h"
#include "edu_wpi_first_hal_sim_mockdata_AnalogTriggerDataJNI.h"

extern "C" {

JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_sim_mockdata_AnalogTriggerDataJNI_registerInitializedCallback(
    JNIEnv* env, jclass, jint index, jobject callback, jboolean initialNotify) {
  return sim::AllocateCallback(
      env, index, callback, initialNotify,
      &HALSIM_RegisterAnalogTriggerInitializedCallback);
}

JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_sim_mockdata_AnalogTriggerDataJNI_cancelInitializedCallback(
    JNIEnv* env, jclass, jint index, jint handle) {
  return sim::FreeCallback(env, handle, index,
                           &HALSIM_CancelAnalogTriggerInitializedCallback);
}

JNIEXPORT jboolean JNICALL
Java_edu_wpi_first_hal_sim_mockdata_AnalogTriggerDataJNI_getInitialized(
    JNIEnv*, jclass, jint index) {
  return HALSIM_GetAnalogTriggerInitialized(index);
}

JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_sim_mockdata_AnalogTriggerDataJNI_setInitialized(
    JNIEnv*, jclass, jint index, jboolean value) {
  HALSIM_SetAnalogTriggerInitialized(index, value);
}

JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_sim_mockdata_AnalogTriggerDataJNI_registerTriggerLowerBoundCallback(
    JNIEnv* env, jclass, jint index, jobject callback, jboolean initialNotify) {
  return sim::AllocateCallback(
      env, index, callback, initialNotify,
      &HALSIM_RegisterAnalogTriggerTriggerLowerBoundCallback);
}

JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_sim_mockdata_AnalogTriggerDataJNI_cancelTriggerLowerBoundCallback(
    JNIEnv* env, jclass, jint index, jint handle) {
  return sim::FreeCallback(
      env, handle, index, &HALSIM_CancelAnalogTriggerTriggerLowerBoundCallback);
}

JNIEXPORT jdouble JNICALL
Java_edu_wpi_first_hal_sim_mockdata_AnalogTriggerDataJNI_getTriggerLowerBound(
    JNIEnv*, jclass, jint index) {
  return HALSIM_GetAnalogTriggerTriggerLowerBound(index);
}

JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_sim_mockdata_AnalogTriggerDataJNI_setTriggerLowerBound(
    JNIEnv*, jclass, jint index, jdouble value) {
  HALSIM_SetAnalogTriggerTriggerLowerBound(index, value);
}

JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_sim_mockdata_AnalogTriggerDataJNI_registerTriggerUpperBoundCallback(
    JNIEnv* env, jclass, jint index, jobject callback, jboolean initialNotify) {
  return sim::AllocateCallback(
      env, index, callback, initialNotify,
      &HALSIM_RegisterAnalogTriggerTriggerUpperBoundCallback);
}

JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_sim_mockdata_AnalogTriggerDataJNI_cancelTriggerUpperBoundCallback(
    JNIEnv* env, jclass, jint index, jint handle) {
  return sim::FreeCallback(
      env, handle, index, &HALSIM_CancelAnalogTriggerTriggerUpperBoundCallback);
}

JNIEXPORT jdouble JNICALL
Java_edu_wpi_first_hal_sim_mockdata_AnalogTriggerDataJNI_getTriggerUpperBound(
    JNIEnv*, jclass, jint index) {
  return HALSIM_GetAnalogTriggerTriggerUpperBound(index);
}

JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_sim_mockdata_AnalogTriggerDataJNI_setTriggerUpperBound(
    JNIEnv*, jclass, jint index, jdouble value) {
  HALSIM_SetAnalogTriggerTriggerUpperBound(index, value);
}

JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_sim_mockdata_AnalogTriggerDataJNI_resetData(JNIEnv*,
                                                                   jclass,
                                                                   jint index) {
  HALSIM_ResetAnalogTriggerData(index);
}

}  // extern "C"
