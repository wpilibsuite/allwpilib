/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include <jni.h>

#include "CallbackStore.h"
#include "edu_wpi_first_hal_sim_mockdata_AnalogInDataJNI.h"
#include "mockdata/AnalogInData.h"

extern "C" {

/*
 * Class:     edu_wpi_first_hal_sim_mockdata_AnalogInDataJNI
 * Method:    registerInitializedCallback
 * Signature: (ILjava/lang/Object;Z)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_sim_mockdata_AnalogInDataJNI_registerInitializedCallback
  (JNIEnv* env, jclass, jint index, jobject callback, jboolean initialNotify)
{
  return sim::AllocateCallback(env, index, callback, initialNotify,
                               &HALSIM_RegisterAnalogInInitializedCallback);
}

/*
 * Class:     edu_wpi_first_hal_sim_mockdata_AnalogInDataJNI
 * Method:    cancelInitializedCallback
 * Signature: (II)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_sim_mockdata_AnalogInDataJNI_cancelInitializedCallback
  (JNIEnv* env, jclass, jint index, jint handle)
{
  return sim::FreeCallback(env, handle, index,
                           &HALSIM_CancelAnalogInInitializedCallback);
}

/*
 * Class:     edu_wpi_first_hal_sim_mockdata_AnalogInDataJNI
 * Method:    getInitialized
 * Signature: (I)Z
 */
JNIEXPORT jboolean JNICALL
Java_edu_wpi_first_hal_sim_mockdata_AnalogInDataJNI_getInitialized
  (JNIEnv*, jclass, jint index)
{
  return HALSIM_GetAnalogInInitialized(index);
}

/*
 * Class:     edu_wpi_first_hal_sim_mockdata_AnalogInDataJNI
 * Method:    setInitialized
 * Signature: (IZ)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_sim_mockdata_AnalogInDataJNI_setInitialized
  (JNIEnv*, jclass, jint index, jboolean value)
{
  HALSIM_SetAnalogInInitialized(index, value);
}

/*
 * Class:     edu_wpi_first_hal_sim_mockdata_AnalogInDataJNI
 * Method:    registerAverageBitsCallback
 * Signature: (ILjava/lang/Object;Z)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_sim_mockdata_AnalogInDataJNI_registerAverageBitsCallback
  (JNIEnv* env, jclass, jint index, jobject callback, jboolean initialNotify)
{
  return sim::AllocateCallback(env, index, callback, initialNotify,
                               &HALSIM_RegisterAnalogInAverageBitsCallback);
}

/*
 * Class:     edu_wpi_first_hal_sim_mockdata_AnalogInDataJNI
 * Method:    cancelAverageBitsCallback
 * Signature: (II)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_sim_mockdata_AnalogInDataJNI_cancelAverageBitsCallback
  (JNIEnv* env, jclass, jint index, jint handle)
{
  return sim::FreeCallback(env, handle, index,
                           &HALSIM_CancelAnalogInAverageBitsCallback);
}

/*
 * Class:     edu_wpi_first_hal_sim_mockdata_AnalogInDataJNI
 * Method:    getAverageBits
 * Signature: (I)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_sim_mockdata_AnalogInDataJNI_getAverageBits
  (JNIEnv*, jclass, jint index)
{
  return HALSIM_GetAnalogInAverageBits(index);
}

/*
 * Class:     edu_wpi_first_hal_sim_mockdata_AnalogInDataJNI
 * Method:    setAverageBits
 * Signature: (II)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_sim_mockdata_AnalogInDataJNI_setAverageBits
  (JNIEnv*, jclass, jint index, jint value)
{
  HALSIM_SetAnalogInAverageBits(index, value);
}

/*
 * Class:     edu_wpi_first_hal_sim_mockdata_AnalogInDataJNI
 * Method:    registerOversampleBitsCallback
 * Signature: (ILjava/lang/Object;Z)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_sim_mockdata_AnalogInDataJNI_registerOversampleBitsCallback
  (JNIEnv* env, jclass, jint index, jobject callback, jboolean initialNotify)
{
  return sim::AllocateCallback(env, index, callback, initialNotify,
                               &HALSIM_RegisterAnalogInOversampleBitsCallback);
}

/*
 * Class:     edu_wpi_first_hal_sim_mockdata_AnalogInDataJNI
 * Method:    cancelOversampleBitsCallback
 * Signature: (II)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_sim_mockdata_AnalogInDataJNI_cancelOversampleBitsCallback
  (JNIEnv* env, jclass, jint index, jint handle)
{
  return sim::FreeCallback(env, handle, index,
                           &HALSIM_CancelAnalogInOversampleBitsCallback);
}

/*
 * Class:     edu_wpi_first_hal_sim_mockdata_AnalogInDataJNI
 * Method:    getOversampleBits
 * Signature: (I)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_sim_mockdata_AnalogInDataJNI_getOversampleBits
  (JNIEnv*, jclass, jint index)
{
  return HALSIM_GetAnalogInOversampleBits(index);
}

/*
 * Class:     edu_wpi_first_hal_sim_mockdata_AnalogInDataJNI
 * Method:    setOversampleBits
 * Signature: (II)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_sim_mockdata_AnalogInDataJNI_setOversampleBits
  (JNIEnv*, jclass, jint index, jint value)
{
  HALSIM_SetAnalogInOversampleBits(index, value);
}

/*
 * Class:     edu_wpi_first_hal_sim_mockdata_AnalogInDataJNI
 * Method:    registerVoltageCallback
 * Signature: (ILjava/lang/Object;Z)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_sim_mockdata_AnalogInDataJNI_registerVoltageCallback
  (JNIEnv* env, jclass, jint index, jobject callback, jboolean initialNotify)
{
  return sim::AllocateCallback(env, index, callback, initialNotify,
                               &HALSIM_RegisterAnalogInVoltageCallback);
}

/*
 * Class:     edu_wpi_first_hal_sim_mockdata_AnalogInDataJNI
 * Method:    cancelVoltageCallback
 * Signature: (II)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_sim_mockdata_AnalogInDataJNI_cancelVoltageCallback
  (JNIEnv* env, jclass, jint index, jint handle)
{
  return sim::FreeCallback(env, handle, index,
                           &HALSIM_CancelAnalogInVoltageCallback);
}

/*
 * Class:     edu_wpi_first_hal_sim_mockdata_AnalogInDataJNI
 * Method:    getVoltage
 * Signature: (I)D
 */
JNIEXPORT jdouble JNICALL
Java_edu_wpi_first_hal_sim_mockdata_AnalogInDataJNI_getVoltage
  (JNIEnv*, jclass, jint index)
{
  return HALSIM_GetAnalogInVoltage(index);
}

/*
 * Class:     edu_wpi_first_hal_sim_mockdata_AnalogInDataJNI
 * Method:    setVoltage
 * Signature: (ID)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_sim_mockdata_AnalogInDataJNI_setVoltage
  (JNIEnv*, jclass, jint index, jdouble value)
{
  HALSIM_SetAnalogInVoltage(index, value);
}

/*
 * Class:     edu_wpi_first_hal_sim_mockdata_AnalogInDataJNI
 * Method:    registerAccumulatorInitializedCallback
 * Signature: (ILjava/lang/Object;Z)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_sim_mockdata_AnalogInDataJNI_registerAccumulatorInitializedCallback
  (JNIEnv* env, jclass, jint index, jobject callback, jboolean initialNotify)
{
  return sim::AllocateCallback(
      env, index, callback, initialNotify,
      &HALSIM_RegisterAnalogInAccumulatorInitializedCallback);
}

/*
 * Class:     edu_wpi_first_hal_sim_mockdata_AnalogInDataJNI
 * Method:    cancelAccumulatorInitializedCallback
 * Signature: (II)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_sim_mockdata_AnalogInDataJNI_cancelAccumulatorInitializedCallback
  (JNIEnv* env, jclass, jint index, jint handle)
{
  return sim::FreeCallback(
      env, handle, index, &HALSIM_CancelAnalogInAccumulatorInitializedCallback);
}

/*
 * Class:     edu_wpi_first_hal_sim_mockdata_AnalogInDataJNI
 * Method:    getAccumulatorInitialized
 * Signature: (I)Z
 */
JNIEXPORT jboolean JNICALL
Java_edu_wpi_first_hal_sim_mockdata_AnalogInDataJNI_getAccumulatorInitialized
  (JNIEnv*, jclass, jint index)
{
  return HALSIM_GetAnalogInAccumulatorInitialized(index);
}

/*
 * Class:     edu_wpi_first_hal_sim_mockdata_AnalogInDataJNI
 * Method:    setAccumulatorInitialized
 * Signature: (IZ)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_sim_mockdata_AnalogInDataJNI_setAccumulatorInitialized
  (JNIEnv*, jclass, jint index, jboolean value)
{
  HALSIM_SetAnalogInAccumulatorInitialized(index, value);
}

/*
 * Class:     edu_wpi_first_hal_sim_mockdata_AnalogInDataJNI
 * Method:    registerAccumulatorValueCallback
 * Signature: (ILjava/lang/Object;Z)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_sim_mockdata_AnalogInDataJNI_registerAccumulatorValueCallback
  (JNIEnv* env, jclass, jint index, jobject callback, jboolean initialNotify)
{
  return sim::AllocateCallback(
      env, index, callback, initialNotify,
      &HALSIM_RegisterAnalogInAccumulatorValueCallback);
}

/*
 * Class:     edu_wpi_first_hal_sim_mockdata_AnalogInDataJNI
 * Method:    cancelAccumulatorValueCallback
 * Signature: (II)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_sim_mockdata_AnalogInDataJNI_cancelAccumulatorValueCallback
  (JNIEnv* env, jclass, jint index, jint handle)
{
  return sim::FreeCallback(env, handle, index,
                           &HALSIM_CancelAnalogInAccumulatorValueCallback);
}

/*
 * Class:     edu_wpi_first_hal_sim_mockdata_AnalogInDataJNI
 * Method:    getAccumulatorValue
 * Signature: (I)J
 */
JNIEXPORT jlong JNICALL
Java_edu_wpi_first_hal_sim_mockdata_AnalogInDataJNI_getAccumulatorValue
  (JNIEnv*, jclass, jint index)
{
  return HALSIM_GetAnalogInAccumulatorValue(index);
}

/*
 * Class:     edu_wpi_first_hal_sim_mockdata_AnalogInDataJNI
 * Method:    setAccumulatorValue
 * Signature: (IJ)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_sim_mockdata_AnalogInDataJNI_setAccumulatorValue
  (JNIEnv*, jclass, jint index, jlong value)
{
  HALSIM_SetAnalogInAccumulatorValue(index, value);
}

/*
 * Class:     edu_wpi_first_hal_sim_mockdata_AnalogInDataJNI
 * Method:    registerAccumulatorCountCallback
 * Signature: (ILjava/lang/Object;Z)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_sim_mockdata_AnalogInDataJNI_registerAccumulatorCountCallback
  (JNIEnv* env, jclass, jint index, jobject callback, jboolean initialNotify)
{
  return sim::AllocateCallback(
      env, index, callback, initialNotify,
      &HALSIM_RegisterAnalogInAccumulatorCountCallback);
}

/*
 * Class:     edu_wpi_first_hal_sim_mockdata_AnalogInDataJNI
 * Method:    cancelAccumulatorCountCallback
 * Signature: (II)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_sim_mockdata_AnalogInDataJNI_cancelAccumulatorCountCallback
  (JNIEnv* env, jclass, jint index, jint handle)
{
  return sim::FreeCallback(env, handle, index,
                           &HALSIM_CancelAnalogInAccumulatorCountCallback);
}

/*
 * Class:     edu_wpi_first_hal_sim_mockdata_AnalogInDataJNI
 * Method:    getAccumulatorCount
 * Signature: (I)J
 */
JNIEXPORT jlong JNICALL
Java_edu_wpi_first_hal_sim_mockdata_AnalogInDataJNI_getAccumulatorCount
  (JNIEnv*, jclass, jint index)
{
  return HALSIM_GetAnalogInAccumulatorCount(index);
}

/*
 * Class:     edu_wpi_first_hal_sim_mockdata_AnalogInDataJNI
 * Method:    setAccumulatorCount
 * Signature: (IJ)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_sim_mockdata_AnalogInDataJNI_setAccumulatorCount
  (JNIEnv*, jclass, jint index, jlong value)
{
  HALSIM_SetAnalogInAccumulatorCount(index, value);
}

/*
 * Class:     edu_wpi_first_hal_sim_mockdata_AnalogInDataJNI
 * Method:    registerAccumulatorCenterCallback
 * Signature: (ILjava/lang/Object;Z)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_sim_mockdata_AnalogInDataJNI_registerAccumulatorCenterCallback
  (JNIEnv* env, jclass, jint index, jobject callback, jboolean initialNotify)
{
  return sim::AllocateCallback(
      env, index, callback, initialNotify,
      &HALSIM_RegisterAnalogInAccumulatorCenterCallback);
}

/*
 * Class:     edu_wpi_first_hal_sim_mockdata_AnalogInDataJNI
 * Method:    cancelAccumulatorCenterCallback
 * Signature: (II)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_sim_mockdata_AnalogInDataJNI_cancelAccumulatorCenterCallback
  (JNIEnv* env, jclass, jint index, jint handle)
{
  return sim::FreeCallback(env, handle, index,
                           &HALSIM_CancelAnalogInAccumulatorCenterCallback);
}

/*
 * Class:     edu_wpi_first_hal_sim_mockdata_AnalogInDataJNI
 * Method:    getAccumulatorCenter
 * Signature: (I)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_sim_mockdata_AnalogInDataJNI_getAccumulatorCenter
  (JNIEnv*, jclass, jint index)
{
  return HALSIM_GetAnalogInAccumulatorCenter(index);
}

/*
 * Class:     edu_wpi_first_hal_sim_mockdata_AnalogInDataJNI
 * Method:    setAccumulatorCenter
 * Signature: (II)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_sim_mockdata_AnalogInDataJNI_setAccumulatorCenter
  (JNIEnv*, jclass, jint index, jint value)
{
  HALSIM_SetAnalogInAccumulatorCenter(index, value);
}

/*
 * Class:     edu_wpi_first_hal_sim_mockdata_AnalogInDataJNI
 * Method:    registerAccumulatorDeadbandCallback
 * Signature: (ILjava/lang/Object;Z)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_sim_mockdata_AnalogInDataJNI_registerAccumulatorDeadbandCallback
  (JNIEnv* env, jclass, jint index, jobject callback, jboolean initialNotify)
{
  return sim::AllocateCallback(
      env, index, callback, initialNotify,
      &HALSIM_RegisterAnalogInAccumulatorDeadbandCallback);
}

/*
 * Class:     edu_wpi_first_hal_sim_mockdata_AnalogInDataJNI
 * Method:    cancelAccumulatorDeadbandCallback
 * Signature: (II)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_sim_mockdata_AnalogInDataJNI_cancelAccumulatorDeadbandCallback
  (JNIEnv* env, jclass, jint index, jint handle)
{
  return sim::FreeCallback(env, handle, index,
                           &HALSIM_CancelAnalogInAccumulatorDeadbandCallback);
}

/*
 * Class:     edu_wpi_first_hal_sim_mockdata_AnalogInDataJNI
 * Method:    getAccumulatorDeadband
 * Signature: (I)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_sim_mockdata_AnalogInDataJNI_getAccumulatorDeadband
  (JNIEnv*, jclass, jint index)
{
  return HALSIM_GetAnalogInAccumulatorDeadband(index);
}

/*
 * Class:     edu_wpi_first_hal_sim_mockdata_AnalogInDataJNI
 * Method:    setAccumulatorDeadband
 * Signature: (II)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_sim_mockdata_AnalogInDataJNI_setAccumulatorDeadband
  (JNIEnv*, jclass, jint index, jint value)
{
  HALSIM_SetAnalogInAccumulatorDeadband(index, value);
}

/*
 * Class:     edu_wpi_first_hal_sim_mockdata_AnalogInDataJNI
 * Method:    resetData
 * Signature: (I)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_sim_mockdata_AnalogInDataJNI_resetData
  (JNIEnv*, jclass, jint index)
{
  HALSIM_ResetAnalogInData(index);
}

}  // extern "C"
