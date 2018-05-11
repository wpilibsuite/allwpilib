/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include <jni.h>

#include "CallbackStore.h"
#include "MockData/AnalogInData.h"
#include "edu_wpi_first_hal_sim_mockdata_AnalogInDataJNI.h"

extern "C" {

JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_sim_mockdata_AnalogInDataJNI_registerInitializedCallback(
    JNIEnv* env, jclass, jint index, jobject callback, jboolean initialNotify) {
  return sim::AllocateCallback(env, index, callback, initialNotify,
                               &HALSIM_RegisterAnalogInInitializedCallback);
}

JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_sim_mockdata_AnalogInDataJNI_cancelInitializedCallback(
    JNIEnv* env, jclass, jint index, jint handle) {
  return sim::FreeCallback(env, handle, index,
                           &HALSIM_CancelAnalogInInitializedCallback);
}

JNIEXPORT jboolean JNICALL
Java_edu_wpi_first_hal_sim_mockdata_AnalogInDataJNI_getInitialized(JNIEnv*,
                                                                   jclass,
                                                                   jint index) {
  return HALSIM_GetAnalogInInitialized(index);
}

JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_sim_mockdata_AnalogInDataJNI_setInitialized(
    JNIEnv*, jclass, jint index, jboolean value) {
  HALSIM_SetAnalogInInitialized(index, value);
}

JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_sim_mockdata_AnalogInDataJNI_registerAverageBitsCallback(
    JNIEnv* env, jclass, jint index, jobject callback, jboolean initialNotify) {
  return sim::AllocateCallback(env, index, callback, initialNotify,
                               &HALSIM_RegisterAnalogInAverageBitsCallback);
}

JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_sim_mockdata_AnalogInDataJNI_cancelAverageBitsCallback(
    JNIEnv* env, jclass, jint index, jint handle) {
  return sim::FreeCallback(env, handle, index,
                           &HALSIM_CancelAnalogInAverageBitsCallback);
}

JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_sim_mockdata_AnalogInDataJNI_getAverageBits(JNIEnv*,
                                                                   jclass,
                                                                   jint index) {
  return HALSIM_GetAnalogInAverageBits(index);
}

JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_sim_mockdata_AnalogInDataJNI_setAverageBits(JNIEnv*,
                                                                   jclass,
                                                                   jint index,
                                                                   jint value) {
  HALSIM_SetAnalogInAverageBits(index, value);
}

JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_sim_mockdata_AnalogInDataJNI_registerOversampleBitsCallback(
    JNIEnv* env, jclass, jint index, jobject callback, jboolean initialNotify) {
  return sim::AllocateCallback(env, index, callback, initialNotify,
                               &HALSIM_RegisterAnalogInOversampleBitsCallback);
}

JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_sim_mockdata_AnalogInDataJNI_cancelOversampleBitsCallback(
    JNIEnv* env, jclass, jint index, jint handle) {
  return sim::FreeCallback(env, handle, index,
                           &HALSIM_CancelAnalogInOversampleBitsCallback);
}

JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_sim_mockdata_AnalogInDataJNI_getOversampleBits(
    JNIEnv*, jclass, jint index) {
  return HALSIM_GetAnalogInOversampleBits(index);
}

JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_sim_mockdata_AnalogInDataJNI_setOversampleBits(
    JNIEnv*, jclass, jint index, jint value) {
  HALSIM_SetAnalogInOversampleBits(index, value);
}

JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_sim_mockdata_AnalogInDataJNI_registerVoltageCallback(
    JNIEnv* env, jclass, jint index, jobject callback, jboolean initialNotify) {
  return sim::AllocateCallback(env, index, callback, initialNotify,
                               &HALSIM_RegisterAnalogInVoltageCallback);
}

JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_sim_mockdata_AnalogInDataJNI_cancelVoltageCallback(
    JNIEnv* env, jclass, jint index, jint handle) {
  return sim::FreeCallback(env, handle, index,
                           &HALSIM_CancelAnalogInVoltageCallback);
}

JNIEXPORT jdouble JNICALL
Java_edu_wpi_first_hal_sim_mockdata_AnalogInDataJNI_getVoltage(JNIEnv*, jclass,
                                                               jint index) {
  return HALSIM_GetAnalogInVoltage(index);
}

JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_sim_mockdata_AnalogInDataJNI_setVoltage(JNIEnv*, jclass,
                                                               jint index,
                                                               jdouble value) {
  HALSIM_SetAnalogInVoltage(index, value);
}

JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_sim_mockdata_AnalogInDataJNI_registerAccumulatorInitializedCallback(
    JNIEnv* env, jclass, jint index, jobject callback, jboolean initialNotify) {
  return sim::AllocateCallback(
      env, index, callback, initialNotify,
      &HALSIM_RegisterAnalogInAccumulatorInitializedCallback);
}

JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_sim_mockdata_AnalogInDataJNI_cancelAccumulatorInitializedCallback(
    JNIEnv* env, jclass, jint index, jint handle) {
  return sim::FreeCallback(
      env, handle, index, &HALSIM_CancelAnalogInAccumulatorInitializedCallback);
}

JNIEXPORT jboolean JNICALL
Java_edu_wpi_first_hal_sim_mockdata_AnalogInDataJNI_getAccumulatorInitialized(
    JNIEnv*, jclass, jint index) {
  return HALSIM_GetAnalogInAccumulatorInitialized(index);
}

JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_sim_mockdata_AnalogInDataJNI_setAccumulatorInitialized(
    JNIEnv*, jclass, jint index, jboolean value) {
  HALSIM_SetAnalogInAccumulatorInitialized(index, value);
}

JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_sim_mockdata_AnalogInDataJNI_registerAccumulatorValueCallback(
    JNIEnv* env, jclass, jint index, jobject callback, jboolean initialNotify) {
  return sim::AllocateCallback(
      env, index, callback, initialNotify,
      &HALSIM_RegisterAnalogInAccumulatorValueCallback);
}

JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_sim_mockdata_AnalogInDataJNI_cancelAccumulatorValueCallback(
    JNIEnv* env, jclass, jint index, jint handle) {
  return sim::FreeCallback(env, handle, index,
                           &HALSIM_CancelAnalogInAccumulatorValueCallback);
}

JNIEXPORT jlong JNICALL
Java_edu_wpi_first_hal_sim_mockdata_AnalogInDataJNI_getAccumulatorValue(
    JNIEnv*, jclass, jint index) {
  return HALSIM_GetAnalogInAccumulatorValue(index);
}

JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_sim_mockdata_AnalogInDataJNI_setAccumulatorValue(
    JNIEnv*, jclass, jint index, jlong value) {
  HALSIM_SetAnalogInAccumulatorValue(index, value);
}

JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_sim_mockdata_AnalogInDataJNI_registerAccumulatorCountCallback(
    JNIEnv* env, jclass, jint index, jobject callback, jboolean initialNotify) {
  return sim::AllocateCallback(
      env, index, callback, initialNotify,
      &HALSIM_RegisterAnalogInAccumulatorCountCallback);
}

JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_sim_mockdata_AnalogInDataJNI_cancelAccumulatorCountCallback(
    JNIEnv* env, jclass, jint index, jint handle) {
  return sim::FreeCallback(env, handle, index,
                           &HALSIM_CancelAnalogInAccumulatorCountCallback);
}

JNIEXPORT jlong JNICALL
Java_edu_wpi_first_hal_sim_mockdata_AnalogInDataJNI_getAccumulatorCount(
    JNIEnv*, jclass, jint index) {
  return HALSIM_GetAnalogInAccumulatorCount(index);
}

JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_sim_mockdata_AnalogInDataJNI_setAccumulatorCount(
    JNIEnv*, jclass, jint index, jlong value) {
  HALSIM_SetAnalogInAccumulatorCount(index, value);
}

JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_sim_mockdata_AnalogInDataJNI_registerAccumulatorCenterCallback(
    JNIEnv* env, jclass, jint index, jobject callback, jboolean initialNotify) {
  return sim::AllocateCallback(
      env, index, callback, initialNotify,
      &HALSIM_RegisterAnalogInAccumulatorCenterCallback);
}

JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_sim_mockdata_AnalogInDataJNI_cancelAccumulatorCenterCallback(
    JNIEnv* env, jclass, jint index, jint handle) {
  return sim::FreeCallback(env, handle, index,
                           &HALSIM_CancelAnalogInAccumulatorCenterCallback);
}

JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_sim_mockdata_AnalogInDataJNI_getAccumulatorCenter(
    JNIEnv*, jclass, jint index) {
  return HALSIM_GetAnalogInAccumulatorCenter(index);
}

JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_sim_mockdata_AnalogInDataJNI_setAccumulatorCenter(
    JNIEnv*, jclass, jint index, jint value) {
  HALSIM_SetAnalogInAccumulatorCenter(index, value);
}

JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_sim_mockdata_AnalogInDataJNI_registerAccumulatorDeadbandCallback(
    JNIEnv* env, jclass, jint index, jobject callback, jboolean initialNotify) {
  return sim::AllocateCallback(
      env, index, callback, initialNotify,
      &HALSIM_RegisterAnalogInAccumulatorDeadbandCallback);
}

JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_sim_mockdata_AnalogInDataJNI_cancelAccumulatorDeadbandCallback(
    JNIEnv* env, jclass, jint index, jint handle) {
  return sim::FreeCallback(env, handle, index,
                           &HALSIM_CancelAnalogInAccumulatorDeadbandCallback);
}

JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_sim_mockdata_AnalogInDataJNI_getAccumulatorDeadband(
    JNIEnv*, jclass, jint index) {
  return HALSIM_GetAnalogInAccumulatorDeadband(index);
}

JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_sim_mockdata_AnalogInDataJNI_setAccumulatorDeadband(
    JNIEnv*, jclass, jint index, jint value) {
  HALSIM_SetAnalogInAccumulatorDeadband(index, value);
}

JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_sim_mockdata_AnalogInDataJNI_resetData(JNIEnv*, jclass,
                                                              jint index) {
  HALSIM_ResetAnalogInData(index);
}

}  // extern "C"
