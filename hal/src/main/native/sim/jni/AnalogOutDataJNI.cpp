/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include <jni.h>

#include "CallbackStore.h"
#include "MockData/AnalogOutData.h"
#include "edu_wpi_first_hal_sim_mockdata_AnalogOutDataJNI.h"

extern "C" {

JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_sim_mockdata_AnalogOutDataJNI_registerVoltageCallback(
    JNIEnv* env, jclass, jint index, jobject callback, jboolean initialNotify) {
  return sim::AllocateCallback(env, index, callback, initialNotify,
                               &HALSIM_RegisterAnalogOutVoltageCallback);
}

JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_sim_mockdata_AnalogOutDataJNI_cancelVoltageCallback(
    JNIEnv* env, jclass, jint index, jint handle) {
  return sim::FreeCallback(env, handle, index,
                           &HALSIM_CancelAnalogOutVoltageCallback);
}

JNIEXPORT jdouble JNICALL
Java_edu_wpi_first_hal_sim_mockdata_AnalogOutDataJNI_getVoltage(JNIEnv*, jclass,
                                                                jint index) {
  return HALSIM_GetAnalogOutVoltage(index);
}

JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_sim_mockdata_AnalogOutDataJNI_setVoltage(JNIEnv*, jclass,
                                                                jint index,
                                                                jdouble value) {
  HALSIM_SetAnalogOutVoltage(index, value);
}

JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_sim_mockdata_AnalogOutDataJNI_registerInitializedCallback(
    JNIEnv* env, jclass, jint index, jobject callback, jboolean initialNotify) {
  return sim::AllocateCallback(env, index, callback, initialNotify,
                               &HALSIM_RegisterAnalogOutInitializedCallback);
}

JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_sim_mockdata_AnalogOutDataJNI_cancelInitializedCallback(
    JNIEnv* env, jclass, jint index, jint handle) {
  return sim::FreeCallback(env, handle, index,
                           &HALSIM_CancelAnalogOutInitializedCallback);
}

JNIEXPORT jboolean JNICALL
Java_edu_wpi_first_hal_sim_mockdata_AnalogOutDataJNI_getInitialized(
    JNIEnv*, jclass, jint index) {
  return HALSIM_GetAnalogOutInitialized(index);
}

JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_sim_mockdata_AnalogOutDataJNI_setInitialized(
    JNIEnv*, jclass, jint index, jboolean value) {
  HALSIM_SetAnalogOutInitialized(index, value);
}

JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_sim_mockdata_AnalogOutDataJNI_resetData(JNIEnv*, jclass,
                                                               jint index) {
  HALSIM_ResetAnalogOutData(index);
}

}  // extern "C"
