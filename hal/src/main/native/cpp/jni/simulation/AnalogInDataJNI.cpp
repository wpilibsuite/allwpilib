// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <jni.h>

#include "CallbackStore.h"
#include "edu_wpi_first_hal_simulation_AnalogInDataJNI.h"
#include "hal/simulation/AnalogInData.h"

using namespace hal;

extern "C" {

/*
 * Class:     edu_wpi_first_hal_simulation_AnalogInDataJNI
 * Method:    registerInitializedCallback
 * Signature: (ILjava/lang/Object;Z)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_simulation_AnalogInDataJNI_registerInitializedCallback
  (JNIEnv* env, jclass, jint index, jobject callback, jboolean initialNotify)
{
  return sim::AllocateCallback(env, index, callback, initialNotify,
                               &HALSIM_RegisterAnalogInInitializedCallback);
}

/*
 * Class:     edu_wpi_first_hal_simulation_AnalogInDataJNI
 * Method:    cancelInitializedCallback
 * Signature: (II)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_simulation_AnalogInDataJNI_cancelInitializedCallback
  (JNIEnv* env, jclass, jint index, jint handle)
{
  return sim::FreeCallback(env, handle, index,
                           &HALSIM_CancelAnalogInInitializedCallback);
}

/*
 * Class:     edu_wpi_first_hal_simulation_AnalogInDataJNI
 * Method:    getInitialized
 * Signature: (I)Z
 */
JNIEXPORT jboolean JNICALL
Java_edu_wpi_first_hal_simulation_AnalogInDataJNI_getInitialized
  (JNIEnv*, jclass, jint index)
{
  return HALSIM_GetAnalogInInitialized(index);
}

/*
 * Class:     edu_wpi_first_hal_simulation_AnalogInDataJNI
 * Method:    setInitialized
 * Signature: (IZ)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_simulation_AnalogInDataJNI_setInitialized
  (JNIEnv*, jclass, jint index, jboolean value)
{
  HALSIM_SetAnalogInInitialized(index, value);
}

/*
 * Class:     edu_wpi_first_hal_simulation_AnalogInDataJNI
 * Method:    registerAverageBitsCallback
 * Signature: (ILjava/lang/Object;Z)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_simulation_AnalogInDataJNI_registerAverageBitsCallback
  (JNIEnv* env, jclass, jint index, jobject callback, jboolean initialNotify)
{
  return sim::AllocateCallback(env, index, callback, initialNotify,
                               &HALSIM_RegisterAnalogInAverageBitsCallback);
}

/*
 * Class:     edu_wpi_first_hal_simulation_AnalogInDataJNI
 * Method:    cancelAverageBitsCallback
 * Signature: (II)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_simulation_AnalogInDataJNI_cancelAverageBitsCallback
  (JNIEnv* env, jclass, jint index, jint handle)
{
  return sim::FreeCallback(env, handle, index,
                           &HALSIM_CancelAnalogInAverageBitsCallback);
}

/*
 * Class:     edu_wpi_first_hal_simulation_AnalogInDataJNI
 * Method:    getAverageBits
 * Signature: (I)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_simulation_AnalogInDataJNI_getAverageBits
  (JNIEnv*, jclass, jint index)
{
  return HALSIM_GetAnalogInAverageBits(index);
}

/*
 * Class:     edu_wpi_first_hal_simulation_AnalogInDataJNI
 * Method:    setAverageBits
 * Signature: (II)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_simulation_AnalogInDataJNI_setAverageBits
  (JNIEnv*, jclass, jint index, jint value)
{
  HALSIM_SetAnalogInAverageBits(index, value);
}

/*
 * Class:     edu_wpi_first_hal_simulation_AnalogInDataJNI
 * Method:    registerOversampleBitsCallback
 * Signature: (ILjava/lang/Object;Z)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_simulation_AnalogInDataJNI_registerOversampleBitsCallback
  (JNIEnv* env, jclass, jint index, jobject callback, jboolean initialNotify)
{
  return sim::AllocateCallback(env, index, callback, initialNotify,
                               &HALSIM_RegisterAnalogInOversampleBitsCallback);
}

/*
 * Class:     edu_wpi_first_hal_simulation_AnalogInDataJNI
 * Method:    cancelOversampleBitsCallback
 * Signature: (II)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_simulation_AnalogInDataJNI_cancelOversampleBitsCallback
  (JNIEnv* env, jclass, jint index, jint handle)
{
  return sim::FreeCallback(env, handle, index,
                           &HALSIM_CancelAnalogInOversampleBitsCallback);
}

/*
 * Class:     edu_wpi_first_hal_simulation_AnalogInDataJNI
 * Method:    getOversampleBits
 * Signature: (I)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_simulation_AnalogInDataJNI_getOversampleBits
  (JNIEnv*, jclass, jint index)
{
  return HALSIM_GetAnalogInOversampleBits(index);
}

/*
 * Class:     edu_wpi_first_hal_simulation_AnalogInDataJNI
 * Method:    setOversampleBits
 * Signature: (II)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_simulation_AnalogInDataJNI_setOversampleBits
  (JNIEnv*, jclass, jint index, jint value)
{
  HALSIM_SetAnalogInOversampleBits(index, value);
}

/*
 * Class:     edu_wpi_first_hal_simulation_AnalogInDataJNI
 * Method:    registerVoltageCallback
 * Signature: (ILjava/lang/Object;Z)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_simulation_AnalogInDataJNI_registerVoltageCallback
  (JNIEnv* env, jclass, jint index, jobject callback, jboolean initialNotify)
{
  return sim::AllocateCallback(env, index, callback, initialNotify,
                               &HALSIM_RegisterAnalogInVoltageCallback);
}

/*
 * Class:     edu_wpi_first_hal_simulation_AnalogInDataJNI
 * Method:    cancelVoltageCallback
 * Signature: (II)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_simulation_AnalogInDataJNI_cancelVoltageCallback
  (JNIEnv* env, jclass, jint index, jint handle)
{
  return sim::FreeCallback(env, handle, index,
                           &HALSIM_CancelAnalogInVoltageCallback);
}

/*
 * Class:     edu_wpi_first_hal_simulation_AnalogInDataJNI
 * Method:    getVoltage
 * Signature: (I)D
 */
JNIEXPORT jdouble JNICALL
Java_edu_wpi_first_hal_simulation_AnalogInDataJNI_getVoltage
  (JNIEnv*, jclass, jint index)
{
  return HALSIM_GetAnalogInVoltage(index);
}

/*
 * Class:     edu_wpi_first_hal_simulation_AnalogInDataJNI
 * Method:    setVoltage
 * Signature: (ID)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_simulation_AnalogInDataJNI_setVoltage
  (JNIEnv*, jclass, jint index, jdouble value)
{
  HALSIM_SetAnalogInVoltage(index, value);
}

/*
 * Class:     edu_wpi_first_hal_simulation_AnalogInDataJNI
 * Method:    resetData
 * Signature: (I)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_simulation_AnalogInDataJNI_resetData
  (JNIEnv*, jclass, jint index)
{
  HALSIM_ResetAnalogInData(index);
}

}  // extern "C"
