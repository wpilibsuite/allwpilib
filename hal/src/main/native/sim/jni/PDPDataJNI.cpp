/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include <jni.h>

#include "CallbackStore.h"
#include "MockData/PDPData.h"
#include "edu_wpi_first_hal_sim_mockdata_PDPDataJNI.h"

extern "C" {

JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_sim_mockdata_PDPDataJNI_registerInitializedCallback(
    JNIEnv* env, jclass, jint index, jobject callback, jboolean initialNotify) {
  return sim::AllocateCallback(env, index, callback, initialNotify,
                               &HALSIM_RegisterPDPInitializedCallback);
}

JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_sim_mockdata_PDPDataJNI_cancelInitializedCallback(
    JNIEnv* env, jclass, jint index, jint handle) {
  return sim::FreeCallback(env, handle, index,
                           &HALSIM_CancelPDPInitializedCallback);
}

JNIEXPORT jboolean JNICALL
Java_edu_wpi_first_hal_sim_mockdata_PDPDataJNI_getInitialized(JNIEnv*, jclass,
                                                              jint index) {
  return HALSIM_GetPDPInitialized(index);
}

JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_sim_mockdata_PDPDataJNI_setInitialized(JNIEnv*, jclass,
                                                              jint index,
                                                              jboolean value) {
  HALSIM_SetPDPInitialized(index, value);
}

JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_sim_mockdata_PDPDataJNI_registerTemperatureCallback(
    JNIEnv* env, jclass, jint index, jobject callback, jboolean initialNotify) {
  return sim::AllocateCallback(env, index, callback, initialNotify,
                               &HALSIM_RegisterPDPTemperatureCallback);
}

JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_sim_mockdata_PDPDataJNI_cancelTemperatureCallback(
    JNIEnv* env, jclass, jint index, jint handle) {
  return sim::FreeCallback(env, handle, index,
                           &HALSIM_CancelPDPTemperatureCallback);
}

JNIEXPORT jdouble JNICALL
Java_edu_wpi_first_hal_sim_mockdata_PDPDataJNI_getTemperature(JNIEnv*, jclass,
                                                              jint index) {
  return HALSIM_GetPDPTemperature(index);
}

JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_sim_mockdata_PDPDataJNI_setTemperature(JNIEnv*, jclass,
                                                              jint index,
                                                              jdouble value) {
  HALSIM_SetPDPTemperature(index, value);
}

JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_sim_mockdata_PDPDataJNI_registerVoltageCallback(
    JNIEnv* env, jclass, jint index, jobject callback, jboolean initialNotify) {
  return sim::AllocateCallback(env, index, callback, initialNotify,
                               &HALSIM_RegisterPDPVoltageCallback);
}

JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_sim_mockdata_PDPDataJNI_cancelVoltageCallback(
    JNIEnv* env, jclass, jint index, jint handle) {
  return sim::FreeCallback(env, handle, index,
                           &HALSIM_CancelPDPVoltageCallback);
}

JNIEXPORT jdouble JNICALL
Java_edu_wpi_first_hal_sim_mockdata_PDPDataJNI_getVoltage(JNIEnv*, jclass,
                                                          jint index) {
  return HALSIM_GetPDPVoltage(index);
}

JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_sim_mockdata_PDPDataJNI_setVoltage(JNIEnv*, jclass,
                                                          jint index,
                                                          jdouble value) {
  HALSIM_SetPDPVoltage(index, value);
}

JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_sim_mockdata_PDPDataJNI_registerCurrentCallback(
    JNIEnv* env, jclass, jint index, jint channel, jobject callback,
    jboolean initialNotify) {
  return sim::AllocateChannelCallback(env, index, channel, callback,
                                      initialNotify,
                                      &HALSIM_RegisterPDPCurrentCallback);
}

JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_sim_mockdata_PDPDataJNI_cancelCurrentCallback(
    JNIEnv* env, jclass, jint index, jint channel, jint handle) {
  return sim::FreeChannelCallback(env, handle, index, channel,
                                  &HALSIM_CancelPDPCurrentCallback);
}

JNIEXPORT jdouble JNICALL
Java_edu_wpi_first_hal_sim_mockdata_PDPDataJNI_getCurrent(JNIEnv*, jclass,
                                                          jint index,
                                                          jint channel) {
  return HALSIM_GetPDPCurrent(index, channel);
}

JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_sim_mockdata_PDPDataJNI_setCurrent(JNIEnv*, jclass,
                                                          jint index,
                                                          jint channel,
                                                          jdouble value) {
  HALSIM_SetPDPCurrent(index, channel, value);
}

JNIEXPORT void JNICALL Java_edu_wpi_first_hal_sim_mockdata_PDPDataJNI_resetData(
    JNIEnv*, jclass, jint index) {
  HALSIM_ResetPDPData(index);
}

}  // extern "C"
